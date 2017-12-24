#define _POSIX_C_SOURCE 200809L //for fileno and strdup
#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/select.h> //code to use select instead of non-blocking is commented out. might decide to use it later.
#include <time.h>
#define __USE_GNU //for longer math constants
#include <math.h>

#include "config.h"
#include "common.h"
#include "math.h"
#ifdef GRAPHICAL
#include "graphics_c3.h"
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

//might be able to make this faster by just using fgets() and not using recursion and malloc.
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
//returns pointer to array of strings. second argument is return by reference length of returned array.
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

int load_stdin() {//this function returns -1 to quit, 0 to not ask for a redraw, and 1 to ask for redraw
 struct c3_shape s;
// struct c3_line l;
 c3_group_rot_t *gr;
 char *command;
 char *line=0;
 char *id;
 char **a=0;
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

// fprintf(stderr,"# entering load_stdin()\n");
//#ifdef _HACKVR_USE_NONBLOCK_
 for(i=0;global.shape[i];i++) ;//hop to the end.
 fcntl(0,F_SETFL,O_NONBLOCK);
 if(feof(stdin))  {
  clearerr(stdin);
 }
//#else
/* readfs=master;
 timeout.tv_sec=0;
 timeout.tv_usec=1;
 if((j=select(1,&readfs,0,0,&timeout)) == -1) {
  perror("select");
  return 0;
 }
 if(FD_ISSET(0,&readfs)) {*/
//#endif
 while((line=line?free(line),read_line_hack(stdin,0):read_line_hack(stdin,0))) {//load as long there's something to load
  if(*line == '#') continue;
//  fprintf(stderr,"# read command: %s\n",line);
 if(a) free(a);//use a static char pointer array so I don't have to use the heap. possible optimization.
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
    fprintf(stderr,"# NOT built headless.\n");
#else
    fprintf(stderr,"# built headless.\n");
#endif
    fprintf(stderr,"# command format:\n");
    fprintf(stderr,"# some_ID_or_nick_or_username command arguments\n");
    fprintf(stderr,"# commands:\n");
    fprintf(stderr,"#   deleteallexcept\n");
    fprintf(stderr,"#   deletegroup\n");
    fprintf(stderr,"#   assimilate\n");
    fprintf(stderr,"#   renamegroup\n");
    fprintf(stderr,"#   dump\n");
    fprintf(stderr,"#   quit\n");
    fprintf(stderr,"#   set\n");
    fprintf(stderr,"#   addshape N x1 y1 z1 ... xN yN zN\n");
    fprintf(stderr,"#   export\n");
    fprintf(stderr,"#   scaleup x y z\n");
    fprintf(stderr,"#   move x y z\n");
    fprintf(stderr,"# that is all.\n");
    continue;
   } else {
    //fprintf(stderr,"# ur not doing it right. '%s'\n",id);
    continue;
   }
  }
  ret=1;
  if(!strcmp(command,"deleteallexcept")) {
   for(j=0;global.shape[j];j++) {//mark first. compress later.
    if(strcmp(global.shape[j]->id,a[2])) {
     free(global.shape[j]->id);
     free(global.shape[j]);
     global.shape[j]=0;
    }
   }
   for(k=0;k<j;k++) {
    if(global.shape[k]) continue;
    for(l=k;global.shape[l] == 0 && l<j;l++);
    global.shape[k]=global.shape[l];
    global.shape[l]=0;
   }
   ret=1;
   //now do the same stuff but for the group_rot structs.
   for(j=0;global.group_rot[j];j++) {

   }
   continue;
  }
  if(!strcmp(command,"deletegroup")) {//should the grouprot get deleted too? sure...
   if(len == 3) {
    for(j=0;global.shape[j] && j < MAXSHAPES;j++) {
     if(!strcmp(global.shape[j]->id,a[2])) {
      free(global.shape[j]->id);
      free(global.shape[j]);
      global.shape[j]=0;
     }
    }
    //we now have an array that needs to be compressed.
    //max length of j.
    for(k=0;k<j;k++) {//now... we go from the beginning
     if(global.shape[k]) continue;
     for(l=k;global.shape[l] == 0 && l<j;l++);
     global.shape[k]=global.shape[l];
     global.shape[l]=0;
    }
    //now for the group_rot struct that goes with it. there should only be one, but might be a few due to bugs elsewhere. heh. let's just get all of them I guess.
    for(j=0;global.group_rot[j] && j < MAXSHAPES;j++) {
     if(!strcmp(global.group_rot[j]->id,a[2])) {
      free(global.group_rot[j]->id);
      free(global.group_rot[j]);
      global.group_rot[j]=0;
     }
    }
    for(k=0;k<j;k++) {
     if(global.group_rot[k]) continue;
     for(l=k;global.group_rot[l] == 0 && l<j;l++);
     global.group_rot[k]=global.group_rot[l];
     global.group_rot[l]=0;
    }
    ret=1;
    continue;
   }
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
   ret=1;
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
    gr=get_group_rotation(a[2]);
    if(gr) {
     free(gr->id);
     gr->id=strdup(a[3]);
    }
   }
   ret=1;
   continue;
  }
  if(!strcmp(command,"status")) {
#ifdef GRAPHICAL
   fprintf(stderr,"# fps: %d\n",gra_global.fps);
   continue;
#endif
  }
  if(!strcmp(command,"dump")) {
   printf("%s set global.camera.p.x %Lf\n",global.user,global.camera.p.x);
   printf("%s set global.camera.p.y %Lf\n",global.user,global.camera.p.y);
   printf("%s set global.camera.p.z %Lf\n",global.user,global.camera.p.z);
   printf("%s set global.camera.r.x %d\n",global.user,global.camera.r.x.d);
   printf("%s set global.camera.r.y %d\n",global.user,global.camera.r.y.d);
   printf("%s set global.camera.r.z %d\n",global.user,global.camera.r.z.d);
   printf("%s set global.zoom %Lf\n",global.user,global.zoom);
   continue;
  }
  if(!strcmp(command,"quit")) {
   return -1;
  }
  if(!strcmp(command,"set")) { //set variable //TODO: add more things to this.
   if(len != 3 && len != 4) continue;
   if(len == 4) {
    if(0);
#ifdef GRAPHICAL
    else if(!strcmp(a[2],"camera.p.x")) global.camera.p.x=strtold(a[3],0);
    else if(!strcmp(a[2],"camera.p.y")) global.camera.p.y=strtold(a[3],0);
    else if(!strcmp(a[2],"camera.p.z")) global.camera.p.z=strtold(a[3],0);
    else if(!strcmp(a[2],"global.zoom")) global.zoom=strtold(a[3],0);
    else if(!strcmp(a[2],"camera.r.x")) global.camera.r.x.d=atoi(a[3]);
    else if(!strcmp(a[2],"camera.r.y")) global.camera.r.y.d=atoi(a[3]);
    else if(!strcmp(a[2],"camera.r.z")) global.camera.r.z.d=atoi(a[3]);
#endif
    else fprintf(stderr,"# unknown variable: %s\n",a[2]);
    ret=1;
    continue;
   }
#ifdef GRAPHICAL
   if(!strcmp(a[2],"global.beep")) global.beep=1;
   else if(!strcmp(a[2],"force_redraw")) gra_global.force_redraw^=1;
   else if(!strcmp(a[2],"red_and_blue")) { gra_global.red_and_blue^=1; set_aspect_ratio(); }
#endif
   else { fprintf(stderr,"# unknown variable: %s\n",a[2]); continue; }
   fprintf(stderr,"# %s toggled!\n",a[2]);
   ret=1;
   continue;
  }
  if(!strcmp(command,"addshape")) {//need to add a grouprot with this.
   if(len > 3) {
    if(len != ((strtold(a[3],0)+(strtold(a[3],0)==1))*3)+4) {
     fprintf(stderr,"# ERROR: wrong amount of parts for addshape. got: %d expected %d\n",len,((int)strtold(a[3],0)+(strtold(a[3],0)==1))*3+4);
     fprintf(stderr,"# usage: addshape color number x y z x y z repeated number of time.\n");
     continue;
    }
    for(i=0;global.shape[i];i++) { if(i>= MAXSHAPES) abort();}//just take me to the end.
    global.shape[i]=malloc(sizeof(struct c3_shape));
    global.shape[i]->len=strtold(a[3],0);
    global.shape[i]->id=strdup(id);
    global.shape[i]->attrib.col=strtold(a[2],0);
    global.shape[i]->attrib.lum=0;
    for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
     global.shape[i]->p[j].x=strtold(a[(j*3)+4],0);//second arg is just for a return value. set to 0 if you don't want it.
     global.shape[i]->p[j].y=strtold(a[(j*3)+5],0);
     global.shape[i]->p[j].z=strtold(a[(j*3)+6],0);
    }
    i++;
    global.shapes=i;
    global.shape[i]=0;

    for(i=0;global.group_rot[i];i++) {
     if(i >= MAXSHAPES) abort();
     if(!strcmp(global.group_rot[i]->id,id)) {
      break;
     }
    }
    if(global.group_rot[i] == 0) {//we have ourselves a new grouprot!
     global.group_rot[i]=malloc(sizeof(c3_group_rot_t));
     global.group_rot[i]->id=strdup(id);
     global.group_rot[i+1]=0;
     global.group_rot[i]->p.x=0;
     global.group_rot[i]->p.y=0;
     global.group_rot[i]->p.z=0;
     global.group_rot[i]->r.x=(degrees){0};
     global.group_rot[i]->r.y=(degrees){0};
     global.group_rot[i]->r.z=(degrees){0};
    }
   }
   ret=1;
   continue;
  }
  if(!strcmp(command,"export")) {//dump shapes and group rotation for argument (or all if arg is *)
   if(len > 2) {
    for(i=0;global.shape[i];i++) {//require a[2], if not it'll segfault. derrrr, epoch.
     if(a[2][0]=='*' || !strcmp(global.shape[i]->id,a[2])) {
      printf("%s_%s addshape %d",id,a[2],global.shape[i]->len);
      for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
       printf(" %Lf %Lf %Lf",global.shape[i]->p[j].x,global.shape[i]->p[j].y,global.shape[i]->p[j].z);
      }//possible TODO: should I combine the string and output it all at once instead of throughout a loop?
      printf("\n");
     }
    }
    for(i=0;global.group_rot[i];i++) {
     if(a[2][0]=='*' || !strcmp(global.group_rot[i]->id,a[2])) {
      printf("%s_%s rotate %d %d %d\n",id,a[2],global.group_rot[i]->p.x,global.group_rot[i]->p.y,global.group_rot[i]->p.z);
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
      global.shape[i]->p[j].y*=strtold(len>4?a[3]:a[2],0);
      global.shape[i]->p[j].z*=strtold(len>4?a[4]:a[2],0);
     }
    }
   }
   ret=1;
   continue;
  }
  if(!strcmp(command,"rotate")) {
   if(len > 4) {
    for(i=0;global.group_rot[i];i++) {
     if(!strcmp(global.group_rot[i]->id,id)) {
      break;
     }
    }
    if(global.group_rot[i] == 0) {//we have ourselves a new grouprot!
     global.group_rot[i]=malloc(sizeof(c3_group_rot_t));
     global.group_rot[i]->id=strdup(id);
     global.group_rot[i+1]=0;
     global.group_rot[i]->p.x=0;//only set these if new.
     global.group_rot[i]->p.y=0;
     global.group_rot[i]->p.z=0;
    }
    global.group_rot[i]->r.x=(degrees){atoi(a[2])};
    global.group_rot[i]->r.y=(degrees){atoi(a[3])};
    global.group_rot[i]->r.z=(degrees){atoi(a[4])};
   }
   ret=1;
   continue;
  }
  if(!strcmp(command,"move")) {
   if(len > 4) {
    for(i=0;global.group_rot[i];i++) {
     if(!strcmp(global.group_rot[i]->id,id)) {
      break;
     }
    }
    if(global.group_rot[i] == 0) {//we have ourselves a new grouprot!
     global.group_rot[i]=malloc(sizeof(c3_group_rot_t));
     global.group_rot[i]->id=strdup(id);
     global.group_rot[i+1]=0;
     global.group_rot[i]->r.x=(degrees){0};//only set these if new.
     global.group_rot[i]->r.y=(degrees){0};
     global.group_rot[i]->r.z=(degrees){0};
    }
    global.group_rot[i]->p.x+=strtold(a[2],0);
    global.group_rot[i]->p.y+=strtold(a[3],0);
    global.group_rot[i]->p.z+=strtold(a[4],0);
   }
   else if(len > 2) {
    if(!strcmp(a[2],"forward")) {
     //move id forward based on id's rotation.
    } else if(!strcmp(a[2],"backward")) {

    } else if(!strcmp(a[2],"up")) {

    } else if(!strcmp(a[2],"down")) {

    } else if(!strcmp(a[2],"left")) {

    } else if(!strcmp(a[2],"right")) {

    } else {
     fprintf(stderr,"# dunno what direction you're talking about. try up, down, left, right, forward, or backward\n");
    }
   } else {
    fprintf(stderr,"# ERROR: wrong amount of parts for move. got: %d expected: 4 or 2\n",len);
   }
   ret=1;
   continue;
  }
  fprintf(stderr,"# I don't know what command you're talking about. %s\n",command);
  //I used to have free(line) here, but this place is never gotten to if a command is found so it wasn't getting released.
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
  char redraw;
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
  global.beep=0;
#ifdef GRAPHICAL
  graphics_init();
#endif
  fprintf(stderr,"# entering main loop\n");
  for(;;) {
    switch(redraw=load_stdin()) {
     case -1:
      return 0;
      break;
     case 0:
      break;
     default:
      break;
    }
#ifdef GRAPHICAL
    graphics_event_handler(redraw);//this thing should call draw_screen when it needs to.
#endif
   sleep(.01);
  }
  return 0;
}
