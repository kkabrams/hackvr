#define _POSIX_C_SOURCE 200809L //for fileno and strdup
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/select.h> //code to use select instead of non-blocking is commented out. might decide to use it later.
#include <time.h>
#include <math.h>
#include <errno.h>
#include <signal.h>

#include "config.h"
#include "common.h"
#include "math.h"
#include "physics.h"
#include "input.h" //just the input handler function definitions.
#ifdef GRAPHICAL
#include "graphics_c3.h"
extern struct gra_global gra_global;
#endif

#include <idc.h>
extern struct idc_global idc;

struct hvr_global global;

//TODO: optimizations
//TODO: store caches of cs_t, c2_t, and c3_t numbers.
//TODO: don't forget to remake gophervr with this.
//TODO: XSegment or line structs to store some shit?
//TODO: line and triangle intersection for finding what object was clicked on
//if I don't do hiliting I only need to calculate that upon click.

//TODO: will have to make some pixmaps get resized when the window does.
//for now set them to be as big as you think you'll ever resize the window to.

int lum_based_on_distance(c3_s_t *s) {
  int i;
  real sum;
  for(i=0;i < s->len;i++) {
    sum+=distance2((c2_t){s->p[i].x,s->p[i].z},(c2_t){0,0});
  }
  //sum /= s->len;
  return sum * 5;
}


//might be able to make this faster by just using fgets() and not using recursion and malloc.
/* does not return the newline. */
char *read_line_hack(FILE *fp,int len) {
 short in;
 char *t;
 errno=0;
 switch(in=fgetc(fp)) {
  case '\n':
   t=malloc(len+1);
   t[len]=0;
   return t;
  case -1:
   if(errno == EAGAIN) return 0;
   if(feof(fp)) {
    fprintf(stderr,"# reached EOF. exiting.\n");
    exit(0);
   }
   fprintf(stderr,"# some other error happened while reading. %d %d\n",EAGAIN,errno);
   perror("hackvr");
   exit(1);
  default:
   if((t=read_line_hack(fp,len+1))) t[len]=in;
   break;
 }
 return t;
}

int selfcommand(char *s) {//send this line to be handled by ourselves and output to stdout
 if(global.localecho) {
   write(global.selfpipe[1],s,strlen(s));
 }
 write(1,s,strlen(s));//stdout
 return 0;
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

//this function is like strcmp
//but the first argument MAY use a simple * glob ONLY at the end.
//this function may be changed to allow globs in different spots later.
int glob_match(char *a,char *b) {
  if(strchr(a,'*')) {
    return strncmp(a,b,strchr(a,'*')-a-1);//hack? sure.
  }
  return strcmp(a,b);
}

void hvr_version() {
  fprintf(stderr,"# hackvr version: %s\n",HVR_VERSION);
}

int hackvr_handler(char *line);

void hackvr_handler_idc(struct shit *me,char *line) {
  switch(hackvr_handler(line)) {
    case -1:
      exit(0);
    case 0:
      break;
    case 1:
    #ifdef GRAPHICAL
      redraw();
    #endif
      break;
    default:
      break;
  }
}

//this function returns -1 to quit, 0 to not ask for a redraw, and 1 to ask for redraw
int hackvr_handler(char *line) {
  int ret=0;
  int len;
  int j,i,k,l;
  c3_group_rot_t *gr;
  real tmpx,tmpy,tmpz;
  char **a;
  char tmp[256];
// radians tmpradx,tmprady,tmpradz;
  radians tmprady;

  char *id;
  //might use these so make command code easier to read.
  char *command;
  //char **args;
  if(!line) return -1;//EOF
  if(*line == '#') return 0;
//  fprintf(stderr,"# read command: %s\n",line);
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
   if(!strcmp(id,"version")) {
    hvr_version();
    return 0;
   }
   if(!strcmp(id,"help")) {
#ifdef GRAPHICAL
    fprintf(stderr,"# NOT built headless.\n");
#else
    fprintf(stderr,"# built headless.\n");
#endif
    fprintf(stderr,"# commands that don't get prepended with groupname: help, version\n");
    fprintf(stderr,"# command format:\n");
    fprintf(stderr,"# group names can be globbed in some cases to operate on multiple groups\n");
    fprintf(stderr,"# groupnam* command arguments\n");
    fprintf(stderr,"# commands:\n");
    fprintf(stderr,"#   deleteallexcept grou*\n");
    fprintf(stderr,"#   deletegroup grou*\n");
    fprintf(stderr,"#   assimilate grou*\n");
    fprintf(stderr,"#   renamegroup group\n");
    fprintf(stderr,"#   control grou* [globbing this group could have fun effects]\n");
    fprintf(stderr,"#   dump\n");
    fprintf(stderr,"#   quit\n");
    fprintf(stderr,"#   set\n");
    fprintf(stderr,"#   addshape color N x1 y1 z1 ... xN yN zN\n");
    fprintf(stderr,"#   export grou*\n");
    fprintf(stderr,"# * scaleup x y z\n");
    fprintf(stderr,"# * move x y z\n");
    fprintf(stderr,"# * move forward|backward|up|down|left|right\n");
    fprintf(stderr,"# that is all.\n");
    return ret;
   } else {
    //fprintf(stderr,"# ur not doing it right. '%s'\n",id);
    return ret;
   }
  }
  ret=1;
  if(!strcmp(command,"deleteallexcept")) {
   if(len == 3) {
    for(j=0;global.shape[j] && j < MAXSHAPES;j++) {//mark first. compress later.
     if(glob_match(a[2],global.shape[j]->id)) {
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
    for(j=1;global.group_rot[j] && j < MAXSHAPES;j++) {//start at 1 so we skip the camera. fuck it. let's delete camera.
     if(glob_match(a[2],global.group_rot[j]->id)) {
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
    return ret;
   }
  }
  if(!strcmp(command,"apply")) {
   /*gr=get_group_relative(a[2]);
   if(gr) {
     for(j=0;global.shape[j] && j < MAXSHAPES;j++) {
       if(!glob_match(a[2],global.shape[j].id)) {
         for(k=0;k<global.shape[j].len+(global.shape[j].len==1);k++) {
           //apply the movement stored in this gr to the shapes themselves.
           //after taking into account the rotation.
           //this code is the same as the code in graphics_c3 but this is object specific.
           global.shape[j].p[k]=c3_add(gr->p,rotate_c3_yr(global.shape[j].p[k]);
         }
         gr->p.x=gr->p.y=gr->p.z=0;

         gr->r.x=0;
         gr->r.y=0;
         gr->r.z=0;
       }
     }
     //we don't need this gr anymore.
     //we could clean it up, but its values are already zero.
   }*/
   fprintf(stderr,"# this group doesn't have a gr.\n");
  }
  if(!strcmp(command,"deletegroup")) {//should the grouprot get deleted too? sure...
   if(len == 3) {
    for(j=0;global.shape[j] && j < MAXSHAPES;j++) {
     if(!glob_match(a[2],global.shape[j]->id)) {
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
    for(j=1;global.group_rot[j] && j < MAXSHAPES;j++) {//start at 1 to skip passed the camera. it isn't malloc()d and will crash if we try to free it.
     if(!glob_match(a[2],global.group_rot[j]->id)) {
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
    return ret;
   }
  }
  if(!strcmp(command,"assimilate")) {//um... what do we do with the group_rotation?
   if(len == 3) {
    for(j=0;global.shape[j];j++) {
     if(!glob_match(a[2],global.shape[j]->id)) {
      free(global.shape[j]->id);
      global.shape[j]->id=strdup(id);
     }
    }
   }
   ret=1;
   return ret;
  }
  if(!strcmp(command,"renamegroup")) {//this command doesn't need globbing
   if(len == 4) {
    for(j=0;global.shape[j];j++) {
     if(!strcmp(a[2],global.shape[j]->id)) {
      free(global.shape[j]->id);
      global.shape[j]->id=strdup(a[3]);
     }
    }
    gr=get_group_relative(a[2]);//this shouldn't be used here.
    if(gr) {
     free(gr->id);
     gr->id=strdup(a[3]);
    }
   }
   ret=1;
   return ret;
  }
  if(!strcmp(command,"status")) {
   fprintf(stderr,"# loops per second: %d\n",global.lps);
   return ret;
  }
  if(!strcmp(command,"dump")) {//same as debug output... and the periodic data.
   printf("%s set global.camera.p.x %f\n",global.user,global.camera.p.x);
   printf("%s set global.camera.p.y %f\n",global.user,global.camera.p.y);
   printf("%s set global.camera.p.z %f\n",global.user,global.camera.p.z);
   printf("%s set global.camera.r.x %d\n",global.user,global.camera.r.x.d);
   printf("%s set global.camera.r.y %d\n",global.user,global.camera.r.y.d);
   printf("%s set global.camera.r.z %d\n",global.user,global.camera.r.z.d);
   printf("%s set global.zoom %f\n",global.user,global.zoom);
   return ret;
  }
  if(!strcmp(command,"quit")) {
   return -1;
  }
  if(!strcmp(command,"set")) { //set variable //TODO: add more things to this.
   if(len != 3 && len != 4) return ret;
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
    else if(!strcmp(a[2],"input_mode")) gra_global.input_mode=atoi(a[3]);
#endif
    else {
     fprintf(stderr,"# unknown variable: %s\n",a[2]);
     fprintf(stderr,"# variables: camera.{p,r}.{x,y,z}, global.zoom, input_mode");
    }
    ret=1;
    return ret;
   }
#ifdef GRAPHICAL
   if(!strcmp(a[2],"global.beep")) global.beep=1;
   else if(!strcmp(a[2],"force_redraw")) gra_global.force_redraw^=1;
   else if(!strcmp(a[2],"red_and_blue")) { gra_global.red_and_blue^=1; set_aspect_ratio(); }
#endif
   else {
    fprintf(stderr,"# unknown variable: %s\n",a[2]);
    fprintf(stderr,"# variables: global.beep, force_redraw, red_and_blue\n");
    return ret;
   }
   fprintf(stderr,"# %s toggled!\n",a[2]);
   ret=1;
   return ret;
  }
  if(!strcmp(command,"control")) {//change what shape key commands  affect.
   if(len > 2) {
    free(global.user);//need to ensure this is on the heap
    global.user=strdup(a[2]); // :D
   }
   ret=0;//doesn't change anything yet...
   return ret;
  }
  if(!strcmp(command,"addshape")) {//need to add a grouprot with this.
   if(len > 3) {
    if(len != ((strtold(a[3],0)+(strtold(a[3],0)==1))*3)+4) {
     fprintf(stderr,"# ERROR: wrong amount of parts for addshape. got: %d expected %d\n",len,((int)strtold(a[3],0)+(strtold(a[3],0)==1))*3+4);
     fprintf(stderr,"# usage: addshape color number x y z x y z repeated number of time.\n");
     return ret;
    }
    for(i=0;global.shape[i];i++) { if(i>= MAXSHAPES) abort();}//just take me to the end.
    global.shape[i]=malloc(sizeof(struct c3_shape));
    global.shape[i]->len=strtold(a[3],0);
    global.shape[i]->id=strdup(id);
    global.shape[i]->attrib.col=strtold(a[2],0);
    for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
     global.shape[i]->p[j].x=strtold(a[(j*3)+4],0);//second arg is just for a return value. set to 0 if you don't want it.
     global.shape[i]->p[j].y=strtold(a[(j*3)+5],0);
     global.shape[i]->p[j].z=strtold(a[(j*3)+6],0);
    }
    global.shape[i]->attrib.lum=0;//lum_based_on_distance(global.shape[i]);//set to distance from center?
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
   return ret;
  }
  if(!strcmp(command,"export")) {//dump shapes and group rotation for argument (or all if arg is *)
   if(len > 2) {
    for(i=0;global.shape[i];i++) {//require a[2], if not it'll segfault. derrrr, epoch.
     if(!glob_match(a[2],global.shape[i]->id)) {
      printf("%s_%s addshape %d %d",id,global.shape[i]->id,global.shape[i]->attrib.col,global.shape[i]->len);
      for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
       printf(" %f %f %f",global.shape[i]->p[j].x,global.shape[i]->p[j].y,global.shape[i]->p[j].z);
      }//possible TODO: should I combine the string and output it all at once instead of throughout a loop?
      printf("\n");
     }
    }
    for(i=0;global.group_rot[i];i++) {
     if(!glob_match(a[2],global.group_rot[i]->id)) {
      printf("%s_%s rotate %d %d %d\n",id,a[2],global.group_rot[i]->r.x.d,global.group_rot[i]->r.y.d,global.group_rot[i]->r.z.d);
      printf("%s_%s move %f %f %f\n",id,a[2],global.group_rot[i]->p.x,global.group_rot[i]->p.y,global.group_rot[i]->p.z);
     }
    }
   }
   return ret;
  }
//should scaleup even be inside hackvr? seems like something an external program could do... but it wouldn't act on hackvr's state. so nevermind.
  if(!strcmp(command,"scaleup")) {//should this scale separately so it can be a deform too?
   for(i=0;global.shape[i];i++) {
    if(!glob_match(id,global.shape[i]->id)) { //we're allowing globbing in this command I guess.
     for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
      global.shape[i]->p[j].x*=strtold(a[2],0);
      global.shape[i]->p[j].y*=strtold(len>4?a[3]:a[2],0);
      global.shape[i]->p[j].z*=strtold(len>4?a[4]:a[2],0);
     }
    }
   }
   return 1;
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
    global.group_rot[i]->r.x=(degrees){(a[2][0]=='+'?global.group_rot[i]->r.x.d:0)+atoi(a[2]+(a[2][0]=='+'))};
    global.group_rot[i]->r.y=(degrees){(a[3][0]=='+'?global.group_rot[i]->r.y.d:0)+atoi(a[3]+(a[3][0]=='+'))};
    global.group_rot[i]->r.z=(degrees){(a[4][0]=='+'?global.group_rot[i]->r.z.d:0)+atoi(a[4]+(a[4][0]=='+'))};
    //now to sanitize them into 0 <= degrees < 360
    global.group_rot[i]->r.x.d -= (-(global.group_rot[i]->r.x.d < 0)+(global.group_rot[i]->r.x.d / 360)) * 360;
    global.group_rot[i]->r.y.d -= (-(global.group_rot[i]->r.y.d < 0)+(global.group_rot[i]->r.y.d / 360)) * 360;
    global.group_rot[i]->r.z.d -= (-(global.group_rot[i]->r.z.d < 0)+(global.group_rot[i]->r.z.d / 360)) * 360;
   }
   ret=1;
   return ret;
  }
  if(!strcmp(command,"physics")) {
   apply_physics();//lol
  }
  if(!strcmp(command,"periodic")) {
#ifdef GRAPHICAL
//     fprintf(stderr,"# loops per second: %d mouse.x: %f mouse.y: %f\n",global.lps,gra_global.mouse.x,gra_global.mouse.y);
#else
     fprintf(stderr,"# loops per second: %d\n",global.lps);
#endif
     global.periodic_output = PERIODIC_OUTPUT;
     //output any difference between current camera's state
     //and the camera state the last time we output it.
     if(memcmp(&global.old_p,&global.camera.p,sizeof(c3_t))) {
      global.old_p.x=global.camera.p.x;
      global.old_p.z=global.camera.p.z;
      global.old_p.y=global.camera.p.y;
      printf("%s move %f %f %f\n",global.user,global.old_p.x,global.old_p.y,global.old_p.z);
     }
     if(memcmp(&global.old_r,&global.camera.r,sizeof(c3_rot_t))) {
      global.old_r.x=global.camera.r.x;
      global.old_r.y=global.camera.r.y;
      global.old_r.z=global.camera.r.z;
      printf("%s rotate %d %d %d\n",global.user,global.camera.r.x.d,global.camera.r.y.d,global.camera.r.z.d);
     }
  }
  if(!strcmp(command,"move")) {
   if(len > 2) {
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
     global.group_rot[i]->p.x=0;//why were these not set before?
     global.group_rot[i]->p.y=0;
     global.group_rot[i]->p.z=0;
    }
   }
   if(len > 4) { //if we have > 4 we're doing relative movement
    global.group_rot[i]->p.x=(a[2][0]=='+'?global.group_rot[i]->p.x:0)+strtold(a[2]+(a[2][0]=='+'),0);
    global.group_rot[i]->p.y=(a[3][0]=='+'?global.group_rot[i]->p.y:0)+strtold(a[3]+(a[3][0]=='+'),0);
    global.group_rot[i]->p.z=(a[4][0]=='+'?global.group_rot[i]->p.z:0)+strtold(a[4]+(a[4][0]=='+'),0);
   }
   else if(len > 2) {
    tmpy=0;
    if(!strcmp(a[2],"forward")) {
     tmprady=d2r((degrees){global.camera.r.y.d});
     //snprintf(tmp,sizeof(tmp)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
    } else if(!strcmp(a[2],"backward")) {
     tmprady=d2r((degrees){global.camera.r.y.d+180});
     //snprintf(tmp,sizeof(tmp)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
    } else if(!strcmp(a[2],"up")) {
     tmprady=d2r((degrees){global.camera.r.y.d});//doesn't matter.
     tmpy=WALK_SPEED*1;
    } else if(!strcmp(a[2],"down")) {
     tmprady=d2r((degrees){global.camera.r.y.d});//doesn't matter. yet.
     tmpy=WALK_SPEED*1;
    } else if(!strcmp(a[2],"left")) {
     tmprady=d2r((degrees){global.camera.r.y.d+90});
     //snprintf(tmp,sizeof(tmp)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
    } else if(!strcmp(a[2],"right")) {
     tmprady=d2r((degrees){global.camera.r.y.d+270});
     //snprintf(tmp,sizeof(tmp)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
    } else {
     fprintf(stderr,"# dunno what direction you're talking about. try up, down, left, right, forward, or backward\n");
     return ret;
    }
    tmpx=WALK_SPEED*sin(tmprady.r);//the camera's y rotation.
    tmpz=WALK_SPEED*cos(tmprady.r);//these are only based on
    snprintf(tmp,sizeof(tmp)-1,"%s move +%f +%f +%f\n",global.user,tmpx,tmpy,tmpz);
    selfcommand(tmp);
   } else {
    fprintf(stderr,"# ERROR: wrong amount of parts for move. got: %d expected: 4 or 2\n",len);
   }
   ret=1;
   return ret;
  }
  fprintf(stderr,"# I don't know what command you're talking about. %s\n",command);
  //I used to have free(line) here, but this place is never gotten to if a command is found so it wasn't getting released.
  return ret;
}

int export_file(FILE *fp) {//not used yet. maybe export in obj optionally? no. that should be an external program
// struct c3_shape *to;
// int i;
// for(i=0;global.shape[i];i++) {
//  to=global.shape[i];
//  printf("%s addshape %f %f %f %f %f %f %f %f %f\n",to->id,to->p1.x,to->p1.y,to->p1.z,to->p2.x,to->p2.y,to->p2.z,to->p3.x,to->p3.y,to->p3.z);
// }
 return 0;
}

#ifdef GRAPHICAL
void redraw_handler(struct shit *me,char *line) {
  if(gra_global.force_redraw) {
    graphics_event_handler(1);
    gra_global.force_redraw=0;
  }
}
#endif

void alarm_handler(int sig) {
  selfcommand("periodic");
  alarm(10);//no...
}

int main(int argc,char *argv[]) {
  int i;
  if(argc == 2) {
   if(!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version")) {
    hvr_version();
    return 0;
   }
   if(!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) {
    printf("usage: hackvr file1 file2 file3 ... fileN < from_others > to_others\n");
    return 0;
   }
  }
  if(argc == 1) {
   //we're just doing stdin
  }
  if(argc > 1) {
   //open every argument and add it to list of files to read..
   //I could just read
  }
  global.user=strdup(getenv("USER"));//this gets free()d later so we need to strdup it.
  global.localecho=1;

  fcntl(1,F_SETFL,O_NONBLOCK);//won't work
  setbuf(stdin,0);
  setbuf(stdout,0);
  global.debug=DEBUG;
  global.periodic_output=PERIODIC_OUTPUT;
  global.beep=0;

  //libidc init
  for(i=0;i<100;i++) {
    idc.fds[i].fd=-1;
  }
  idc.shitlen=0;

#ifdef GRAPHICAL
  graphics_init();

  fprintf(stderr,"# x11 fd: %d",input_init());
  i=add_fd(input_init(),input_event_handler);
  idc.fds[i].read_lines_for_us=0;

  pipe(gra_global.redraw);
  add_fd(gra_global.redraw[0],redraw_handler);//lol. write a byte to other half of pipe to redraw screen.
#endif
  add_fd(0,hackvr_handler_idc);//looks like default mode is to exit on EOF of stdin
  pipe(global.selfpipe);
  add_fd(global.selfpipe[0],hackvr_handler_idc);//looks like default mode is to exit on EOF of stdin
  //signal(SIGALRM,alarm_handler);
  //alarm(10);
  fprintf(stderr,"# entering main loop\n");
  select_on_everything();

  return 0;
}
