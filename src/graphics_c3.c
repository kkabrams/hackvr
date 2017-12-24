#define _POSIX_C_SOURCE 200809L //for fileno and strdup
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "common.h"

//#include "graphics_cs.h" //this header includes all the functions you'll need to implement if you want to port hackvr to something else
#include "graphics_c3.h"//we're defining these functions in this file.
#include "graphics_c2.h"//we're using these functions make them.
#include "graphics_cs.h"//we'll need generic function that don't give a damn about which dimension it is?

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

/*
real distance2(c2_t p1,c2_t p2) {
 return sqrt(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) ));
}
*/
real distance3(c3_t p1,c3_t p2) {
 return sqrt(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) )+( (p1.z-p2.z)*(p1.z-p2.z) ));
}

int between_angles(degrees d,real lower,real upper) {
  //lower may be higher than upper.
  //because lower is < 0 which wraps to higher. lower is 270, upper is 90. 270-90 is in front.
  if(lower > upper) {
    if(d.d > lower) return 1;
    if(d.d < upper) return 1;
  }
  if(upper > lower) {
    if(d.d > lower && d.d < upper) return 1;
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

c3_t c3_add(c3_t p1,c3_t p2) {
 return (c3_t){p1.x+p2.x,p1.y+p2.y,p1.z+p2.z};
}

c3_t c3_subtract(c3_t p1,c3_t p2) {
 return (c3_t){p1.x-p2.x,p1.y-p2.y,p1.z-p2.z};
}

//how is this supposed to work? x is distance?
#define MAGIC(x) (1.0l-(1.0l/pow(1.01l,(x)))) //??? might want to have some changables in here

real magic(real x) {
  return MAGIC(x);
}

c2_t c3_to_c2(c3_t p3) { //DO NOT DRAW STUFF IN HERE
  c2_t p2;
//  c3_t tmp1;
//  c3_t tmp2;
//  c3_t tmp3;
  c3_t final;
//these rotations need to be about the previous axis after the axis itself has been rotated.

  final=rotate_c3_yr(p3,global.camera.p,d2r(global.camera.r.y));//rotate everything around the camera's location.
  //now to rotate the shape around it's group's center.
//  final=rotate_c3_yr(p3,(c3_t){0,0,0},d2r(camera.yr));//rotate everything around the center no matter what.
//  tmp2=rotate_c3_xr(tmp1,camera.p,d2r(camera.xr));
//  final=rotate_c3_zr(tmp2,camera.p,d2r(camera.zr));
  real delta_x=(global.camera.p.x - final.x);//I guess X needs this flippage too.
  real delta_y=(global.camera.p.y - final.y);//I dunno. Y is weird.
  real delta_z=(final.z - global.camera.p.z);
//  real d=distance3(camera.p,final);
  p2.x=global.zoom * (delta_x * MAGIC(delta_z) - delta_x);
  p2.y=global.zoom * (delta_y * MAGIC(delta_z) - delta_y);
//  p2.x=global.zoom * (delta_x * MAGIC(delta_z));
//  p2.y=global.zoom * (delta_y * MAGIC(delta_z));//dunno if this is better or not.
  return p2;
}

void draw_minimap_line(c3_t p1,c3_t p2) {
 if(gra_global.drawminimap == 1) {//map moves but doesn't rotate.
  draw_c2_line((c2_t){(p1.z-global.camera.p.z)*global.mmz,(global.camera.p.x-p1.x)*global.mmz},
               (c2_t){(p2.z-global.camera.p.z)*global.mmz,(global.camera.p.x-p2.x)*global.mmz});
 }
 if(gra_global.drawminimap == 2) {//map rotates. //NOT WORKING YET
  c3_t t1=rotate_c3_yr(p1,global.camera.p,d2r(global.camera.r.y));
  c3_t t2=rotate_c3_yr(p2,global.camera.p,d2r(global.camera.r.y));
  draw_c2_line((c2_t){t1.x*global.mmz,t1.z*global.mmz},(c2_t){t2.x*global.mmz,t2.z*global.mmz});
 }
}

void draw_c3_shape(c3_s_t s) {//outlined. needs to be filled? //draw minimap shit in here too? probably...
  int i;
  char drawthefucker=0;//I don't wanna!
  c3_s_t s2;//post rotation
  c2_s_t s3;//post projection
  radians r;
  s2.id=s.id;//it shouldn't disappear and we shouldn't need to make a copy.
  s2.len=s.len;
  s3.id=s.id;
  s3.len=s.len;
  c3_group_rot_t *gr=get_group_rotation(s.id);
  if(s.len > 1) {
   for(i=0;i<s.len+(s.len==1);i++) {//apply the group's rotation and store in s2.
     s2.p[i]=(gr?c3_add(gr->p,rotate_c3_yr(s.p[i],gr->p,d2r(gr->r.y))):s.p[i]);
     //s2.p[i]=c3_to_c2(s.p[i]);
   }
  }
  for(i=0;i<s.len+(s.len==1);i++) {//
   r=points_to_angle((c2_t){global.camera.p.x,global.camera.p.z},(c2_t){s2.p[i].x,s2.p[i].z});
   if(between_angles(r2d(r),(360-global.camera.r.y.d-45+360+90)%360,(360-global.camera.r.y.d+45+360+90)%360)) {
    drawthefucker=1;//damn it. somewhere in this shape needs to be drawn.
   }   
  }
  if(!drawthefucker) return;//fuck yeah. didn't have to do all that other crap.
  if(s.len == 1) {
   real dist=distance3(s.p[0],s.p[1]);
   s2.p[0]=s.p[0];
   s2.p[1]=c3_add(s.p[0],(c3_t){dist,0,0});//whatever
  }
  for(i=0;i<s.len+(s.len==1);i++) {
   if(i>1) draw_minimap_line(s2.p[i],s2.p[(i+1)%s2.len]);
   s3.p[i]=c3_to_c2(s2.p[i]);//we need to convert all points in the shape if we have to draw any parts of it.
  }
  if(gra_global.draw3d == 1) {
    set_ansi_color(s.attrib.col%16);
    draw_c2_shape(s3);
  }
  if(gra_global.draw3d == 2) {
    //set foreground to a gray based on distance
    //between 0 to 100
//    color_based_on_distance();//I don't have the distance in here. :/
//foreground_set();
//how... I want to draw the outline as one color and the fill as another.
//  foreground is set to color based on distance BEFORE we get here.
//  we need that function to tell us whether we're going to draw brighter or darker than middle.
//  then somehow get that information to here so we can pick the draw_mode...
//  darker is draw_mode_and(); brighter is draw_mode_or(); lol. set some global? XD hackhackhack
//the color of this shape is set before it gets drawn.
//which is a grey.
    if(s.attrib.col < 16) {
     set_luminosity_color(s.attrib.lum);
     draw_c2_filled_shape(s3);
     if(s.attrib.lum > 100) {
      draw_mode_or();
     } else {
      draw_mode_and();
     }
    }
    set_ansi_color(s.attrib.col%16);
    draw_c2_filled_shape(s3);
    draw_mode_copy();
    draw_c2_shape(s3);
  }
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

cs_t c3_to_cs(c3_t p) {
 return c2_to_cs(c3_to_c2(p));
}

real shitdist2(c3_t p1,c3_t p2) {
 return sqrt(((p1.x - p2.x) * (p1.x - p2.x)) +
        ((p1.y - p2.y) * (p1.y - p2.y)) +
        ((p1.z - p2.z) * (p1.z - p2.z)));
}

real shitdist(struct c3_shape *s,c3_t p) {//this function is a killer. :/
 int i;
 real total=0;
 for(i=0;i< s->len+(s->len==1);i++) {
  c3_group_rot_t *gr=get_group_rotation(s->id);
  total=total+shitdist2(
                rotate_c3_yr(//we're rotating the point around the camera...
                  gr?
                    c3_add(gr->p,rotate_c3_yr(s->p[i],gr->p,d2r(gr->r.y)))://after applying the rotation of the group it is in.
                    s->p[i],global.camera.p
                  ,d2r(global.camera.r.y))
              ,global.camera.p);
 }//why the fuck are we rotating this around the camera? that shouldn't make any difference to the distance from the camera...
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

void draw_c3_text(c3_t p,char *text) {
 c2_t p2=c3_to_c2(p);
 draw_c2_text(p2,text);
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
 snprintf(tmp,sizeof(tmp)-1,"(%f,%f,%f)",p.x,p.y,p.z);
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
  real oldx=global.camera.p.x;
  real oldz=global.camera.p.z;

  for(i=0;global.shape[i];i++) zs[i].s=global.shape[i];
  for(i=0;global.shape[i];i++) zs[i].d=shitdist(zs[i].s,global.camera.p);
  qsort(&zs,i,sizeof(zs[0]),(int (*)(const void *,const void *))compar);//sort these zs structs based on d. farthest first.
  if(i > 0 && zs[i-1].s) strcpy(global.selected_object,zs[i-1].s->id);

  if(gra_global.split_screen > 1) {
//oh... this will need to be a couple more lines... of what? I forgot. -Sep 2017
   radians tmprad=d2r((degrees){global.camera.r.y.d+90});
   radians tmprad2=d2r((degrees){global.camera.r.y.d+90});
   global.camera.p.z-=(gra_global.split_flip)*((gra_global.split/gra_global.split_screen)*cos( tmprad.r ));
   global.camera.p.x-=(gra_global.split_flip)*((gra_global.split/gra_global.split_screen)*sin( tmprad2.r ));
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
    if(gra_global.drawsky) { //draw_sky() should be a function in an engine specific file.
     //draw_sky();//???p?
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
    if(global.debug) {//the way I have text done won't scale...
//      draw_c2_text((cs_t){0,0},global.user);
//      fprintf(stderr,"\x1b[H");
//      fprintf(stderr,"\x1b[2J");
      snprintf(tmp,sizeof(tmp)-1,"debug: %s minimap: %d 3d: %d fps: %d shapes: %d",global.debug?"on":"off",gra_global.drawminimap,gra_global.draw3d,gra_global.oldfps,global.shapes);
      fprintf(stderr,"%s\n",tmp);
//      draw_c2_text((cs_t){gra_global.xoff,(gra_global.height/2)+10},tmp);
//      snprintf(tmp,sizeof(tmp)-1,"x: %d y: %d",gra_global.mousex,gra_global.mousey);
      fprintf(stderr,"%s\n",tmp);
//      draw_c2_text((cs_t){gra_global.xoff,(gra_global.height/2)+20},tmp);
      snprintf(tmp,sizeof(tmp)-1,"cx: %f cy: %f cz: %f",global.camera.p.x,global.camera.p.y,global.camera.p.z);
      fprintf(stderr,"%s\n",tmp);
//      draw_c2_text((cs_t){gra_global.xoff,(gra_global.height/2)+30},tmp);
      snprintf(tmp,sizeof(tmp)-1,"xr: %d yr: %d zr: %d",global.camera.r.x.d,global.camera.r.y.d,global.camera.r.z.d);
      fprintf(stderr,"%s\n",tmp);
//      draw_c2_text((cs_t){gra_global.xoff,(gra_global.height/2)+40},tmp);
    }

//  if(global.drawminimap) {//this isn't even useful I guess.
   //cx1=(sin(d2r(camera.yr+270))*7l);
   //cy1=(cos(d2r(camera.yr+270))*7l);
   //cx2=(sin(d2r(camera.yr+90))*7l);
   //cy2=(cos(d2r(camera.yr+90))*7l);
//   draw_c2_line((c2_t){0,0},(c2_t){10,10});
//   draw_c2_line((c2_t){0,0},(c2_t){-10,10});
//   draw_c2_line((c2_t){10,10},(c2_t){-10,10});
//  }


///// shiiiit. I should be applying group rotations to all these shapes before sorting them.
//when I do that. I need to make sure to take the group rotation out of draw_c3_shape()'s code.
    for(i=0;global.shape[i];i++) {
     zs[i].d=shitdist(zs[i].s,global.camera.p);
    }
    qsort(&zs,i,sizeof(zs[0]),(int (*)(const void *,const void *))compar);//sort these zs structs based on d.
   //draw all triangles
    if(global.debug) {
     //snprintf(tmp,sizeof(tmp)-1,"selected object: %s",global.selected_object);
     //draw_c2_text((c2_t){gra_global.xoff,(gra_global.height/2)+50},tmp);
    }
   //i already equals the length of the array.
   i-=gra_global.maxshapes;
   if(i<0) i=0;

   for(;global.shape[i];i++) {
    //now we pick the color of this triangle!
    if(gra_global.red_and_blue) {
     if(cn%2==0) {
      set_color_red();
     } else {
      set_color_blue();
     }
//tests of blending grayscale with red and blue
//     draw_c3_triangle(*(zs[i].t));
    } else {
//     if(!strcmp(global.selected_object,zs[i].s->id)) {
      //XSetForeground(global.dpy,global.backgc,global.green.pixel);
//     } else {
//     }
     zs[i].s->attrib.lum=200-((int)(zs[i].d * 4.0l))%200; 
    }
    //if(between_angles(points_to_angle((c2_t){zs[i].s->p[0].x,zs[i].s->p[0].z},(c2_t){camera.p.x,camera.p.z}),d2r(camera.yr+45),d2r(camera.yr+135))) {
//     set_color_based_on_distance(zs[i].d);
     draw_c3_shape(*(zs[i].s));
    //}
   }

 if(gra_global.drawminimap == 1) {
  set_color();
  c2_t t1=rotate_c2((c2_t){3*global.mmz,0},(c2_t){0,0},d2r(global.camera.r.y));
  c2_t t2=rotate_c2((c2_t){-2*global.mmz,2*global.mmz},(c2_t){0,0},d2r(global.camera.r.y));
  c2_t t3=rotate_c2((c2_t){-1*global.mmz,0},(c2_t){0,0},d2r(global.camera.r.y));
  c2_t t4=rotate_c2((c2_t){-2*global.mmz,-2*global.mmz},(c2_t){0,0},d2r(global.camera.r.y));
  draw_c2_line(t1,t2);
  draw_c2_line(t2,t3);
  draw_c2_line(t3,t4);
  draw_c2_line(t4,t1);
 }

//   XSetForeground(global.dpy, global.backgc, global.green.pixel);
   radians tmprad=d2r((degrees){global.camera.r.y.d+90});
   radians tmprad2=d2r((degrees){global.camera.r.y.d+90});
   global.camera.p.z+=(gra_global.split_flip)*(gra_global.split*cos( tmprad.r ));
   global.camera.p.x+=(gra_global.split_flip)*(gra_global.split*sin( tmprad2.r ));
  }
//just draw a line from center to 40 away from the center at the angle of the camera's y-rotation
//this should be minimap shit  draw_c2_line((c2_t){0,0},rotate_c2((c2_t){40,0},(c2_t){0,0},d2r(global.camera.r.y)));
//draw a line from the center to 80 away from the center in the angle of what should point at the mouse.
//  draw_c2_line((c2_t){0,0},rotate_c2((c2_t){80,0},(c2_t){0,0},points_to_angle((c2_t){0,0},cs_to_c2((cs_t){gra_global.mousex,gra_global.mousey}))));
//  draw_c2_line((c2_t){0,0},cs_to_c2((cs_t){gra_global.mousex,gra_global.mousey}));
  global.camera.p.x = oldx;
  global.camera.p.z = oldz; //-= cn*CAMERA_SEPARATION;
  flipscreen();
}

int graphics_init() {
//some of these values set 
 global.zoom=25.0l;//I think if this is set to 1, then 1 3d unit is 1 2d unit?
 global.camera.r.x.d=0;
 global.camera.r.y.d=0;//we should be facing east.
 global.camera.r.z.d=0;
 global.mmz=1;//this is minimap zoom.
 global.camera.p.x=0;
 global.camera.p.y=10;//10 units above the ground should be as low as it goes.
 global.camera.p.z=-6;

 gra_global.input_mode=DEFAULT_INPUT_MODE;
 gra_global.split_screen=SPLIT_SCREEN;
 gra_global.split_flip=-1;
 gra_global.split=CAMERA_SEPARATION;
 gra_global.maxshapes=MAXSHAPES;
 gra_global.red_and_blue=RED_AND_BLUE;
 gra_global.greyscale=1;
 gra_global.zsort=1;
 if(gra_global.red_and_blue) {
  gra_global.width=WIDTH;
 } else {
  gra_global.width=WIDTH*gra_global.split_screen;
 }
 gra_global.height=HEIGHT;
 gra_global.mapxoff=gra_global.width/2;
 gra_global.mapyoff=gra_global.height/2;
 gra_global.drawminimap=DEFAULT_MINIMAP;
 gra_global.draw3d=2;
 gra_global.force_redraw=FORCE_REDRAW;
 graphics_sub_init();
 return 0;//we're fine
}

