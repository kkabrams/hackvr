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
  real sum=0;
  for(i=0;i < s->len;i++) {
    sum+=distance2((c2_t){s->p[i].x,s->p[i].z},(c2_t){0,0});
  }
  //sum /= s->len;
  return sum * 5;
}


//might be able to make this faster by just using fgets() and not using recursion and malloc.
/* does not return the newline. */

//this isn't being used anymore afaict.
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
    if(*a == '*') return strcmp("yep","yep");//lol
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
    case -1://quit
      fprintf(stderr,"# exiting due to EOF\n");
      exit(0);
    case 0://don't redraw
      break;
    case 1://redraw please.
    #ifdef GRAPHICAL
      redraw();
    #endif
      break;
    default://no idea.
      break;
  }
}

//this function returns -1 to quit, 0 to not ask for a redraw, and 1 to ask for redraw
int hackvr_handler(char *line) {
  int ret=0;
  int len;
  int j,i,k,l;
  unsigned int key_count;
  c3_group_rel_t *gr;
  real tmpx,tmpy,tmpz;
  char **a;
  char **keys;
  struct entry *m;
  char helping=0;//a flag that we can check for to see if we need to output our help
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
  } else {
    command=a[0];//meh
  }
  if(len < 2) {
    if(!strcmp(id,"version")) {
      hvr_version();
      return 0;
    }
    if(!strcmp(id,"help")) {
      helping=1;
#ifdef GRAPHICAL
      fprintf(stderr,"# NOT built headless.\n");
#else
      fprintf(stderr,"# built headless.\n");
#endif
      fprintf(stderr,"# commands that don't get prepended with groupname: help, version\n");
      fprintf(stderr,"# command format:\n");
      fprintf(stderr,"# group names can be globbed in some cases to operate on multiple groups\n");
      fprintf(stderr,"# some commands that take numbers as arguments can be made to behave relative\n");
      fprintf(stderr,"# by putting + before the number. makes negative relative a bit odd like:\n");
      fprintf(stderr,"#   user move +-2 +-2 0\n");
      fprintf(stderr,"# groupnam* command arguments\n");
      fprintf(stderr,"# commands:\n");
    }
  }
  ret=1;
  
  
/* ---------- */
  if(helping) fprintf(stderr,"#   deleteallexcept grou*\n");
  if(!strcmp(command,"deleteallexcept")) {
    if(len == 3) {
      for(j=0;global.shape[j] && j < MAXSHAPES;j++) {//mark first. compress later.
        if(glob_match(a[2],global.shape[j]->id)) {
          free(global.shape[j]->id);
          free(global.shape[j]);
          global.shape[j]=0;
        }
      }
      for(k=0;k<j;k++) {//compress the shape array around the gaps
        if(global.shape[k]) continue;
        for(l=k;global.shape[l] == 0 && l<j;l++);
        global.shape[k]=global.shape[l];
        global.shape[l]=0;
      }
      //now do the same stuff but for the group_rel structs.
      keys=ht_getkeys(&global.ht_group,&key_count);
      for(i=0;i<key_count;i++) {
        if((m=ht_getentry(&global.ht_group,keys[i]))) {
          if(glob_match(a[2],m->original)) {//we're inverting the glob match
            if(m->target != &global.camera) {
              gr=m->target;
              free(gr->id);
              free(gr);//ht_delete doesn't know that the target is a malloc()d structure, so we have to do this.
              ht_delete(&global.ht_group,m->original);
            }
          }
        } else {
          fprintf(stderr,"# somehow an item is in the list of keys but ht_getentry failed. '%s'\n",a[2]);
          abort();
        }
      }
      free(keys);
      ret=1;
      return ret;
    }
  }
  

/* ---------- */
  if(helping) fprintf(stderr,"# _ deletegroup grou*\n");
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
      if(!strchr(a[2],'*')) {//don't bother looping over everything if it isn't a glob..
        if((m=ht_getentry(&global.ht_group,a[2]))) {
          //should we allow the deletion of the camera if the explicitly ask?
          if(m->target != &global.camera) {
            //maybe later...
            gr=m->target;
            free(gr->id);
            free(gr);
            ht_delete(&global.ht_group,m->original);
          }
        }
      } else {
        keys=ht_getkeys(&global.ht_group,&key_count);
        for(i=0;i<key_count;i++) {
          if((m=ht_getentry(&global.ht_group,keys[i]))) {
            if(m->target != &global.camera) {
              gr=m->target;
              free(gr->id);
              free(gr);
              ht_delete(&global.ht_group,m->original);
            }
          } else {
            fprintf(stderr,"# somehow an item is in the list of keys but ht_getentry failed. '%s'\n",a[2]);
            abort();
          }
        }
        free(keys);
      }
      ret=1;
      return ret;
    }
  }
  

/* ---------- */
  if(helping) fprintf(stderr,"#   assimilate grou*\n");
  if(!strcmp(command,"assimilate")) {//um... what do we do with the group_relative? flatten it?
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
  

/* ---------- */
  if(helping) fprintf(stderr,"#   renamegroup group\n");
  if(!strcmp(command,"renamegroup")) {//this command doesn't need globbing
   if(len == 4) {//syntax: epoch renamegroup originally eventually?
    for(j=0;global.shape[j];j++) {
     if(!strcmp(a[2],global.shape[j]->id)) {
      free(global.shape[j]->id);
      global.shape[j]->id=strdup(a[3]);//wait. what? wtf is the syntax for this?
     }
    }
    gr=get_group_relative(a[2]);//this shouldn't be used here. why?
    if(gr) {
     ht_delete(&global.ht_group,gr->id);
     free(gr->id);
     gr->id=strdup(a[3]);//we also need to remove and reinsert into the hash table.
     ht_setkey(&global.ht_group,gr->id,gr);
    }
   }
   ret=1;
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   status  # old. just outputs a variable that is supposed to be loops per second.\n");
  if(!strcmp(command,"status")) {
   fprintf(stderr,"# loops per second: %d\n",global.lps);
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   dump  # tries to let you output the various things that can be set.\n");
  if(!strcmp(command,"dump")) {//same as debug output... and the periodic data.
   printf("%s set global.camera.p.x %f\n",global.user,global.camera.p.x);
   printf("%s set global.camera.p.y %f\n",global.user,global.camera.p.y);
   printf("%s set global.camera.p.z %f\n",global.user,global.camera.p.z);
   printf("%s set global.camera.r.x %d\n",global.user,global.camera.r.x.d);
   printf("%s set global.camera.r.y %d\n",global.user,global.camera.r.y.d);
   printf("%s set global.camera.r.z %d\n",global.user,global.camera.r.z.d);
   printf("%s set global.zoom %f\n",global.user,global.zoom);
   //printf("%s set title %s\n",global.user
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   quit  #closes hackvr only if the id that is doing it is the same as yours.\n");
  if(!strcmp(command,"quit")) {
   if(!strcmp(id,global.user)) {//only exit hackvr if *we* are quitting
    return -1;
   } else {
    fprintf(stderr,"# %s has quit hackvr\n",id);
   }
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   set\n");
  if(!strcmp(command,"set")) { //set variable //TODO: add more things to this.
   if(len != 3 && len != 4) return ret;
   if(len == 4) {
    if(0);
#ifdef GRAPHICAL
    else if(!strcmp(a[2],"title")) set_title(a[3]);//who cares for spaces anyway?
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
#ifdef GRAPHICAL
     fprintf(stderr,"# variables: camera.{p,r}.{x,y,z}, global.zoom, input_mode");
#else
     fprintf(stderr,"# don't have any variables to set in headles mode.\n");
#endif
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
#ifdef GRAPHICAL
    fprintf(stderr,"# variables: global.beep, force_redraw, red_and_blue\n");
#else
    fprintf(stderr,"# don't have any variables to toggle in headless mode.\n");
#endif
    return ret;
   }
   fprintf(stderr,"# %s toggled!\n",a[2]);
   ret=1;
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   physics\n");
  if(!strcmp(command,"physics")) {
   apply_physics();//lol
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   control grou* [globbing this group could have fun effects]\n");
  if(!strcmp(command,"control")) {
   if(len > 2) {
    free(global.user);//need to ensure this is on the heap
    global.user=strdup(a[2]); // :D
   }
   ret=0;//doesn't change anything yet...
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   subsume child-group\n");
  if(!strcmp(command,"subsume")) {
   gr=get_group_relative(a[2]);//we need the child's group relative...
   gr->parent = gr->id;
   ret=0;
   return ret;
  }

/* ---------- */
  if(helping) fprintf(stderr,"#   addshape color N x1 y1 z1 ... xN yN zN\n");
  if(!strcmp(command,"addshape")) {
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

    //wtf are we actually needing here? just initialization?
    if(!get_group_relative(id)) abort();//malloc error. fuck this shit.
   }
   ret=1;
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   export grou*\n");
  if(!strcmp(command,"export")) {//dump shapes and group rotation for argument (or all if arg is *)
   if(len > 2) {
    for(i=0;global.shape[i];i++) {
     if(!glob_match(a[2],global.shape[i]->id)) {
      printf("%s_%s addshape %d %d",id,global.shape[i]->id,global.shape[i]->attrib.col,global.shape[i]->len);
      for(j=0;j < global.shape[i]->len+(global.shape[i]->len==1);j++) {
       printf(" %f %f %f",global.shape[i]->p[j].x,global.shape[i]->p[j].y,global.shape[i]->p[j].z);
      }//possible TODO: should I combine the string and output it all at once instead of throughout a loop?
      printf("\n");
     }
    }
    //so... this might be a bit dense.
    //the hash table has an array of the filled buckets
    //and we're looping over all of them.
    //THEN inside each bucket is a linked list we also have to descend
    //this all to just find the keys that match a glob.
    for(i=0;i < global.ht_group.kl;i++) {//for each bucket and item in each bucket...
     for(m=global.ht_group.bucket[global.ht_group.keys[i]]->ll;m;m=m->next) {
      if(!glob_match(a[2],m->original)) {
       gr=m->target;//this almost CERTAINLY exists... I think.
       printf("%s_%s rotate %d %d %d\n",id,gr->id,gr->r.x.d,gr->r.y.d,gr->r.z.d);
       printf("%s_%s move %f %f %f\n",id,gr->id,gr->p.x,gr->p.y,gr->p.z);
       printf("%s_%s scale %f %f %f\n",id,gr->id,gr->s.x,gr->s.y,gr->s.z);
      }
     }
    }
   }
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   ping any-string-without-spaces\n");
  if(!strcmp(command,"ping")) {//lol wat?
    if(len > 2) {
      printf("%s pong %s\n",global.user,a[2]);
    } else {
      printf("%s pong\n",global.user);
    }
    return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"# * scale x y z\n");
  if(!strcmp(command,"scale")) {
   if(len == 5) {
    if(strchr(id,'*')) {//we're globbing
     for(i=0;i < global.ht_group.kl;i++) {
      for(m=global.ht_group.bucket[global.ht_group.keys[i]]->ll;m;m=m->next) {
       if(!glob_match(id,m->original)) {
        gr=m->target;
        gr->s=(c3_t){strtold(a[2],0),strtold(a[3],0),strtold(a[4],0)};
       }
      }
     }
    } else {
     gr=get_group_relative(id);
     gr->s=(c3_t){strtold(a[2],0),strtold(a[3],0),strtold(a[4],0)};
    }
   }
   return 1;
  }


/* ---------- */
  if(helping) fprintf(stderr,"# * rotate [+]x [+]y [+]z\n");
  if(!strcmp(command,"rotate")) {
   if(len == 5) {
    if(strchr(id,'*')) {//we're globbing
     for(i=0;i < global.ht_group.kl;i++) {
      for(m=global.ht_group.bucket[global.ht_group.keys[i]]->ll;m;m=m->next) {
       if(!glob_match(id,m->original)) {
        gr=m->target;
        gr->r.x=(degrees){(a[2][0]=='+'?gr->r.x.d:0)+atoi(a[2]+(a[2][0]=='+'))};
        gr->r.y=(degrees){(a[3][0]=='+'?gr->r.y.d:0)+atoi(a[3]+(a[3][0]=='+'))};
        gr->r.z=(degrees){(a[4][0]=='+'?gr->r.z.d:0)+atoi(a[4]+(a[4][0]=='+'))};
        //now to sanitize them into 0 <= degrees < 360
        gr->r.x.d -= (-(gr->r.x.d < 0)+(gr->r.x.d / 360)) * 360;
        gr->r.y.d -= (-(gr->r.y.d < 0)+(gr->r.y.d / 360)) * 360;
        gr->r.z.d -= (-(gr->r.z.d < 0)+(gr->r.z.d / 360)) * 360;
       }
      }
     }
    } else {
     gr=get_group_relative(id);
     gr->r.x=(degrees){(a[2][0]=='+'?gr->r.x.d:0)+atoi(a[2]+(a[2][0]=='+'))};
     gr->r.y=(degrees){(a[3][0]=='+'?gr->r.y.d:0)+atoi(a[3]+(a[3][0]=='+'))};
     gr->r.z=(degrees){(a[4][0]=='+'?gr->r.z.d:0)+atoi(a[4]+(a[4][0]=='+'))};
     //now to sanitize them into 0 <= degrees < 360
     gr->r.x.d -= (-(gr->r.x.d < 0)+(gr->r.x.d / 360)) * 360;
     gr->r.y.d -= (-(gr->r.y.d < 0)+(gr->r.y.d / 360)) * 360;
     gr->r.z.d -= (-(gr->r.z.d < 0)+(gr->r.z.d / 360)) * 360;
    }
   }
   ret=1;
   return ret;
  }


/* ---------- */
  if(helping) fprintf(stderr,"#   periodic  # flushes out locally-cached movement and rotation\n");
  if(!strcmp(command,"periodic")) {
#ifdef GRAPHICAL
//     fprintf(stderr,"# loops per second: %d mouse.x: %f mouse.y: %f\n",global.lps,gra_global.mouse.x,gra_global.mouse.y);
#else
     fprintf(stderr,"# loops per second: %d\n",global.lps);
#endif
     global.periodic_output = PERIODIC_OUTPUT;
     //output any difference between current camera's state
     //and the camera state the last time we output it.
     if(memcmp(&global.old_p,&global.camera.p,sizeof(c3_t))) {//could I use plain ==? I bet I could.
      global.old_p=global.camera.p;
      printf("%s move %f %f %f\n",global.user,global.old_p.x,global.old_p.y,global.old_p.z);
     }
     if(memcmp(&global.old_r,&global.camera.r,sizeof(c3_rot_t))) {
      global.old_r=global.camera.r;//duh
      printf("%s rotate %d %d %d\n",global.user,global.camera.r.x.d,global.camera.r.y.d,global.camera.r.z.d);
     }
  }

/* ---------- */
  if(helping) fprintf(stderr,"#   flatten  # combines group attributes to the shapes.\n");
  if(!strcmp(command,"flatten")) {//usage: gro* flatten\n
   if(len > 1) {
    if(strchr(id,'*')) {//we're globbing
     for(i=0;i < global.ht_group.kl;i++) {
      for(m=global.ht_group.bucket[global.ht_group.keys[i]]->ll;m;m=m->next) {
       if(!glob_match(id,m->original)) {
        gr=m->target;
        for(j=0;global.shape[j];j++) {
         if(!strcmp(global.shape[j]->id,gr->id)) {
          (*global.shape[j])=apply_group_relative((*global.shape[j]),0);
         }
        }
        gr->r=(c3_rot_t){(degrees){0},(degrees){0},(degrees){0}};
        gr->p=(c3_t){0,0,0};
        gr->s=(c3_t){1,1,1};
       }
      }
     }
    } else {
     //this... would like to find a way to combine them.
     gr=get_group_relative(id);
     for(j=0;global.shape[j];j++) {
      if(!strcmp(global.shape[j]->id,gr->id)) {
       (*global.shape[j])=apply_group_relative((*global.shape[j]),0);
      }
     }
     gr->r=(c3_rot_t){(degrees){0},(degrees){0},(degrees){0}};
     gr->p=(c3_t){0,0,0};
     gr->s=(c3_t){1,1,1};
    }
   }
   return ret;
  }


  if(helping) {
   fprintf(stderr,"# * move [+]x [+]y [+]z\n");
   fprintf(stderr,"# * move forward|backward|up|down|left|right\n");
  }
  if(!strcmp(command,"move")) {//this is only moving the first group_rel it finds instead of all group_rels that match the pattern
   if(len > 2) {
    gr=get_group_relative(id);
   }
   //this is to allow globbing of moves finally.
   /*if(len == 5) {
    if(strchr(id,'*')) {//we're globbing
     for(i=0;i < global.ht_group.kl;i++) {
      for(m=global.ht_group.bucket[global.ht_group.keys[i]]->ll;m;m=m->next) {
       if(!glob_match(id,m->original)) {
        gr=m->target;
        gr->s=(c3_t){strtold(a[2],0),strtold(a[3],0),strtold(a[4],0)};
       }
      }
     }
    } else {
     gr=get_group_relative(id);
     gr->s=(c3_t){strtold(a[2],0),strtold(a[3],0),strtold(a[4],0)};
    }
   }*/
   //pasted from shit.
   if(len > 4) { //if we have > 4 we're doing relative movement
    gr->p.x=(a[2][0]=='+'?gr->p.x:0)+strtold(a[2]+(a[2][0]=='+'),0);
    gr->p.y=(a[3][0]=='+'?gr->p.y:0)+strtold(a[3]+(a[3][0]=='+'),0);
    gr->p.z=(a[4][0]=='+'?gr->p.z:0)+strtold(a[4]+(a[4][0]=='+'),0);
   }
   else if(len > 2) {
    tmpy=0;
    if(!strcmp(a[2],"forward")) {
     tmprady=d2r((degrees){global.camera.r.y.d});
    } else if(!strcmp(a[2],"backward")) {
     tmprady=d2r((degrees){global.camera.r.y.d+180});
    } else if(!strcmp(a[2],"up")) {
     tmprady=(radians){0};
    } else if(!strcmp(a[2],"down")) {
     tmprady=(radians){0};
    } else if(!strcmp(a[2],"left")) {
     tmprady=d2r((degrees){global.camera.r.y.d+270});
    } else if(!strcmp(a[2],"right")) {
     tmprady=d2r((degrees){global.camera.r.y.d+90});
    } else {
     fprintf(stderr,"# dunno what direction you're talking about. try up, down, left, right, forward, or backward\n");
     return ret;
    }
    if(!tmpy) {//if we're moving up or down we don't need to calculate this shit.
     tmpx=WALK_SPEED*sin(tmprady.r);//the camera's y rotation.
     tmpz=WALK_SPEED*cos(tmprady.r);//these are only based on
    } else {
     tmpx=0;
     tmpz=0;
    }
    gr->p.x+=tmpx;
    gr->p.y+=tmpy;
    gr->p.z+=tmpz;
   } else {
    fprintf(stderr,"# ERROR: wrong amount of parts for move. got: %d expected: 4 or 2\n",len);
   }
   ret=1;
   return ret;
  }
  if(!helping) {
   fprintf(stderr,"# I don't know what command you're talking about. %s\n",command);
   for(i=0;a[i];i++) {
     fprintf(stderr,"# a[%02d] = %s\n",i,a[i]);
   }
  }
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
void redraw_handler(struct shit *me,char *line) {//how do we strip out extra redraws?
  if(gra_global.force_redraw) {
    draw_screen();
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
  int fd=0;//stdin
  if(argc == 2) {
   if(!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version")) {
    hvr_version();
    return 0;
   }
   if(!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) {
    //wtf should go here?
    printf("try this: echo help | hackvr\n");
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
  if(!strcmp(global.user,"help") || !strcmp(global.user,"version")) {
    fprintf(stderr,"# /!\\ WARNING /!\\/ a USER of help or version maybe cause problems when piped into another hackvr. right now it is '%s'\n",global.user);
  }
  global.localecho=1;

  inittable(&global.ht_group,65536);

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
  add_fd(fd,hackvr_handler_idc);//looks like default mode is to exit on EOF of stdin
  pipe(global.selfpipe);
  add_fd(global.selfpipe[0],hackvr_handler_idc);//looks like default mode is to exit on EOF of stdin

#ifdef GRAPHICAL
  //even if the fds for graphics, mouse, and keyboard are all the same, we need to run the handler on it that many times... right?
  if((fd=graphics_init()) == -1) {
    fprintf(stderr,"# graphics system in use doesn't generate events.\n");
  } else {
    i=add_fd(fd,graphics_event_handler);
    fprintf(stderr,"# graphics fd: %d\n",idc.fds[i].fd);
    idc.fds[i].read_lines_for_us=0;
  }

  i=add_fd(mouse_init(),mouse_event_handler);//this should probably be split to keyboard_init, and mouse_init
  fprintf(stderr,"# mouse fd: %d\n",idc.fds[i].fd);
  idc.fds[i].read_lines_for_us=0;

  i=add_fd(keyboard_init(),keyboard_event_handler);
  fprintf(stderr,"# keyboard fd: %d\n",idc.fds[i].fd);
  idc.fds[i].read_lines_for_us=0;

  pipe(gra_global.redraw);
  add_fd(gra_global.redraw[0],redraw_handler);//write a line to get a redraw?
#endif
  //signal(SIGALRM,alarm_handler);
  //alarm(10);
  fprintf(stderr,"# entering main loop\n");
  select_on_everything();

  return 0;
}
