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

extern struct hvr_global global;
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

/* moved to math.c
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
*/

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

c3_t c3_subtract(c3_t p1,c3_t p2) {
 return (c3_t){p1.x-p2.x,p1.y-p2.y,p1.z-p2.z};
}

//how is this supposed to work? x is distance?
#define MAGIC(x) (1.0l-(1.0l/pow(1.01l,(x)))) //??? might want to have some changables in here
//#define MAGIC(x) (-250.0l / x )

#define TOO_CLOSE (.25l)
#define TOO_FAR (15l)

//input is distance from camera point.
real magic(real x) {
  return MAGIC(x);
}

c2_t c3_to_c2(c3_group_rot_t eye,c3_t p3) { //DO NOT DRAW STUFF IN HERE
  c2_t p2;
  c3_t tmp1;
//  c3_t tmp2;
//  c3_t tmp3;
  c3_t final;
//these rotations need to be about the previous axis after the axis itself has been rotated.
//this rotation needs to not be applied to objects attached to the camera?
  tmp1=rotate_c3_yr(p3,eye.p,d2r(eye.r.y));//rotate everything around the camera's location.
  //now to rotate the shape around it's group's center.
  //final=rotate_c3_yr(p3,(c3_t){0,0,0},d2r(camera.yr));//rotate everything around the center no matter what.
  final=rotate_c3_xr(tmp1,eye.p,d2r(eye.r.x));
//  final=rotate_c3_zr(tmp2,camera.p,d2r(camera.zr));
  real delta_x=(eye.p.x - final.x);//I guess X needs this flippage too.
  real delta_y=(eye.p.y - final.y);//I dunno. Y is weird.
  real delta_z=(final.z - eye.p.z);
  //real d=distance3(global.camera.p,final);
  p2.x=global.zoom * (delta_x * MAGIC(delta_z) - delta_x);
  p2.y=global.zoom * (delta_y * MAGIC(delta_z) - delta_y);//this doesn't look right either.

  //  p2.x=global.zoom * (delta_x * MAGIC(d) - delta_x); // this doesn't look quite right
//  p2.y=global.zoom * (delta_y * MAGIC(d) - delta_y);//dunno if this is better or not.
  return p2;
}

void draw_minimap_shape(c3_s_t s) {//this should replace the draw_minimap_line and actually do circles.
  //hrm... should I have functions for converting c3 points into minimap points?
  c2_s_t s2;
  s2.len=s.len;
  s2.id=s.id;
  int i=0;
  //if(gra_global.drawminimap == 1) {
    for(i=0;i<s.len+(s.len==1);i++) {
      //s2.p[i]=(c2_t){(s.p[i].z - global.camera.p.z) * global.mmz,(s.p[i].x - global.camera.p.x) * global.mmz};
      //s2.p[i]=(c2_t){(global.camera.p.z - s.p[i].z) * global.mmz,(global.camera.p.x - s.p[i].x) * global.mmz};
      s2.p[i]=(c2_t){(global.camera.p.z - s.p[i].z) * global.mmz,(s.p[i].x - global.camera.p.x) * global.mmz};
    }
    draw_c2_shape(s2);
 // }
}

/*
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
*/

void draw_c3_shape(c3_group_rot_t eye,c3_s_t s) {//outlined. needs to be filled? //draw minimap shit in here too? probably...
  int i;
  char drawthefucker=0;//I don't wanna!
  c3_s_t s2;//post rotation
  c2_s_t s3;//post projection
  //radians rx;
  radians ry;
  real d;
  s3.id=s.id;
  s3.len=s.len;

  //c3_group_rot_t gr=get_group_relative(s.id);//it doesn't matter if I get it here or not, I'm not looping the next call.
  //this function will get the group relative by itself if the argument is NULL
  s2=apply_group_relative(s,NULL);//math.c

  for(i=0;i<s.len+(s.len==1);i++) {
    d=distance3(s2.p[i],eye.p);
    if(d > TOO_FAR) {
      //return;//
    }
    if(d < TOO_CLOSE) {//how close is too close?
      //return;//ha. not going to draw a shape that we're too close to.
    }
  }
  //all s2 needs to bet set before this loop.
  for(i=0;i<s.len+(s.len==1);i++) {//
   ry=points_to_angle((c2_t){eye.p.x,eye.p.z},(c2_t){s2.p[i].x,s2.p[i].z});
   //rx=points_to_angle((c2_t){eye.p.y,eye.p.z},(c2_t){s2.p[i].y,s2.p[i].z});
   if(between_angles(r2d(ry),(360-eye.r.y.d-(gra_global.fieldofview/2)+360+90)%360,(360-eye.r.y.d+(gra_global.fieldofview/2)+360+90)%360)
      //&& between_angles(r2d(rx),(360-eye.r.x.d-(gra_global.fieldofview/2)+360)%360,(360-eye.r.x.d+(gra_global.fieldofview/2)+360)%360)
     ) {
    drawthefucker=1;//damn it. somewhere in this shape needs to be drawn.
   }
  }
  if(!drawthefucker) return;//fuck yeah. didn't have to do all that other crap.
  for(i=0;i<s.len+(s.len==1);i++) {
//   if(s.len > 1) draw_minimap_line(s2.p[i],s2.p[(i+1)%s2.len]);//we shouldn't draw circles in here. //yeah. we probably should.
   s3.p[i]=c3_to_c2(eye,s2.p[i]);//we need to convert all points in the shape if we have to draw any parts of it.
  }
  draw_minimap_shape(s2);
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
    if(s.attrib.col < 8) {
     set_luminosity_color(s.attrib.lum);
     draw_c2_filled_shape(s3);
     if(s.attrib.lum > 100) {
      draw_mode_or();
     } else {
      draw_mode_and();
     }
    }
    set_ansi_color(s.attrib.col%8);
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

cs_t c3_to_cs(c3_group_rot_t eye,c3_t p) {
 return c2_to_cs(c3_to_c2(eye,p));
}

real shitdist2(c3_t p1,c3_t p2) {
 return sqrt(((p1.x - p2.x) * (p1.x - p2.x)) +
        ((p1.y - p2.y) * (p1.y - p2.y)) +
        ((p1.z - p2.z) * (p1.z - p2.z)));
}

real shitdist(struct c3_shape *s,c3_t p) {//this function is a killer. :/
 int i;
 real curdist=0;
 real maxdist=0;
 c3_group_rot_t *gr=get_group_relative(s->id);
 for(i=0;i< s->len+(s->len==1);i++) {
  if(gr) {
   curdist=shitdist2(p,rotate_c3_yr(c3_add(gr->p,s->p[i]),gr->p,d2r(gr->r.y)));
  } else {
   curdist=shitdist2(p,s->p[i]);//if there's no gr we're basically just this point. no rotation, not offests.
  }
  if(curdist > maxdist) maxdist=curdist;
 }
 return maxdist;
 //averaging it works ok, but I've decided to now use the farthest to say the distance something is.
 //why the fuck did we get group rotations each... nevermind. this code just sucks.
 /*
 real total=0;
 for(i=0;i< s->len+(s->len==1);i++) {
  c3_group_rot_t *gr=get_group_relative(s->id);
  total=total+shitdist2(
                rotate_c3_yr(//we're rotating the point around the camera...
                  gr?
                    c3_add(gr->p,rotate_c3_yr(s->p[i],gr->p,d2r(gr->r.y)))://after applying the rotation of the group it is in.
                    s->p[i],global.camera.p
                  ,d2r(global.camera.r.y))
              ,global.camera.p);
 }//why the fuck are we rotating this around the camera? that shouldn't make any difference to the distance from the camera...
 return (total) / (real)(s->len+(s->len==1));
 */
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

void draw_c3_text(c3_group_rot_t eye,c3_t p,char *text) {
 c2_t p2=c3_to_c2(eye,p);
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

#endif

void draw_screen() {
  int i;
  int en=0;//eye number. different from "camera number" because a camera can have multiple eyes... a camera is like a "shape"??? hrm..
  //char tmp[256];
  zsort_t zs[SHAPES];
  clear_backbuffer();

  if(gra_global.split_screen > 1) {
    recalculate_eyes();//still have to redo this even if we only have one eye.
  }
  if(gra_global.split_screen == 1) {
    global.eye[0]=global.camera;//shortcut.
  }
  //return 0;//after this
  for(en=0;en<gra_global.split_screen;en++) {
    //now, to zsort for all shapes in each eye... is there a quick way to filter some out early?
    for(i=0;global.shape[i];i++) zs[i].s=global.shape[i];
    for(i=0;global.shape[i];i++) zs[i].d=shitdist(zs[i].s,global.eye[en].p);
    qsort(&zs,i,sizeof(zs[0]),(int (*)(const void *,const void *))compar);//sort these zs structs based on d. farthest first.
    if(i > 0 && zs[i-1].s) strcpy(global.selected_object,zs[i-1].s->id);
    //
    set_color();//restart each draw with the default color.
    if(gra_global.red_and_blue) {
     //set color for left half to red and right half to blue
     //and draw mode for the gc to GXOr
     red_and_blue_magic();
     gra_global.xoff=0;//we're overlapping our drawing so no offset based on camera.
    } else {
     gra_global.xoff=(gra_global.width/gra_global.split_screen)*en;
    }
    if(!gra_global.red_and_blue) {
     set_clipping_rectangle(gra_global.xoff,0,gra_global.width/gra_global.split_screen,gra_global.height);//
    }
    if(gra_global.drawminimap == 3) { draw_graph(magic); continue; }
    if(gra_global.drawsky) { //draw_sky() should be a function in an engine specific file.
     //draw_sky();//???p?
     //XCopyArea(global.dpy,skypixmap,global.backbuffer,global.backgc,((camera.yr*5)+SKYW)%SKYW,0,WIDTH,global.height/2,0,0);
    }
    if(gra_global.draw3d) {//wtf? why do I not compensate for camaera rotation along the x and z?
      //draw_c2_line((c2_t){LEFT,0},(c2_t){RIGHT,0}); //horizon
    }
///// shiiiit. I should be applying group rotations to all these shapes before sorting them.
//when I do that. I need to make sure to take the group rotation out of draw_c3_shape()'s code.
    for(i=0;global.shape[i];i++) {
     zs[i].d=shitdist(zs[i].s,global.eye[en].p);
    }
    qsort(&zs,i,sizeof(zs[0]),(int (*)(const void *,const void *))compar);//sort these zs structs based on d.
   //draw all triangles
   //i already equals the length of the array.
   i-=gra_global.maxshapes;
   if(i<0) i=0;

   for(;global.shape[i];i++) {
    //now we pick the color of this triangle!
    if(gra_global.red_and_blue) {//this can be moved to the draw_c3_shape function and color can be set then.
     if(en%2==0) {
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
     draw_c3_shape(global.eye[en],*(zs[i].s));//the eye viewing it, and the shape
    //}
   }
   //we check here to see if the mouse button is still down
   if(gra_global.mousemap[0]==-1) {
     printf("%s action %f %f\n",global.user,gra_global.mouse.x,gra_global.mouse.y);
     gra_global.mousemap[0]=0;
   }
/*
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
 }*/

//   XSetForeground(global.dpy, global.backgc, global.green.pixel);
  set_color();
  draw_c2_line((c2_t){0,0},rotate_c2((c2_t){40,0},(c2_t){0,0},d2r(global.eye[en].r.x)));
  set_color_red();
  draw_c2_line((c2_t){0,0},rotate_c2((c2_t){-40,0},(c2_t){0,0},d2r((degrees){360-global.eye[en].r.y.d})));
  set_color_blue();
  draw_c2_line((c2_t){0,0},rotate_c2((c2_t){40,0},(c2_t){0,0},d2r(global.eye[en].r.z)));
  }
//just draw a line from center to 40 away from the center at the angle of the camera's y-rotation
//this should be minimap shit  

//draw a line from the center to 80 away from the center in the angle of what should point at the mouse.
  //draw_c2_line((c2_t){0,0},gra_global.mouse);
  set_color();
  draw_c2_line(c2_add(gra_global.mouse,(c2_t){0,1}) , c2_add(gra_global.mouse,(c2_t){0,5}) );
  draw_c2_line(c2_add(gra_global.mouse,(c2_t){0,-1}) , c2_add(gra_global.mouse,(c2_t){0,-5}) );
  draw_c2_line(c2_add(gra_global.mouse,(c2_t){1,0}) , c2_add(gra_global.mouse,(c2_t){5,0}) );
  draw_c2_line(c2_add(gra_global.mouse,(c2_t){-1,0}) , c2_add(gra_global.mouse,(c2_t){-5,0}) );
  set_color();
  //if(points_on_same_side_of_line(gra_global.mouse,(c2_t){80,80},(c2_t){0,0},(c2_t){0,80})) {
  //draw_c2_line((c2_t){0,0},rotate_c2((c2_t){80,0},(c2_t){0,0},points_to_angle((c2_t){0,0},gra_global.mouse)));
  //}
  flipscreen();
}

void redraw() {//something is requesting a redraw.
  if(gra_global.force_redraw == 0) {
    gra_global.force_redraw=1;//this is to prevent drawing way too often.
    write(gra_global.redraw[1],"redraw plzkthx!\n",16);
  }
}

void recalculate_eyes() {//should I put the eyes into the global group_rot array so we can move them?
 int i;
 radians rad=d2r((degrees){global.camera.r.y.d+90});
 //radians tmprad2=d2r((degrees){global.camera.r.y.d+90});
 for(i=0;i<gra_global.split_screen;i++) {//derp. eyes need to be recalculated each camera rotation.
   global.eye[i]=global.camera;//re-init so it follows the camera's location.
   //if split == 1, we need a 0 eye.
   //if split == 2, we need a -.5, and a +.5 eye
   //if split == 3, we need a -1, and a 0 and a +1
   //etc... each eye is gra_global.split away from the next. by default. moving individual eyes might be fun later.
   //even split-screen single-computer playing?
   //left offset 
   /*
   i - (split_screen / 2) + (split / 2) : -.5, .5
   i - (split_screen / 2) + (split / 2) : 
	   1.5			.5	: -1, 0, 1
	   numbers add up close enough for me.
   */
   global.eye[i].p.x+=(gra_global.split_flip) * ( i - (gra_global.split_screen/2 + gra_global.split/2) * sin( rad.r ));
   //global.eye[i].p.y+=0;//no relative change from the camera.
   global.eye[i].p.z+=(gra_global.split_flip) * ( i - (gra_global.split_screen/2 + gra_global.split/2) * cos( rad.r ));
 }
}

int graphics_init() {//return the fd needed to read graphics events.
//some of these values set
// global.zoom=1.0l;//I think if this is set to 1, then 1 3d unit is 1 2d unit?
 global.zoom=25.0l;
 global.camera.r.x.d=0;
 global.camera.r.y.d=0;//we should be facing east. but we're facing north? increasing z goes farther away from camera. :/
 global.camera.r.z.d=0;

 recalculate_eyes();

 global.mmz=1;//this is minimap zoom.

 global.shape[0]=0;//we'll allocate as we need more.
 global.camera.id=strdup(global.user);//make a copy so if we change global.user later we can reattach to this camera.
 ht_setkey(&global.ht_group,global.user,&global.camera);//merp. this will probably break when you try to change global.user to anything else.

 global.camera.p.x=0;
 global.camera.p.y=0;
 global.camera.p.z=-6;//back away from the 0 z plane so it is easier to draw 2D GUIs

 gra_global.oldcamera=global.camera.r;
 gra_global.input_mode=DEFAULT_INPUT_MODE;
 gra_global.split_screen=SPLIT_SCREEN;
 gra_global.split_flip=-1;
 gra_global.split=CAMERA_SEPARATION;
 gra_global.maxshapes=MAXSHAPES;
 gra_global.red_and_blue=RED_AND_BLUE;
 gra_global.greyscale=1;
 gra_global.zsort=1;
 gra_global.fieldofview=FIELDOFVIEW;//config.h

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
 return graphics_sub_init();
}
