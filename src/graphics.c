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

//choose which graphic's end you want here I guess?
#include "graphics_x11.h"
#include "graphics.h"

//typedef float real; //think this conflicts?

//TODO: will have to make some pixmaps get resized when the window does.
//for now set them to be as big as you think you'll ever resize the window to.

#define SKYRULE 90
#define SKYW (WIDTH*5)
#define SKYH (HEIGHT/2)

extern struct global global;
struct gra_global gra_global;

#ifdef GRAPHICAL

//used to figure out what c2 values are associated with each edge of the window.
//#define TOP     160.0
//#define BOTTOM  -160.0
#define TOP     240.0
#define BOTTOM  -240.0
#define RIGHT   320.0
#define LEFT    -320.0

int c2sX(real x) { return (gra_global.width/(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen: 1))) * ((x + RIGHT) / (RIGHT *2)) + gra_global.xoff; }
int s2cX(real x) { return (x/(gra_global.width/(gra_global.split_screen / (gra_global.red_and_blue?gra_global.split_screen :1))))*(RIGHT*2)-RIGHT; }

int c2sY(real y) { return gra_global.height * ((TOP-y) / (TOP*2)); }
int s2cY(real y) { return -((y/gra_global.height) * (TOP*2) - TOP); }

cs_t c2_to_cs(c2_t p) {
 return (cs_t){c2sX(p.x),c2sY(p.y)};
}
c2_t cs_to_c2(cs_t p) {
 return (c2_t){s2cX(p.x),s2cY(p.y)};
}
/*
real distance2(c2_t p1,c2_t p2) {
 return sqrtl(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) ));
}
*/
real distance3(c3_t p1,c3_t p2) {
 return sqrtl(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) )+( (p1.z-p2.z)*(p1.z-p2.z) ));
}

/*
real points_to_angle(c2_t p1,c2_t p2) {
  real a=atan2l(p2.y-p1.y,p2.x-p1.x);
  return a>=0?a:M_PIl+M_PIl+a;
}
*/
/*
real d2r(int d) {
 while(d<0) d+=360;
 return (real)(d%360) / (real)180 * M_PIl;
}*/
int r2d(real r) {
 return r / M_PIl * 180;
}

int between_angles(real angle,real lower,real upper) {
  //lower may be higher than upper.
  //because lower is < 0 which wraps to higher. lower is 270, upper is 90. 270-90 is in front.
  if(lower > upper) {
    if(angle > lower) return 1;
    if(angle < upper) return 1;
  }
  if(upper > lower) {
    if(angle > lower && angle < upper) return 1;
  }
  return 0;
}

/*
B (x2,y2)
 |-.   c
a|  '-.
 |     '-.
 '--------~
C    b     A (x1,y1)
*/

//I'll need this for collision I guess.

/*

//use this for finding a line that intersects with what the camera is pointing at
c2_t get_c2_intersection(c2_t p1,real theta,c2_t p2) {
  real c;
  c=distance2(p1,p2);
  theta_a=C A B
  b = (c/1) * (theta_a + theta_offset)
}

int get_2D_intersection_X(x1,y1,theta_offset,x2,y2) {
 int x3a,y3a,x3b,y3b;
 int a,b,c;//lenght of sides.
 int theta_a,theta_b,theta_c;//angles opposite of same named sides.

 //get angle C A B
 theta_a=
// x1,y1,x2,y2

//what are these d1 and d2?
 if(d1==d2) return global.math_error=1;
 c=dist(x1,y1,x2,y2);
 b = (c/1) * (theta_a + theta_offset);
 a = (c/1) * (theta_b + theta_offset);

 x3b=sin(theta_a) * b;
 y3b=cos(theta_a) * b;
 x3a=sin(theta_a) * a;
 y3a=cos(theta_a) * a;
 printf("x: %d %d\nx: %d %d\n",x3a,y3a,x3b,y3b);
}

int get_2D_intersection_Y(x,y,d,x,z,d) {
 int x3a,y3a,x3b,y3b;
 int a,b,c;//lenght of sides.
 int theta_a,theta_b,theta_c;//angles opposite of same named sides.

 //get angle from A to B.
 x1,y1,x2,y2

 if(d1==d2) return global.math_error=1;
 c=dist(x1,y1,x2,y2);
 b = (c/1) * theta_a;
 a = (c/1) * theta_b;

 x3b=sin(a) * b;
 y3b=cos(a) * b;
 x3a=sin(a) * a;
 y3a=cos(a) * a;
 printf("y: %d %d\ny: %d %d\n",x3a,y3a,x3b,y3b);
}
*/

void draw_c2_line(c2_t p1,c2_t p2) {
  draw_cs_line(c2_to_cs(p1),c2_to_cs(p2));
}

void draw_c3_shape(c3_s_t s) {//outlined. needs to be filled? //draw minimap shit in here too? probably...
  int i;
  cs_s_t ss;
  ss.id=s.id;//it shouldn't disappear and we shouldn't need to make a copy.
  ss.len=s.len;
  for(i=0;i<s.len+(s.len==1);i++) {
    ss.p[i]=c3_to_cs(s.p[i]);
  }
  if(gra_global.draw3d == 1) {
    draw_cs_shape(ss);
  }
  if(gra_global.draw3d == 2) {
    //set foreground to a gray based on distance
    //between 0 to 100
//    color_based_on_distance();//I don't have the distance in here. :/
//foreground_set();
//how... I want to draw the outline as one color and the fill as another.
    draw_cs_filled_shape(ss);
    set_color();//resets it to the default color.
    if(!strcmp(global.selected_object,ss.id)) {
      draw_cs_shape(ss);
    }
  }
}

#define MAGIC(x) (1.0l-(1.0l/powl(1.01l,(x)))) //??? might want to have some changables in here

real magic(real x) {
  return MAGIC(x);
}

void draw_graph(real (*fun)(real x)) {
 c2_t pa;
 draw_c2_line((c2_t){LEFT,0},(c2_t){RIGHT,0});
 draw_c2_line((c2_t){0,TOP},(c2_t){0,BOTTOM});
 for(pa.x=LEFT;pa.x<RIGHT;pa.x+=5.0) {
  draw_c2_line((c2_t){pa.x,2.0},(c2_t){pa.x,-2.0});
 }
 for(pa.y=BOTTOM;pa.y<TOP;pa.y+=5.0) {
  draw_c2_line((c2_t){-2.0,pa.y},(c2_t){2.0,pa.y});
 }
 for(pa.x=LEFT;pa.x<RIGHT;pa.x+=1.0) {
  draw_c2_line((c2_t){pa.x,fun(pa.x)},(c2_t){pa.x+1.0,fun(pa.x+1.0)});
 }
}

/*
c2_t rotate_c2(c2_t p1,c2_t p2,real dr) {//dr is in radians
  c2_t p3;
  real d=distance2(p1,p2);
  real r=points_to_angle(p1,p2);
  r=r+dr;
  p3.x=(sinl(r) * d) + p2.x;
  p3.y=(cosl(r) * d) + p2.y;
  return p3;
}

c3_t rotate_c3_xr(c3_t p1,c3_t p2,real xr) {//rotate y and z around camera based on xr (looking up and down)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.y,p1.z},(c2_t){p2.y,p2.z},xr);
  return (c3_t){p1.x,tmp.x,tmp.y};
}
c3_t rotate_c3_yr(c3_t p1,c3_t p2,real yr) {//rotate x and z around camera based on yr (looking left and right)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.x,p1.z},(c2_t){p2.x,p2.z},yr);
  return (c3_t){tmp.x,p1.y,tmp.y};
}
c3_t rotate_c3_zr(c3_t p1,c3_t p2,real zr) {//rotate x and y around camera based on zr (cocking your head to a side)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.x,p1.y},(c2_t){p2.x,p2.y},zr);
  return (c3_t){tmp.x,tmp.y,p1.z};
}
*/
c2_t c3_to_c2(c3_t p3) { //DO NOT DRAW STUFF IN HERE
  c2_t p2;
//  c3_t tmp1;
//  c3_t tmp2;
//  c3_t tmp3;
  c3_t final;
//these rotations need to be about the previous axis after the axis itself has been rotated.
  final=rotate_c3_yr(p3,camera.p,d2r(camera.yr.d));//rotate everything around the camera's location.
//  final=rotate_c3_yr(p3,(c3_t){0,0,0},d2r(camera.yr));//rotate everything around the center no matter what.
//  tmp2=rotate_c3_xr(tmp1,camera.p,d2r(camera.xr));
//  final=rotate_c3_zr(tmp2,camera.p,d2r(camera.zr));
  real delta_x=(camera.p.x - final.x);
  real delta_y=(camera.p.y - final.y);
  real delta_z=(camera.p.z - final.z);
//  real d=distance3(camera.p,final);
  p2.y=camera.zoom * (delta_y * MAGIC(delta_z) - delta_y);
  p2.x=camera.zoom * (delta_x * MAGIC(delta_z) - delta_x);
  return p2;
}

void draw_c3_line(c3_t p1,c3_t p2) {
// if(!between_angles(points_to_angle((c2_t){camera.p.x,camera.p.z},(c2_t){p1.x,p1.z}),0,90) ||
//    !between_angles(points_to_angle((c2_t){camera.p.x,camera.p.z},(c2_t){p2.x,p2.z}),0,90)) return;
 if(gra_global.drawminimap == 1) {
  draw_c2_line((c2_t){(camera.p.x-2)*global.mmz,(camera.p.z+2)*global.mmz},(c2_t){(camera.p.x+2)*global.mmz,(camera.p.z-2)*global.mmz});
  draw_c2_line((c2_t){(camera.p.x+2)*global.mmz,(camera.p.z+2)*global.mmz},(c2_t){(camera.p.x-2)*global.mmz,(camera.p.z-2)*global.mmz});
  draw_c2_line((c2_t){p1.x*global.mmz,p1.z*global.mmz},(c2_t){p2.x*global.mmz,p2.z*global.mmz});
 }
 if(gra_global.drawminimap == 2) {//map rotates.
  c3_t t1=rotate_c3_yr(p1,camera.p,d2r(camera.yr.d));
  c3_t t2=rotate_c3_yr(p2,camera.p,d2r(camera.yr.d));
  draw_c2_line((c2_t){t1.x*global.mmz,t1.z*global.mmz},(c2_t){t2.x*global.mmz,t2.z*global.mmz});
 }
 if(gra_global.draw3d != 0) draw_c2_line(c3_to_c2(p1),c3_to_c2(p2));
 if(global.debug) {
  
 }
}

cs_t c3_to_cs(c3_t p) {
 return c2_to_cs(c3_to_c2(p));
}

real shitdist2(c3_t p1,c3_t p2) {
 return sqrtl(((p1.x - p2.x) * (p1.x - p2.x)) +
        ((p1.y - p2.y) * (p1.y - p2.y)) +
        ((p1.z - p2.z) * (p1.z - p2.z)));
}

real shitdist(struct c3_shape *s,c3_t p) {
// apply rotation then find distance?
 int i;
 real total=0;
 for(i=0;i< s->len+(s->len==1);i++) {
//  total=total+shitdist2(rotate_c3_yr(s->p[i],(c3_t){0,0,0},d2r(camera.yr)),camera.p);
  total=total+shitdist2(rotate_c3_yr(s->p[i],camera.p,d2r(camera.yr.d)),camera.p);
 }
 return (total) / (real)(s->len+(s->len==1));
}


void HatchLines(c2_t p1,c2_t p2,c2_t p3,int density) {
 real i=0;
 for(i=1;i<density;i=i+1) {//only need 1/2, not 0/2 and 2/2, those are edges of the triangle.
  draw_c2_line(
   (c2_t){((i*p1.x)+((density-i)*p3.x))/density,
          ((i*p1.y)+((density-i)*p3.y))/density},
   (c2_t){((i*p2.x)+((density-i)*p3.x))/density,
          ((i*p2.y)+((density-i)*p3.y))/density}
  );
 }
}

/*void DrawHatchedTriangle(struct c3_triangle t,int d) {
 c2_t p1,p2,p3;
 p1=c3_to_c2(t.p1);
 p2=c3_to_c2(t.p2);
 p3=c3_to_c2(t.p3);
 HatchLines(p1,p2,p3,d);
 HatchLines(p2,p3,p1,d);
 HatchLines(p1,p3,p2,d);
}*/

//^ subdivision algorithm for display purposes only.
//black out the rest of the triangle first?
//sounds alright to me...

void draw_c2_text(c2_t p,char *text) {
 cs_t p2=c2_to_cs(p);
 draw_cs_text(p2,text);
}

void draw_c3_text(c3_t p,char *text) {
 cs_t p2=c3_to_cs(p);
 draw_cs_text(p2,text);
}

/*
cs_t *c3s_to_css(c3_t *p3,int len) {
 cs_t *ps=malloc(sizeof(cs_t) * len);
 int i;
 for(i=0;i<len;i++) ps[i]=c3_to_cs(p3[i]);
 return ps;
}*/

int compar(zsort_t *a,zsort_t *b) {
 return ((a->d) < (b->d));
}


/*
void draw_c3_point_text(c3_t p,char *text) {
 char tmp[256];
 int direction,ascent,descent;
 cs_t p2;
 p2=c3_to_cs(p);
 snprintf(tmp,sizeof(tmp)-1,"(%Lf,%Lf,%Lf)",p.x,p.y,p.z);
 XTextExtents(font,text,strlen(text),&direction,&ascent,&descent,&overall);
 XDrawString(global.dpy,global.backbuffer,global.backgc,p2.x,p2.y+(descent+ascent),tmp,strlen(tmp));
}*/

//push a string back into stdin so it can be read by the file loader. :P
int selfcommand(char *s) {
 char t;
 if(!strlen(s)) return 0;
 ungetc(s[strlen(s)-1],stdin);
 t=s[strlen(s)-1];
 s[strlen(s)-1]=0;
 selfcommand(s);
 putchar(t);
 return 0;
}
#endif

void draw_screen() {
  int i;
  int cn=0;//camera number.
  char tmp[256];
  zsort_t zs[SHAPES];
  clear_backbuffer();
  real oldx=camera.p.x;
  real oldz=camera.p.z;
  if(gra_global.split_screen > 1) {
   camera.p.z-=(gra_global.split_flip)*((gra_global.split/gra_global.split_screen)*cosl(d2r(camera.yr.d+180)));
   camera.p.x-=(gra_global.split_flip)*((gra_global.split/gra_global.split_screen)*sinl(d2r(camera.yr.d+180)));
  }
  for(cn=0;cn<gra_global.split_screen;cn++) {
    set_color();//restart each draw with the default color.
    if(gra_global.red_and_blue) {
     //set color for left half to red and right half to blue
     //and draw mode for the gc to GXOr
     red_and_blue_magic();
     gra_global.xoff=0;//we're overlapping our drawing so no offset based on camera.
    } else {
     gra_global.xoff=(gra_global.width/gra_global.split_screen)*cn;
    }
    if(!gra_global.red_and_blue) {
     set_clipping_rectangle(gra_global.xoff,0,gra_global.width/gra_global.split_screen,gra_global.height);//
    }
    if(gra_global.drawminimap == 3) { draw_graph(magic); continue; }
    if(gra_global.drawsky) {
     //XCopyArea(global.dpy,skypixmap,global.backbuffer,global.backgc,((camera.yr*5)+SKYW)%SKYW,0,WIDTH,global.height/2,0,0);
    }
    if(gra_global.draw3d) {
      draw_c2_line((c2_t){LEFT,0},(c2_t){RIGHT,0}); //horizon
    }
    if(time(0) == gra_global.oldtime) {
     gra_global.fps++;
    }
    else {
     gra_global.oldtime=time(0);
     gra_global.oldfps=gra_global.fps;
     gra_global.fps=0;
    }
    //XSetForeground(global.dpy, global.backgc, global.green.pixel);
    if(global.debug) {
      snprintf(tmp,sizeof(tmp)-1,"debug: %s minimap: %d 3d: %d fps: %d shapes: %d",global.debug?"on":"off",gra_global.drawminimap,gra_global.draw3d,gra_global.oldfps,global.shapes);
      draw_cs_text((cs_t){gra_global.xoff,(gra_global.height/2)+10},tmp);
      snprintf(tmp,sizeof(tmp)-1,"x: %d y: %d",gra_global.mousex,gra_global.mousey);
      draw_cs_text((cs_t){gra_global.xoff,(gra_global.height/2)+20},tmp);
      snprintf(tmp,sizeof(tmp)-1,"cx: %Lf cy: %Lf cz: %Lf",camera.p.x,camera.p.y,camera.p.z);
      draw_cs_text((cs_t){gra_global.xoff,(gra_global.height/2)+30},tmp);
      snprintf(tmp,sizeof(tmp)-1,"xr: %d yr: %d zr: %d",camera.xr.d,camera.yr.d,camera.zr.d);
      draw_cs_text((cs_t){gra_global.xoff,(gra_global.height/2)+40},tmp);
    }

//  if(global.drawminimap) {//this isn't even useful I guess.
   //cx1=(sinl(d2r(camera.yr+270))*7l);
   //cy1=(cosl(d2r(camera.yr+270))*7l);
   //cx2=(sinl(d2r(camera.yr+90))*7l);
   //cy2=(cosl(d2r(camera.yr+90))*7l);
//   draw_c2_line((c2_t){0,0},(c2_t){10,10});
//   draw_c2_line((c2_t){0,0},(c2_t){-10,10});
//   draw_c2_line((c2_t){10,10},(c2_t){-10,10});
//  }

   //apply rotation?
   // load up the triangles to render... after applying rotation?
   for(i=0;global.shape[i];i++) {
    zs[i].s=global.shape[i];
    //rotate_shape(zs[i].s);
   }
   //
   if(1) {//global.zsort) {
    for(i=0;global.shape[i];i++) {
     zs[i].d=shitdist(zs[i].s,camera.p);
    }
    qsort(&zs,i,sizeof(zs[0]),(__compar_fn_t)compar);//sort these zs structs based on d.
   }
   if(zs[i-1].s) {
    strcpy(global.selected_object,zs[i-1].s->id);//0th is farthest. fixme.
   }
   //draw all triangles
   if(global.debug) {
    snprintf(tmp,sizeof(tmp)-1,"selected object: %s",global.selected_object);
    draw_cs_text((cs_t){gra_global.xoff,(gra_global.height/2)+50},tmp);
   }
   for(i=0;global.shape[i];i++) {
    //now we pick the color of this triangle!
    if(gra_global.red_and_blue) {
     if(cn==0) {
//      XSetForeground(global.dpy,global.backgc,global.red.pixel);//???
     } else {
//      XSetForeground(global.dpy,global.backgc,global.blue.pixel);//???
     }
//tests of blending grayscale with red and blue
//     draw_c3_triangle(*(zs[i].t));
    } else {
     if(!strcmp(global.selected_object,zs[i].s->id)) {
      //XSetForeground(global.dpy,global.backgc,global.green.pixel);
     } else {
//      set_color_based_on_distance(zs[i].d);
     }
    }
//    set_color();
    //if(between_angles(points_to_angle((c2_t){zs[i].s->p[0].x,zs[i].s->p[0].z},(c2_t){camera.p.x,camera.p.z}),d2r(camera.yr+45),d2r(camera.yr+135))) {
     set_color_based_on_distance(zs[i].d);
     draw_c3_shape(*(zs[i].s));
    //}
   }
//   XSetForeground(global.dpy, global.backgc, global.green.pixel);
   camera.p.z+=(gra_global.split_flip)*(gra_global.split*cosl(d2r(camera.yr.d+180)));
   camera.p.x+=(gra_global.split_flip)*(gra_global.split*sinl(d2r(camera.yr.d+180)));
  }

//TODO: figure out what all this shit is and either update or remove.
//DONT USE WIDTH for shit.
/*
  x1=nextX(WIDTH/2,HEIGHT/2,d2r(camera.yr),40);
  y1=nextY(WIDTH/2,HEIGHT/2,d2r(camera.yr),40);
  x2=nextX(WIDTH/2,HEIGHT/2,d2r(camera.yr+180),80);
  y2=nextY(WIDTH/2,HEIGHT/2,d2r(camera.yr+180),80);
  XDrawLine(global.dpy,w,gc,WIDTH/2,HEIGHT/2,x1,y1);
  XDrawLine(global.dpy,w,gc,WIDTH/2,HEIGHT/2,x2,y2);

  XDrawLine(global.dpy,w,gc,0,HEIGHT/2,WIDTH,HEIGHT/2);

  x1=global.mousex;
  y1=global.mousey;
  real a=points_to_angle((c2_t){0,0},cs_to_c2((cs_t){x1,y1}));

  snprintf(tmp,sizeof(tmp)-1,"%llf",a);
  XTextExtents(font,tmp,strlen(tmp),&direction,&ascent,&descent,&overall);
  XDrawString(global.dpy,w,gc,global.xoff,(descent+0+ascent)*6,tmp,strlen(tmp));
  snprintf(tmp,sizeof(tmp)-1,"%llf",points_to_angle(cs_to_c2((cs_t){global.mousex,global.mousey}),(c2_t){0,0})+(M_PIl/2));
  XTextExtents(font,tmp,strlen(tmp),&direction,&ascent,&descent,&overall);
  XDrawString(global.dpy,w,gc,global.xoff,(descent+0+ascent)*7,tmp,strlen(tmp));
  XDrawLine(global.dpy,w,gc,global.mousex,global.mousey,global.width/2,global.height/2);

  real c=cosl(d2r(camera.yr) - a) * distance((c2_t){x1,y1},(c2_t){WIDTH/2,HEIGHT/2});
  x2=nextX(x1,y1,d2r(camera.yr-90),c);
  y2=nextY(x1,y1,d2r(camera.yr-90),c);
  XDrawLine(global.dpy,w,gc,x1,y1,x2,y2);
*/
  camera.p.x = oldx;
  camera.p.z = oldz; //-= cn*CAMERA_SEPARATION;
  flipscreen();
}