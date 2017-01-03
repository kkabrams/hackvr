#define _POSIX_C_SOURCE 200809L //for fileno and strdup
#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
//#include <sys/select.h> //code to use select instead of non-blocking is commented out. might decide to use it later.
#include <time.h>
#define __USE_GNU //for longer math constants
#include <math.h>

#include "config.h"
#include "common.h"
#ifdef GRAPHICAL
#include "graphics.h"
extern struct gra_global gra_global;
#endif

//TODO: optimizations
//TODO: store caches of cs_t, c2_t, and c3_t numbers.
//TODO: don't forget to remake gophervr with this.
//TODO: XSegment or line structs to store some shit?
//TODO: line and triangle intersection for finding what object was clicked on
//if I don't do hiliting I only need to calculate that upon click.

//TODO: will have to make some pixmaps get resized when the window does.
//for now set them to be as big as you think you'll ever resize the window to.

struct global global;

/* does not return the newline. */
char *read_line_hack(FILE *fp,int len) {
 short in;
 char *t;
 switch(in=fgetc(fp)) {
  case '\n':
   t=malloc(len+1);
   t[len]=0;
   return t;
  case -1:
   return 0;
  default:
   if((t=read_line_hack(fp,len+1))) t[len]=in;
   break;
 }
 return t;
}

//warning: clobbers input
//skips leading and trailing space.
//compresses multiple spaces to one.
//return length of array
char **line_splitter(char *line,int *rlen) {
 char **a;
 int len,i=0;
 len=1;//we're just counting how much we'll need the first loop through.
 for(i=0;line[i] && line[i] == ' ';i++);//skip leading space
 for(;line[i];len++) {
  for(;line[i] && line[i] != ' ';i++);//skip rest of data
  for(;line[i] && line[i] == ' ';i++);//skip rest of space
 }
 a=malloc(sizeof(char *) * len+1);
 a[len]=0;
 len=0;//reuse!
 for(i=0;line[i] && line[i] == ' ';i++);//skip leading space
 a[len]=line+i;
 for(;;) {
  for(;line[i] && line[i] != ' ';i++);//skip rest of data
  if(!line[i]) break;
  line[i++]=0;
  for(;line[i] && line[i] == ' ';i++);//skip rest of space
  if(!line[i]) break;
  a[++len]=line+i;
 }
 a[++len]=0;
 *rlen=len;
 return a;
}

int load_stdin() {
 struct c3_shape s;
// struct c3_line l;
 char *command;
 char *line;
 char *id;
 char **a;
 int len;
 int ret=0;
 int j,k,l;
 //struct timeval timeout;
 //fd_set master;
 //fd_set readfs;
 //FD_ZERO(&master);
 //FD_ZERO(&readfs);
 //FD_SET(0,&master);//just stdin.
 int i;//used to store the last triangle. even though I have a global for that. >_>

// printf("# entering load_stdin()\n");
 for(i=0;global.shape[i];i++) ;//hop to the end.
 fcntl(0,F_SETFL,O_NONBLOCK);
 if(feof(stdin))  {
  clearerr(stdin);
 }
 // readfs=master;
 // timeout.tv_sec=0;
 // timeout.tv_usec=1;
 // if((j=select(1,&readfs,0,0,&timeout)) == -1) {
 //  perror("select");
 //  return 0;
 // }
 // if(FD_ISSET(0,&readfs)) {
 while((line=read_line_hack(stdin,0))) {//load as long there's something to load
  if(*line == '#') return 0;
//  printf("# read command: %s\n",line);
  a=line_splitter(line,&len);
//  for(i=0;i<len;i++) {
//   printf("\"%s\" ",a[i]);
//  }
//  printf("\n");
  id=a[0];
  if(len > 1) {
   command=a[1];
  }
  if(len < 2) {
   if(!strcmp(id,"help")) {
#ifdef GRAPHICAL
    printf("# NOT built headless.\n");
#else
    printf("# built headless.\n");
#endif
    printf("# command format:\n");
    printf("# some_ID_or_nick_or_username command arguments\n");
    printf("# commands:\n");
    printf("#   deleteallexcept\n");
    printf("#   deletegroup\n");
    printf("#   assimilate\n");
    printf("#   renamegroup\n");
    printf("#   dump\n");
    printf("#   quit\n");
    printf("#   set\n");
    printf("#   addshape\n");
    printf("#   export\n");
    printf("#   scaleup\n");
    printf("#   move\n");
    printf("# that is all.\n");
    return 0;
   } else {
    printf("# ur not doing it right. '%s'\n",id);
    return 0;
   }
  }
  ret=1;
  if(!strcmp(command,"deleteallexcept")) {
   for(j=0;global.shape[j];j++) {//mark first. compress later.
    if(strcmp(global.shape[j]->id,a[2])) {//TODO: memory leak!
     //free(global.triangle[j]->id);
     //free(global.triangle[j]);
     global.shape[j]=0;
    }
   }
   l=0;
   for(k=0;k<j;k++) {
    while(global.shape[l] == 0 && l < j) l++;
    global.shape[k]=global.shape[l];
   }
   continue;
  }
  if(!strcmp(command,"deletegroup")) {
   for(j=0;global.shape[j];j++) {//mark first. compress later.
    if(!strcmp(global.shape[j]->id,s.id)) {//??? where is s.id supposed to be set from?
     free(global.shape[j]->id);
     free(global.shape[j]);
     global.shape[j]=0;
    }
   }
   l=0;
   for(k=0;k<j;k++) {
    while(global.shape[l] == 0 && l < j) l++;
    global.shape[k]=global.shape[l];
   }
   continue;
  }
  if(!strcmp(command,"assimilate")) {
   if(len == 3) {
    for(j=0;global.shape[j];j++) {
     if(!strcmp(global.shape[j]->id,a[2])) {
      free(global.shape[j]->id);
      global.shape[j]->id=strdup(id);
     }
    }
   }
   continue;
  }
  if(!strcmp(command,"renamegroup")) {
   if(len == 4) {
    for(j=0;global.shape[j];j++) {
     if(!strcmp(global.shape[j]->id,a[2])) {
      free(global.shape[j]->id);
      global.shape[j]->id=strdup(a[3]);
     }
    }
   }
   continue;
  }
  if(!strcmp(command,"dump")) {
   printf("%s set camera.p.x %Lf\n",global.user,camera.p.x);
   printf("%s set camera.p.y %Lf\n",global.user,camera.p.y);
   printf("%s set camera.p.z %Lf\n",global.user,camera.p.z);
   printf("%s set camera.xr %d\n",global.user,camera.xr);
   printf("%s set camera.yr %d\n",global.user,camera.yr);
   printf("%s set camera.zr %d\n",global.user,camera.zr);
   printf("%s set camera.zoom %Lf\n",global.user,camera.zoom);
   continue;
  }
  if(!strcmp(command,"quit")) {
   return -1;
  }
  if(!strcmp(command,"set")) { //set variable //TODO: add more things to this.
   if(len != 3 && len != 4) continue;
   if(len == 4) {
    if(!strcmp(a[2],"camera.p.x")) camera.p.x=strtold(a[3],0);
    else if(!strcmp(a[2],"camera.p.y")) camera.p.y=strtold(a[3],0);
    else if(!strcmp(a[2],"camera.p.z")) camera.p.z=strtold(a[3],0);
    else if(!strcmp(a[2],"camera.zoom")) camera.zoom=strtold(a[3],0);
    else if(!strcmp(a[2],"camera.xr")) camera.xr=atoi(a[3]);
    else if(!strcmp(a[2],"camera.yr")) camera.yr=atoi(a[3]);
    else if(!strcmp(a[2],"camera.zr")) camera.zr=atoi(a[3]);
    else printf("# unknown variable: %s\n",a[2]);
    continue;
   }
#ifdef GRAPHICAL
   if(!strcmp(a[2],"force_redraw")) gra_global.force_redraw^=1;
   else if(!strcmp(a[2],"red_and_blue")) { gra_global.red_and_blue^=1; set_aspect_ratio(); }
#endif
   else { printf("# unknown variable: %s\n",a[2]); continue; }
   printf("# %s toggled!\n",a[2]);
   continue;
  }
  if(!strcmp(command,"addshape")) {
   if(len > 3) {
    if(len != (strtold(a[2],0)*3)+3) {
     printf("# ERROR: wrong amount of parts for addshape. got: %d expected %d\n",len,((int)strtold(a[2],0))*3+3);
     continue;
    }
    global.shape[i]=malloc(sizeof(struct c3_shape));
    global.shape[i]->len=strtold(a[2],0);
    global.shape[i]->id=strdup(id);
    for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
     global.shape[i]->p[j].x=strtold(a[(j*3)+3],0);//second arg is just for a return value. set to 0 if you don't want it.
     global.shape[i]->p[j].y=strtold(a[(j*3)+4],0);
     global.shape[i]->p[j].z=strtold(a[(j*3)+5],0);
    }
    i++;
    global.shapes=i;
    global.shape[i]=0;
   }
   continue;
  }
  if(!strcmp(command,"export")) {
   if(len > 2) {
    for(i=0;global.shape[i];i++) {//require a[2], if not it'll segfault. derrrr, epoch.
     if(!strcmp(global.shape[i]->id,a[2])) {
      printf("%s addshape %d",a[2],global.shape[i]->len);
      for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
       printf(" %Lf %Lf %Lf",global.shape[i]->p[j].x,global.shape[i]->p[j].y,global.shape[i]->p[j].z);
      }
      printf("\n");
     }
    }
   }
   continue;
  }
//should scaleup even be inside hackvr? seems like something an external program could do... but it wouldn't act on hackvr's state. so nevermind.
  if(!strcmp(command,"scaleup")) {//should this scale separately so it can be a deform too?
   for(i=0;global.shape[i];i++) {
    if(!strcmp(global.shape[i]->id,id)) {
     for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
      global.shape[i]->p[j].x*=strtold(a[2],0);
      global.shape[i]->p[j].y*=strtold(a[2],0);
      global.shape[i]->p[j].z*=strtold(a[2],0);
     }
    }
   }
   continue;
  }
  if(!strcmp(command,"move")) {
   if(len > 4) {
    for(i=0;global.shape[i];i++) {
     if(!strcmp(global.shape[i]->id,id)) {
      for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
       global.shape[i]->p[j].x+=strtold(a[2],0);
       global.shape[i]->p[j].y+=strtold(a[3],0);
       global.shape[i]->p[j].z+=strtold(a[4],0);
      }
     }
    }
   }
   else {
    printf("# ERROR: wrong amount of parts for move. got: %d expected: 11\n",len);
   }
   continue;
  }
  printf("# I don't know what command you're talking about. %s\n",command);
/*   if(!strcmp(command,"rotate")) {
     for(i=0;global.shape[i];i++) {
      global.shape[i]->p1=rotate_c3_about()
      global.shape[i]->p2=
      global.shape[i]->p3=
     }
*/
  free(line);
  if(a) free(a);
 }
 return ret;
}

int export_file(FILE *fp) {//not used yet. maybe export in obj optionally? no. that should be an external program
// struct c3_shape *to;
// int i;
// for(i=0;global.shape[i];i++) {
//  to=global.shape[i];
//  printf("%s addshape %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf\n",to->id,to->p1.x,to->p1.y,to->p1.z,to->p2.x,to->p2.y,to->p2.z,to->p3.x,to->p3.y,to->p3.z);
// }
 return 0;
}

int main(int argc,char *argv[]) {
  if(argc < 2) {
   fprintf(stderr,"usage: hackvr yourname < from_others > to_others\n");
   return 1;
  } else {
   global.user=strdup(argv[1]);
  }
  fcntl(1,F_SETFL,O_NONBLOCK);//won't work
  setbuf(stdin,0);
  setbuf(stdout,0);
  global.debug=DEBUG;
#ifdef GRAPHICAL
  int redraw;
  graphics_init();
#endif
  printf("# entering main loop\n");
  for(;;) {
#ifdef GRAPHICAL
    redraw=gra_global.force_redraw;
    if((redraw=graphics_event_handler()) == -1) {
     return 0;
    }
    if(redraw && !global.headless) {
     draw_screen();
    }
#endif
    switch(load_stdin()) {
     case -1:
      return 0;
      break;
     case 0:
      break;
     default:
#ifdef GRAPHICAL
      redraw=1;
#endif
      break;
    }
  }
  return 0;
}
