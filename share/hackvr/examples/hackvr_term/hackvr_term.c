#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h> //PATH_MAX
#include "libtmt/tmt.h"

//these numbers include the space between characters
//apple401 font is this wide:
//#define FONTW 14
//#define FONTH 16

//epoch font is this wide:
#define FONTW 6
#define FONTH 10

//////////////////////////////////// hey. you can do some config here. either preload glyphs or re-read font file each time.
//comment out the next line if you want what is probably the slow version. might be safer and use less memory though.
//maybe just put the font file in ram. linux caches that crap anyway, right?
//testing showed preloading was at least 10x faster.
#define PRELOAD_GLYPHS
//only 8-bit characters atm.
//and we'll need to malloc the amount of lines each character will have.
//and malloc each line in that glyph...
//or just set it statically.

#ifdef PRELOAD_GLYPHS

#define MAXCHARACTER 256
#define MAXLINESPERGLYPH 16
#define MAXCHARSPERGLYPHLINE 256
char glyphs[MAXCHARACTER][MAXLINESPERGLYPH][MAXCHARSPERGLYPHLINE];

char font_file[PATH_MAX];

void preload_glyphs() {
  char line[MAXCHARSPERGLYPHLINE];//tmp storage
  FILE *fp;
  int charN;
  int i;
  for(i=0;i<MAXCHARACTER;i++) {
   glyphs[i][0][0]=0;//zero out the first byte of the first line of every glyph. we'll mark the end if we actually read something
  }
  if((fp=fopen(font_file,"r")) == NULL) {
    fprintf(stderr,"hackvr_term: failed to open font file: %s\n",font_file);
    exit(1);
  }
  for(i=0;fgets(line,sizeof(line)-1,fp) != 0;i++) {
    if(charN != strtol(line,0,16)) {
     i=0;
     charN=strtol(line,0,16);
    }
    strcpy(glyphs[charN][i],line);
    glyphs[charN][i+1][0]=0;//mark the end here. if we do it inside the conditional it won't mark the last one.
  }
  fclose(fp);
}

void hackvr_draw_character(int c,int r,const TMTCHAR *ch) {
  char *line;
  int i,ret=0;
  //printf("term_%02d_%02d addshape %d 4  -1 -3 0  5 -3 0  5 7 0  -1 7 0\n",c,r,ch->a.bg+15);
  for(i=0,line=glyphs[ch->c][0];line[0];i++,line=glyphs[ch->c][i]) {
    ret=1;
    printf("term_%02d_%02d addshape %d %s",c,r,ch->a.fg == -1 ? 17 : ch->a.fg + 15,line+strlen("XX addshape X"));
  }
  if(ret) {//if we drew something we should place it somewhere.
    printf("term_%02d_%02d move %d %d 0\n",c,r,c*FONTW,-r*FONTH);
  }
}

#else

void hackvr_draw_character(int c,int r,const TMTCHAR *ch) {//this is slow but allows you to change the font between each character... if you really wanted that.
 int i;
 FILE *fp;
 char str[16];
 char line[256];//whatever
 if(ch->c < 128) {
  snprintf(str,sizeof(str)-1,"%02lx",ch->c);
 } else {
  snprintf(str,sizeof(str)-1,"%08lx",ch->c);
 }
 if((fp=fopen(font_file,"r")) == NULL ) {
  fprintf(stderr,"# fail to open font\n");
  exit(1);
 }
 printf("term_%02d_%02d addshape %d 4  -1 -3 0  5 -3 0  5 7 0  -1 7 0\n",c,r,ch->a.bg+15);
 for(i=0;fgets(line,sizeof(line)-1,fp) != 0;i++) {
  if(!strncmp(str,line,2)) {
   printf("term_%02d_%02d addshape %d %s",c,r,ch->a.fg==-1?2:ch->a.fg+15,line+strlen("XX addshape X "));
  }
 }
 if(i) printf("term_%02d_%02d move %d %d 0\n",c,r,c*FONTW,-r*FONTH);
 fclose(fp);
}

#endif

void callback(tmt_msg_t m,TMT *vt, const void *a,void *vt_old) {
 static int cr_old=0;
 static int cc_old=0;
 const TMTSCREEN *s_old=tmt_screen(vt_old);
 const TMTSCREEN *s=tmt_screen(vt);
 const TMTPOINT *c=tmt_cursor(vt);
 const TMTCHAR *ch;
 switch(m) {
  case TMT_MSG_BELL:
   printf("term set global.beep\n");
   break;
  case TMT_MSG_UPDATE:
   for (size_t r = 0; r < s->nline; r++){
    if (s->lines[r]->dirty){
     for (size_t c = 0; c < s->ncol; c++){
      if(memcmp(&(s->lines[r]->chars[c]),&(s_old->lines[r]->chars[c]),sizeof(TMTCHAR))) {
       ch=&(s->lines[r]->chars[c]);
       printf("term_%02d_%02d deletegroup term_%02d_%02d\n",c,r,c,r);
       hackvr_draw_character(c,r,ch);
       memcpy(&(s_old->lines[r]->chars[c]),&(s->lines[r]->chars[c]),sizeof(TMTCHAR));
      }
     }
    }
   }
   break;
  case TMT_MSG_ANSWER://what does this do?
   fprintf(stderr,"ANSWER: %s\n",(const char *)a);
   break;
  case TMT_MSG_MOVED:
   printf("cursor move %d %d 0\n", (c->c - cc_old) * 5, - (c->r - cr_old) * 9);//calculate relative movement needed based on previous and current positions.
   cr_old=c->r;
   cc_old=c->c;
   break;
  case TMT_MSG_CURSOR:
   if(!strcmp(a,"t")) {
    printf("cursor deletegroup cursor\n");//just delete it and redraw the cursor.
    printf("cursor addshape 2 4  0 -2 0  4 -2 0  4 6 0  0 6 0\n");//let's pretend this is how cursors should be.
    printf("cursor move %d %d 0\n",c->c * 5,-c->r * 9);
   }
   if(!strcmp(a,"f")) {
    printf("cursor deletegroup cursor\n");
    cr_old=0;
    cc_old=0;
   }
   break;
  default:
   fprintf(stderr,"unhandled message type: %d\n",m);
 }
 return;
}

int main(int argc,char *argv[]) {
 char in[16];
 if(argc < 3) return fprintf(stderr,"usage: ./hackvr_term cols rows\n"),1;
 int r=atoi(argv[2]);
 int ret=0;
 int c=atoi(argv[1]);
 if(!getenv("PREFIX")) {
  fprintf(stderr,"hackvr_term: PREFIX is not set. I dunno where to find my fonts.\n");
  return 1;
 }
 snprintf(font_file,sizeof(font_file)-1,"%s/share/hackvr/font/default.hackvr",getenv("PREFIX"));
#ifdef PRELOAD_GLYPHS
 printf("# hackvr_term: preloading glyphs\n");
 preload_glyphs();//will exit on error
 printf("# hackvr_term: done.\n");
#endif
 setbuf(stdin,0);
 setbuf(stdout,0);
 TMT *vt_old = tmt_open(r,c,NULL,NULL,NULL);
 if(!vt_old) return fprintf(stderr,"failed to open tmt's virtual terminal for storage"),1;
 TMT *vt = tmt_open(r,c,callback,vt_old,NULL);
 if(!vt) return fprintf(stderr,"failed to open tmt's virtual terminal"),1;
 //read from stdin and write to terminal.
 while((ret=read(0,in,16)) != 0) {//16 bytes at a time work?
  if(ret == -1) {
   if(errno == EAGAIN) continue;
   break;
  }
  tmt_write(vt,in,ret);
 }
 tmt_close(vt);
 return 0;
}
