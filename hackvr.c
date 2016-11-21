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
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h> //for size hints
#include <time.h>
#define __USE_GNU //for longer math constants
#include <math.h>

int oldtime=0;
int oldfps=0;
int fps=0;

//TODO: don't forget to remake gophervr with this.
//TODO: line and triangle intersection for finding what object was clicked on
//if I don't do hiliting I only need to calculate that upon click.

#define WALK_SPEED 1
#define SPLIT_SCREEN 1
#define CAMERA_SEPARATION 4

#define DEPTH_FACTOR 0.965

#define TRIANGLES 16386

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

typedef double long real; //think this conflicts?
//typedef float real; //think this conflicts?

typedef struct {
 real x;
 real y;
 real z;
} c3_t;

typedef struct {
 real x;
 real y;
} c2_t;

typedef struct { //same as XPoint, just use that? nah...
 short x;
 short y;
} cs_t;

//TODO: will have to make some pixmaps get resized when the window does.
//for now set them to be as big as you think you'll ever resize the window to.
#define WIDTH 1024
#define HEIGHT 768

#define SKYRULE 90
#define SKYW (WIDTH*5)
#define SKYH (HEIGHT/2)

Pixmap skypixmap;
char sky[SKYH][SKYW];

struct object_1 {
 int type;
 unsigned char x;
 unsigned char y;
 unsigned char z;
};

struct object_2 {
 int type;
 unsigned short x;
 unsigned short y;
 unsigned short z;
};

struct object_4 {
 int type;
 unsigned int x;
 unsigned int y;
 unsigned int z;
};

struct camera {
  c3_t p;
  int xr;//rotations
  int yr;
  int zr;
  real zoom;
} camera;

struct c3_line {
 char *id;
 c3_t p1;
 c3_t p2;
};

#define MAX_NGON 64 //why not?

struct c3_polygon {
 char *id;
 c3_t p[MAX_NGON];
};

struct c3_triangle {//use array or linked list?
  char *id;
  c3_t p1;
  c3_t p2;
  c3_t p3;
};

struct mainwin {
  int x;
  int y;
  unsigned int depth;
  int mousex;
  int mousey;
  int rmousex;
  int rmousey;
  int buttonpressed;
  unsigned int width;
  unsigned int height;
  unsigned int border_width;
  XColor colors[256];
  int xoff;
  int math_error;
  int mapxoff;
  int mapyoff;
  int split_screen;
  int split_flip;//1 or -1
  char *user;
  char greyscale;
  char drawminimap;//flag
  char draw3d;//flag
  char debug;//flag
  char drawsky;//flag
  char zsort;
  char selected_object[256];//meh
  real mmz;
  XColor green;
  Colormap color_map;
  Display *dpy;
  Window w;
  Pixmap backbuffer;
  Pixmap cleanbackbuffer;
  GC gc;
  GC backgc;
  struct c3_triangle *triangle[TRIANGLES];
  real tridist[TRIANGLES];
  int triangles;
  int derp;
  int root_window;
  real split;
} global;

//used to figure out what c2 values are associated with each edge of the window.
#define TOP     240.0
#define BOTTOM  -240.0
#define RIGHT   320.0
#define LEFT    -320.0

int c2sX(long double x) { return (global.width/global.split_screen) * ((x + RIGHT) / (RIGHT *2)) + global.xoff; }
int s2cX(long double x) { return (x/(global.width/global.split_screen))*(RIGHT*2)-RIGHT; }

int c2sY(long double y) { return global.height * ((TOP-y) / (TOP*2)); }
int s2cY(long double y) { return -((y/global.height) * (TOP*2) - TOP); }

cs_t c2_to_cs(c2_t p) {
 return (cs_t){c2sX(p.x),c2sY(p.y)};
}
c2_t cs_to_c2(cs_t p) {
 return (c2_t){s2cX(p.x),s2cY(p.y)};
}

long double distance2(c2_t p1,c2_t p2) {
 return sqrtl(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) ));
}

long double distance3(c3_t p1,c3_t p2) {
 return sqrtl(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) )+( (p1.z-p2.z)*(p1.z-p2.z) ));
}

long double points_to_angle(c2_t p1,c2_t p2) {
  long double a=atan2l(p2.y-p1.y,p2.x-p1.x);
  return a>=0?a:M_PIl+M_PIl+a;
}

long double d2r(int d) {
 while(d<0) d+=360;
 return (long double)(d%360) / (long double)180 * M_PIl;
}
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

void draw_cs_line(cs_t p1,cs_t p2) {
  XDrawLine(global.dpy,global.backbuffer,global.backgc,p1.x,p1.y,p2.x,p2.y);
}

void draw_c2_line(c2_t p1,c2_t p2) {
  draw_cs_line(c2_to_cs(p1),c2_to_cs(p2));
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

c2_t rotate_c2(c2_t p1,c2_t p2,real dr) {
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

void rotate_triangle_yr(struct c3_triangle *t) {//changes input value!
 t->p1=rotate_c3_yr(t->p1,camera.p,camera.yr);
 t->p2=rotate_c3_yr(t->p2,camera.p,camera.yr);
 t->p3=rotate_c3_yr(t->p3,camera.p,camera.yr);
}

c2_t c3_to_c2(c3_t p3) { //DO NOT DRAW STUFF IN HERE
  c2_t p2;
//  c3_t tmp1;
//  c3_t tmp2;
//  c3_t tmp3;
  c3_t final;
  final=rotate_c3_yr(p3,camera.p,d2r(camera.yr));//rotate everything around the camera's location.
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
 if(global.drawminimap == 1) {
  draw_c2_line((c2_t){(camera.p.x-2)*global.mmz,(camera.p.z+2)*global.mmz},(c2_t){(camera.p.x+2)*global.mmz,(camera.p.z-2)*global.mmz});
  draw_c2_line((c2_t){(camera.p.x+2)*global.mmz,(camera.p.z+2)*global.mmz},(c2_t){(camera.p.x-2)*global.mmz,(camera.p.z-2)*global.mmz});
  draw_c2_line((c2_t){p1.x*global.mmz,p1.z*global.mmz},(c2_t){p2.x*global.mmz,p2.z*global.mmz});
 }
 if(global.drawminimap == 2) {//map rotates.
  c3_t t1=rotate_c3_yr(p1,camera.p,d2r(camera.yr));
  c3_t t2=rotate_c3_yr(p2,camera.p,d2r(camera.yr));
  draw_c2_line((c2_t){t1.x*global.mmz,t1.z*global.mmz},(c2_t){t2.x*global.mmz,t2.z*global.mmz});
 }
 if(global.draw3d != 0) draw_c2_line(c3_to_c2(p1),c3_to_c2(p2));
 if(global.debug) {
  
 }
}

cs_t c3_to_cs(c3_t p) {
 return c2_to_cs(c3_to_c2(p));
}

void XDrawTriangle(cs_t p1,cs_t p2,cs_t p3) {
 //this is probably slower than drawing a triangle.
 XPoint p[3];
 p[0]=(XPoint){p1.x,p1.y};
 p[1]=(XPoint){p2.x,p2.y};
 p[2]=(XPoint){p3.x,p3.y};
 XDrawLines(global.dpy,global.backbuffer,global.backgc,p,3,CoordModeOrigin);
// XDrawLine(global.dpy,global.backbuffer,global.backgc,p1.x,p1.y,p2.x,p2.y);
// XDrawLine(global.dpy,global.backbuffer,global.backgc,p2.x,p2.y,p3.x,p3.y);
// XDrawLine(global.dpy,global.backbuffer,global.backgc,p3.x,p3.y,p1.x,p1.y);
}

void XDrawFilledTriangle(cs_t p1,cs_t p2,cs_t p3) {
 XPoint p[3];
 p[0]=(XPoint){p1.x,p1.y};//should I just use XPoint instead of cs_t ?
 p[1]=(XPoint){p2.x,p2.y};
 p[2]=(XPoint){p3.x,p3.y};
 XFillPolygon(global.dpy,global.backbuffer,global.backgc,p,3,Convex,CoordModeOrigin);
}

real shitdist2(c3_t p1,c3_t p2) {
 return sqrtl(((p1.x - p2.x) * (p1.x - p2.x)) +
        ((p1.y - p2.y) * (p1.y - p2.y)) +
        ((p1.z - p2.z) * (p1.z - p2.z)));
}

real shitdist(struct c3_triangle t,c3_t p) {
// apply rotation then find distance?
 struct c3_triangle t_;
 t_=t;
 rotate_triangle_yr(&t_);
 return (shitdist2(t_.p1,p) + shitdist2(t_.p2,p) + shitdist2(t_.p3,p)) / 3.0l;
}

//^ subdivision algorithm for display purposes only.
//black out the rest of the triangle first?
//sounds alright to me...

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

void DrawHatchedTriangle(struct c3_triangle t,int d) {
 c2_t p1,p2,p3;
 p1=c3_to_c2(t.p1);
 p2=c3_to_c2(t.p2);
 p3=c3_to_c2(t.p3);
 HatchLines(p1,p2,p3,d);
 HatchLines(p2,p3,p1,d);
 HatchLines(p1,p3,p2,d);
}

void draw_cs_text(cs_t p,char *text) {
 char t[256];
 int direction,ascent,descent;
 XFontStruct *font=XLoadQueryFont(global.dpy,"fixed");
 XCharStruct overall;
 snprintf(t,sizeof(t)-1,"%s",text);
 XTextExtents(font,text,strlen(text),&direction,&ascent,&descent,&overall);
 XDrawString(global.dpy,global.backbuffer,global.backgc,p.x,p.y+(descent+ascent),text,strlen(text));
}

void draw_c2_text(c2_t p,char *text) {
 cs_t p2=c2_to_cs(p);
 draw_cs_text(p2,text);
}

void draw_c3_text(c3_t p,char *text) {
 cs_t p2=c3_to_cs(p);
 draw_cs_text(p2,text);
}

void draw_c3_triangle(struct c3_triangle t) {
 if(global.draw3d == 2) { //draw it filled in
  XDrawFilledTriangle(c3_to_cs(t.p1),c3_to_cs(t.p2),c3_to_cs(t.p3));
 }
 if(global.draw3d == 3) { //hashed
  XSetForeground(global.dpy, global.backgc, global.colors[0].pixel);
  //XDrawFilledTriangle(c3_to_cs(t.p1),c3_to_cs(t.p2),c3_to_cs(t.p3));//clear out this triangle.
  XSetForeground(global.dpy, global.backgc, global.green.pixel);
  DrawHatchedTriangle(t,10 - (shitdist(t,camera.p) / 10));//how to get density?
 }
 draw_c3_line(t.p1,t.p2);
 draw_c3_line(t.p2,t.p3);
 draw_c3_line(t.p3,t.p1);
 draw_c3_text(t.p1,t.id);
}

//is basing this all on triangles best, or should I use polygons?
//void pushTriangle(x1,y1,z1,1) {
//  for(i=0;global.triangle[i];i++);
//  global.triangle[i]=malloc(sizeof(struct triangle));
//  global.triangle[i]->x1=x1;
//}
//void pushSquare() {
//  pushTriangle();
//  pushTriangle();
//}

typedef struct {
 struct c3_triangle *t;
 real d;
} zsort_t;

int compar(zsort_t *a,zsort_t *b) {
 return ((a->d) > (b->d));
}


/*
void draw_c3_point_text(c3_t p,char *text) {
 char coords[256];
 int direction,ascent,descent;
 cs_t p2;
 p2=c3_to_cs(p);
 snprintf(coords,sizeof(coords)-1,"(%Lf,%Lf,%Lf)",p.x,p.y,p.z);
 XTextExtents(font,text,strlen(text),&direction,&ascent,&descent,&overall);
 XDrawString(global.dpy,global.backbuffer,global.backgc,p2.x,p2.y+(descent+ascent),coords,strlen(coords));
}*/

void draw_screen(Display *dpy,Window w,GC gc) {
  int i;
  int colori=100;
  int cn=0;//camera number.
  XFontStruct *font=XLoadQueryFont(global.dpy,"fixed");
  XCharStruct overall;
  int direction,ascent,descent;
  char coords[256];
  zsort_t zs[TRIANGLES];
  XCopyArea(global.dpy,global.cleanbackbuffer,global.backbuffer,gc,0,0,global.width,global.height,0,0);//clear the backbuffer.
//  XCopyPlane(global.dpy,global.cleanbackbuffer,global.backbuffer,gc,0,0,global.width,global.height,0,0,1);//clear the backbuffer.

  XRectangle cliprect;
  cliprect.x=0;
  cliprect.y=0;
  cliprect.width=global.width/global.split_screen;
  cliprect.height=global.height;

  real oldx=camera.p.x;
  real oldz=camera.p.z;
  if(global.split_screen > 1) {
   camera.p.z-=(global.split_flip)*((global.split/global.split_screen)*cosl(d2r(camera.yr+180)));
   camera.p.x-=(global.split_flip)*((global.split/global.split_screen)*sinl(d2r(camera.yr+180)));
  }
  for(cn=0;cn<global.split_screen;cn++) {
    global.xoff=(global.width/global.split_screen)*cn;
    XSetClipRectangles(global.dpy,global.backgc,global.xoff,0,&cliprect,1,Unsorted);
    //if(global.drawminimap == 3) { draw_graph(magic); continue; }
    if(global.drawsky) {
     XCopyArea(global.dpy,skypixmap,global.backbuffer,global.backgc,((camera.yr*5)+SKYW)%SKYW,0,WIDTH,global.height/2,0,0);
    }
    if(global.draw3d) {
      draw_c2_line((c2_t){LEFT,0},(c2_t){RIGHT,0}); //horizon
    }
    if(time(0) == oldtime) {
     fps++;
    }
    else {
     oldtime=time(0);
     oldfps=fps;
     fps=0;
    }
    XSetForeground(global.dpy, global.backgc, global.green.pixel);
    snprintf(coords,sizeof(coords)-1,"debug: %s minimap: %d 3d: %d fps: %d triangles: %d",global.debug?"on":"off",global.drawminimap,global.draw3d,oldfps,global.triangles);
    XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
    XDrawString(global.dpy,global.backbuffer,global.backgc,global.xoff,global.height/2+(descent+0+ascent)*1,coords,strlen(coords));
    if(global.debug) {
      snprintf(coords,sizeof(coords)-1,"x: %d y: %d",global.mousex,global.mousey);
      XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
      XDrawString(global.dpy,global.backbuffer,global.backgc,global.xoff,global.height/2+(descent+0+ascent)*2,coords,strlen(coords));
      snprintf(coords,sizeof(coords)-1,"cx: %Lf cy: %Lf cz: %Lf",camera.p.x,camera.p.y,camera.p.z);
      XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
      XDrawString(global.dpy,global.backbuffer,global.backgc,global.xoff,global.height/2+(descent+0+ascent)*3,coords,strlen(coords));
      snprintf(coords,sizeof(coords)-1,"xr: %d yr: %d zr: %d",camera.xr,camera.yr,camera.zr);
      XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
      XDrawString(global.dpy,global.backbuffer,global.backgc,global.xoff,global.height/2+(descent+0+ascent)*4,coords,strlen(coords));
    }

//  if(global.drawminimap) {//this isn't even useful I guess.
   //cx1=(sinl(d2r(camera.yr+270))*7l);
   //cy1=(cosl(d2r(camera.yr+270))*7l);
   //cx2=(sinl(d2r(camera.yr+90))*7l);
   //cy2=(cosl(d2r(camera.yr+90))*7l);
//   XDrawTriangle(camera.x+global.mapxoff,camera.z+global.mapyoff,
//                 camera.x+global.mapxoff+cx1,camera.z+global.mapyoff+cy1,
//                 camera.x+global.mapxoff+cx2,camera.z+global.mapyoff+cy2);
//   draw_c2_line((c2_t){0,0},(c2_t){10,10});
//   draw_c2_line((c2_t){0,0},(c2_t){-10,10});
//   draw_c2_line((c2_t){10,10},(c2_t){-10,10});
//  }

/* cube
   draw_c3_line((c3_t){-3,0,-3},(c3_t){-3,0,3});
   draw_c3_line((c3_t){-3,0,-3},(c3_t){-3,6,-3});
   draw_c3_line((c3_t){-3,0,-3},(c3_t){3,0,-3});

   draw_c3_line((c3_t){3,6,3},(c3_t){3,6,-3});
   draw_c3_line((c3_t){3,6,3},(c3_t){-3,6,3});
   draw_c3_line((c3_t){3,6,3},(c3_t){3,0,3});

   draw_c3_line((c3_t){-3,0,3},(c3_t){3,0,3});
   draw_c3_line((c3_t){-3,0,3},(c3_t){-3,6,3});

   draw_c3_line((c3_t){-3,6,-3},(c3_t){-3,6,3});
   draw_c3_line((c3_t){-3,6,-3},(c3_t){3,6,-3});

   draw_c3_line((c3_t){3,0,-3},(c3_t){3,0,3});
   draw_c3_line((c3_t){3,0,-3},(c3_t){3,6,-3});
*/
/*
   //floor grid
   for(i=-21;i<21;i+=3) {
    for(j=-21;j<21;j+=3) {
     //draw_c3_triangle((struct c3_triangle){"id",(c3_t){i,0,j},(c3_t){i,0,j+3},(c3_t){i+3,0,j}});
     draw_c3_line((c3_t){i,0,j},(c3_t){i,0,j+3});
     draw_c3_line((c3_t){i,0,j},(c3_t){i+3,0,j});
    }
   }
*/
   //apply rotation?
   // load up the triangles to render... after applying rotation?
   for(i=0;global.triangle[i];i++) {
    zs[i].t=global.triangle[i];
    //rotate_triangle(zs[i].t);
   }
   //
   if(1) {//global.zsort) {
    for(i=0;global.triangle[i];i++) {
     zs[i].d=shitdist(*(zs[i].t),camera.p);
    }
    qsort(&zs,i,sizeof(zs[0]),(__compar_fn_t)compar);//sort these zs structs based on d.
   }
   //draw all triangles
   if(zs[0].t) {
    strcpy(global.selected_object,zs[0].t->id);
   }
   for(i=0;global.triangle[i];i++) {
    //now we pick the color of this triangle!

    if(!strcmp(global.selected_object,zs[i].t->id)) {
     XSetForeground(global.dpy,global.backgc,global.green.pixel);
    } else {
     if(global.greyscale) {
      if(zs[i].d > 0) {
       if(zs[i].d < 100) {
        colori=zs[i].d;
       }
      }
      XSetForeground(global.dpy,global.backgc,global.colors[100-colori].pixel);
     }
    }

    draw_c3_triangle(*(zs[i].t));
   }
   XSetForeground(global.dpy, global.backgc, global.green.pixel);
   snprintf(coords,sizeof(coords)-1,"selected object: %s",global.selected_object);
   XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
   XDrawString(global.dpy,global.backbuffer,global.backgc,global.xoff,global.height/2+(descent+0+ascent)*5,coords,strlen(coords));

   camera.p.z+=(global.split_flip)*(global.split*cosl(d2r(camera.yr+180)));
   camera.p.x+=(global.split_flip)*(global.split*sinl(d2r(camera.yr+180)));


  }


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

  snprintf(coords,sizeof(coords)-1,"%llf",a);
  XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
  XDrawString(global.dpy,w,gc,global.xoff,(descent+0+ascent)*6,coords,strlen(coords));
  snprintf(coords,sizeof(coords)-1,"%llf",points_to_angle(cs_to_c2((cs_t){global.mousex,global.mousey}),(c2_t){0,0})+(M_PIl/2));
  XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
  XDrawString(global.dpy,w,gc,global.xoff,(descent+0+ascent)*7,coords,strlen(coords));
  XDrawLine(global.dpy,w,gc,global.mousex,global.mousey,global.width/2,global.height/2);

  real c=cosl(d2r(camera.yr) - a) * distance((c2_t){x1,y1},(c2_t){WIDTH/2,HEIGHT/2});
  x2=nextX(x1,y1,d2r(camera.yr-90),c);
  y2=nextY(x1,y1,d2r(camera.yr-90),c);
  XDrawLine(global.dpy,w,gc,x1,y1,x2,y2);
*/
  camera.p.x = oldx;
  camera.p.z = oldz; //-= cn*CAMERA_SEPARATION;
//  XClearWindow(global.dpy, w);
  XCopyArea(global.dpy,global.backbuffer,w,gc,0,0,global.width,global.height,0,0);//move backbuffer to window
//  XCopyPlane(global.dpy,global.backbuffer,w,gc,0,0,global.width,global.height,0,0,0x000100);//move backbuffer to window
}

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
 len=1;
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
 struct c3_triangle *to;
 struct c3_triangle t;
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
 int i;//used to store the last triangle.

// printf("# entering load_stdin()\n");
 for(i=0;global.triangle[i];i++) ;//hop to the end.
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
// while((line=read_line_hack(stdin,0))) {//load as long there's something to load
   if(*line == '#') return 0;
   //printf("# read command: %s\n",line);
   a=line_splitter(line,&len);
   if(len > 1) {
    id=a[0];
    command=a[1];
   } else {
    printf("Ohhhh. shit.\n");
   }
//   for(i=0;i<len;i++) {
//    printf("a[%d]=\"%s\"\n",i,a[i]);
//   }
//   fflush(stdout);
   ret=1;
   if(!strcmp(command,"deleteallexcept")) {
    for(j=0;global.triangle[j];j++) {//mark first. compress later.
     if(strcmp(global.triangle[j]->id,t.id)) {
     // free(global.triangle[j]->id);
     // free(global.triangle[j]);
      global.triangle[j]=0;
     }
    }
    l=0;
    for(k=0;k<j;k++) {
     while(global.triangle[l] == 0 && l < j) l++;
     global.triangle[k]=global.triangle[l];
    }
    continue;
   }
   if(!strcmp(command,"deletegroup")) {
    for(j=0;global.triangle[j];j++) {//mark first. compress later.
     if(!strcmp(global.triangle[j]->id,t.id)) {
      free(global.triangle[j]->id);
      free(global.triangle[j]);
      global.triangle[j]=0;
     }
    }
    l=0;
    for(k=0;k<j;k++) {
     while(global.triangle[l] == 0 && l < j) l++;
     global.triangle[k]=global.triangle[l];
    }
    continue;
   }
   if(!strcmp(command,"assimilate")) {
    if(len == 3) {
     for(j=0;global.triangle[j];j++) {
      if(!strcmp(global.triangle[j]->id,a[2])) {
       free(global.triangle[j]->id);
       global.triangle[j]->id=strdup(id);
      }
     }
    }
    continue;
   }
   if(!strcmp(command,"addtriangle")) {
    if(len == 11) {
     t.id=strdup(id);
     t.p1.x=strtold(a[2],0);//second arg is just for a return value. set to 0 if you don't want it.
     t.p1.y=strtold(a[3],0);
     t.p1.z=strtold(a[4],0);
     t.p2.x=strtold(a[5],0);
     t.p2.y=strtold(a[6],0);
     t.p2.z=strtold(a[7],0);
     t.p3.x=strtold(a[8],0);
     t.p3.y=strtold(a[9],0);
     t.p3.z=strtold(a[10],0);
//     printf("%s[%s]=(%d,%d,%d),(%d,%d,%d),(%d,%d,%d)t.p1.x",command,id,t.p1.x,t.p1.y,t.p1.z,t.p1.);
     global.triangle[i]=malloc(sizeof(struct c3_triangle));
     to=global.triangle[i];
     memcpy(to,&t,sizeof(t));
     i++;
     global.triangles=i;
     global.triangle[i]=0;
    } else {
     printf("# ERROR: wrong amount of parts for addtriangle. got: %d expected: 11\n",len);
    }
    continue;
    //return ret;
   }
   if(!strcmp(command,"scaleup")) {
    for(i=0;global.triangle[i];i++) {
     if(!strcmp(global.triangle[i]->id,t.id)) {
      global.triangle[i]->p1.x*=t.p1.x;
      global.triangle[i]->p1.y*=t.p1.x;
      global.triangle[i]->p1.z*=t.p1.x;

      global.triangle[i]->p2.x*=t.p1.x;
      global.triangle[i]->p2.y*=t.p1.x;
      global.triangle[i]->p2.z*=t.p1.x;

      global.triangle[i]->p3.x*=t.p1.x;
      global.triangle[i]->p3.y*=t.p1.x;
      global.triangle[i]->p3.z*=t.p1.x;
     }
    }
    continue;
    //return ret;
   }

   if(!strcmp(command,"move")) {// extra fun if the arguments are different.
    t.id=strdup(id);
    if(len == 11) {
     t.p1.x=strtold(a[2],0);//second arg is just for a return value. set to 0 if you don't want it.
     t.p1.y=strtold(a[3],0);
     t.p1.z=strtold(a[4],0);
     t.p2.x=strtold(a[5],0);
     t.p2.y=strtold(a[6],0);
     t.p2.z=strtold(a[7],0);
     t.p3.x=strtold(a[8],0);
     t.p3.y=strtold(a[9],0);
     t.p3.z=strtold(a[10],0);
     for(i=0;global.triangle[i];i++) {
      if(!strcmp(global.triangle[i]->id,t.id)) {
       global.triangle[i]->p1.x+=t.p1.x;
       global.triangle[i]->p1.y+=t.p1.y;
       global.triangle[i]->p1.z+=t.p1.z;
       global.triangle[i]->p2.x+=t.p1.x;
       global.triangle[i]->p2.y+=t.p1.y;
       global.triangle[i]->p2.z+=t.p1.z;
       global.triangle[i]->p3.x+=t.p1.x;
       global.triangle[i]->p3.y+=t.p1.y;
       global.triangle[i]->p3.z+=t.p1.z;
/*
       global.triangle[i]->p2.x+=t.p2.x;
       global.triangle[i]->p2.y+=t.p2.y;
       global.triangle[i]->p2.z+=t.p2.z;
       global.triangle[i]->p3.x+=t.p3.x;
       global.triangle[i]->p3.y+=t.p3.y;
       global.triangle[i]->p3.z+=t.p3.z;
*/
      }
     }
    }
    else {
     printf("# ERROR: wrong amount of parts for move. got: %d expected: 11\n",len);
    }
    continue;
    //return ret;
   }
   printf("# I don't know what command you're talking about. %s\n",command);
/*   if(!strcmp(command,"rotate")) {
     for(i=0;global.triangle[i];i++) {
      global.triangle[i]->p1=rotate_c3_about()
      global.triangle[i]->p2=
      global.triangle[i]->p3=
     }
    }*/
   free(line);
   if(a) free(a);
//  }
 }
// printf("# leaving load_stdin()\n");
 return ret;
}

int export_file(FILE *fp) {//not used yet. maybe export in obj optionally?
 struct c3_triangle *to;
 int i;
 for(i=0;global.triangle[i];i++) {
  to=global.triangle[i];
  printf("%s addtriangle %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf\n",to->id,to->p1.x,to->p1.y,to->p1.z,to->p2.x,to->p2.y,to->p2.z,to->p3.x,to->p3.y,to->p3.z);
 }
 return 0;
}

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

int keypress_handler(int sym) {
  char line[1024];
  real tmpx;
//  real tmpy; //unused atm
  real tmpz;
  switch(sym) {
   case XK_Return:
    snprintf(line,sizeof(line)-1,"%s action %s\n",global.user,global.selected_object);
    selfcommand(line);
    break;
   case XK_Up:
    tmpx=WALK_SPEED*sinl(d2r(camera.yr+90));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr+90));
    camera.p.x+=tmpx;
    camera.p.z+=tmpz;
    snprintf(line,sizeof(line)-1,"%s move %Lf 0 %Lf 0 0 0 0 0 0\n",global.user,tmpx,tmpz);
    selfcommand(line);
    break;
   case XK_Down:
    tmpx=WALK_SPEED*sinl(d2r(camera.yr+270));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr+270));
    camera.p.x+=tmpx;
    camera.p.z+=tmpz;
    snprintf(line,sizeof(line)-1,"%s move %Lf 0 %Lf 0 0 0 0 0 0\n",global.user,tmpx,tmpz);
    selfcommand(line);
    break;
   case XK_Left:
    tmpx=WALK_SPEED*sinl(d2r(camera.yr));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr));
    camera.p.x+=tmpx;
    camera.p.z+=tmpz;
    snprintf(line,sizeof(line)-1,"%s move %Lf 0 %Lf 0 0 0 0 0 0\n",global.user,tmpx,tmpz);
    selfcommand(line);
    break;
   case XK_Right:
    tmpx=WALK_SPEED*sinl(d2r(camera.yr+180));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr+180));
    camera.p.x+=tmpx;
    camera.p.z+=tmpz;
    snprintf(line,sizeof(line)-1,"%s move %Lf 0 %Lf 0 0 0 0 0 0\n",global.user,tmpx,tmpz);
    selfcommand(line);
    break;
   case XK_w:
    camera.p.y+=1;
    snprintf(line,sizeof(line)-1,"%s move 0 1 0 0 0 0 0 0 0\n",global.user);
    selfcommand(line);
    break;
   case XK_s:
    camera.p.y-=1;
    snprintf(line,sizeof(line)-1,"%s move 0 -1 0 0 0 0 0 0 0\n",global.user);
    selfcommand(line);
    break;
   case XK_r:
    camera.xr+=5;
    while(camera.xr > 360) camera.xr-=360;
    break;
   case XK_y:
    camera.xr-=5;
    while(camera.xr < 0) camera.xr+=360;
    break;
   case XK_q:
    camera.yr+=5;
    while(camera.yr > 360) camera.yr-=360;
    break;
   case XK_e:
    camera.yr-=5;
    while(camera.yr < 0) camera.yr+=360;
    break;
   case XK_u:
    camera.zr+=5;
    while(camera.zr > 360) camera.zr-=360;
    break;
   case XK_o:
    camera.zr-=5;
    while(camera.zr < 0) camera.zr+=360;
    break;
   case XK_z: camera.zoom+=1; break;
   case XK_x: camera.zoom-=1; break;
   case XK_c: global.mmz*=1.1; break;
   case XK_v: global.mmz/=1.1; break;
   case XK_h: global.split+=1; break;
   case XK_j: global.split-=1; break;
   case XK_d:
    global.debug ^= 1;
    break;
   case XK_f:
    global.derp ^= 1;
    break;
   case XK_m:
    global.drawminimap += 1;
    global.drawminimap %= 4;
    break;
   case XK_a:
    global.drawsky ^= 1;
    break;
   case XK_3:
    global.draw3d += 1;
    global.draw3d %= 4;
    break;
   case XK_Escape:
    return -1;
   default:
    return 0;
    break;
 }
 return 1;
}

int main(int argc,char *argv[]) {
  Cursor cursor;
  Window w;
  XEvent e;
  XSetWindowAttributes attributes;
  Window root,child;//why do I have this?
//  XColor toss;
  int i,j;
  char tmp[64];
  unsigned int mask;
  char redraw=0;
  if(argc < 2) {
   fprintf(stderr,"usage: hackvr yourname < from_others > to_others\n");
   return 1;
  } else {
   global.user=strdup(argv[1]);
  }
  fcntl(1,F_SETFL,O_NONBLOCK);//won't work
  printf("# Opening X Display... (%s)\n",getenv("DISPLAY"));
  if((global.dpy = XOpenDisplay(0)) == NULL) return printf("# failure.\n"),2;
  else printf("# done.\n");
  global.color_map=DefaultColormap(global.dpy, DefaultScreen(global.dpy));

  printf("# generating grays...\n");
  for(i=0;i<=100;i++) {
   snprintf(tmp,sizeof(tmp),"gray%d",i);
   XAllocNamedColor(global.dpy,global.color_map,tmp,&global.colors[i],&global.colors[i]);
  }
  printf("# done.\n");
  setbuf(stdin,0);
  setbuf(stdout,0);
  assert(global.dpy);
  global.split_screen=SPLIT_SCREEN;
  global.split_flip=-1;
  global.split=5;
  global.root_window=0;
  //global.colors[0]=BlackPixel(global.dpy,DefaultScreen(global.dpy));
//  int whiteColor = //WhitePixel(global.dpy, DefaultScreen(global.dpy));
  attributes.background_pixel=global.colors[0].pixel;
  if(global.root_window) {
   w = DefaultRootWindow(global.dpy); //this is still buggy.
  } else {
   w = XCreateWindow(global.dpy,DefaultRootWindow(global.dpy),0,0,WIDTH*global.split_screen,HEIGHT,1,DefaultDepth(global.dpy,DefaultScreen(global.dpy)),InputOutput,DefaultVisual(global.dpy,DefaultScreen(global.dpy))\
                    ,CWBackPixel, &attributes);
   XSizeHints *hints=XAllocSizeHints();
   hints->min_aspect.x=4*global.split_screen;
   hints->min_aspect.y=3;
   hints->max_aspect.x=4*global.split_screen;
   hints->max_aspect.y=3;
   hints->flags=PAspect;
   XSelectInput(global.dpy, w, PointerMotionMask|StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|KeyPressMask|ExposureMask);
   XSetWMNormalHints(global.dpy,w,hints);
  }
  XMapWindow(global.dpy, w);
  XStoreName(global.dpy,w,"hackvr");
  global.w=w;
  global.greyscale=1;
  global.zsort=1;
  global.triangle[0]=0;//we'll allocate as we need more.
  global.gc=XCreateGC(global.dpy, w, 0, 0);

  global.width=WIDTH*global.split_screen;
  global.height=HEIGHT;

  global.backbuffer=XCreatePixmap(global.dpy,w,global.width,global.height,DefaultDepth(global.dpy,DefaultScreen(global.dpy)));
  global.cleanbackbuffer=XCreatePixmap(global.dpy,w,global.width,global.height,DefaultDepth(global.dpy,DefaultScreen(global.dpy)));

//backbuffer is uninitialized
  global.backgc=XCreateGC(global.dpy,global.backbuffer,0,0);

  cursor=XCreateFontCursor(global.dpy,XC_crosshair);
  XDefineCursor(global.dpy, w, cursor);

  XAllocNamedColor(global.dpy, global.color_map, "green", &global.green, &global.green);
  XSetForeground(global.dpy, global.gc, global.green.pixel);
  XSetForeground(global.dpy, global.backgc, global.colors[0].pixel);//black. we're about to draw the blank background using this.
  XSetBackground(global.dpy, global.gc, global.colors[160].pixel);
  XSetBackground(global.dpy, global.backgc, global.colors[140].pixel);

  XFillRectangle(global.dpy,global.cleanbackbuffer,global.backgc,0,0,global.width,global.height);
  XSetForeground(global.dpy, global.backgc, global.green.pixel);

//  XSetForeground(global.dpy, gc, whiteColor);
// this was taking a "long" time.
  printf("# generating sky... ");
  skypixmap=XCreatePixmap(global.dpy,w,SKYW,SKYH,DefaultDepth(global.dpy,DefaultScreen(global.dpy)));
  for(i=0;i<SKYH;i++) {
   for(j=0;j<SKYW;j++) {
    if(i==0) sky[i][j]=rand()%2;
    else {
     sky[i][j]=(SKYRULE >> ((!!sky[i-1][(j+(SKYW)-1)%(SKYW)]<<2) | (!!sky[i-1][j]<<1) | (!!sky[i-1][j+1%(SKYW)])) & 1);
     if(sky[i][j]) {
      XDrawPoint(global.dpy,skypixmap,global.backgc,j,i);
     }
    }
   }
  }
  printf("done.\n");

  global.mapxoff=global.width/2;
  global.mapyoff=global.height/2;
  global.drawminimap=0;
  global.draw3d=1;
  global.debug=0;
  camera.zoom=30.0l;
  camera.xr=270;
  camera.yr=90;
  camera.zr=0;
  global.mmz=1;
  camera.p.x=0;
  camera.p.z=6;
  camera.p.y=5;
  printf("# entering main loop\n");
  for(;;) {
    redraw=0;
    while(XPending(global.dpy)) {//these are taking too long?
     XNextEvent(global.dpy, &e);
     printf("# handling event with type: %d\n",e.type);
     switch(e.type) {
//       case Expose:
//         if(e.xexpose.count == 0) redraw=1;
//         break;
       case MotionNotify:
         redraw=1;
         XQueryPointer(global.dpy,w,&root,&child,&global.rmousex,&global.rmousey,&global.mousex,&global.mousey,&mask);
         break;
       case ButtonPress:
         redraw=1;
         global.buttonpressed=e.xbutton.button;
         break;
       case ButtonRelease:
         redraw=1;
         global.buttonpressed=0;
         break;
       case ConfigureNotify:
         redraw=1;
         XGetGeometry(global.dpy,w,&root,&global.x,&global.y,&global.width,&global.height,&global.border_width,&global.depth);
         if(global.width / global.split_screen / 4 * 3 != global.height) {
          if(global.height * 3 / 4 == global.height) {
           printf("math doesn't work.\n");
          }
          printf("# DERPY WM CANT TAKE A HINT %d / %d / 4 * 3 = %d != %d\n",global.width,global.split_screen,global.width /global.split_screen /4 * 3,global.height);
          if(global.width / global.split_screen / 4 * 3 < global.height) {
           global.height=global.width / global.split_screen / 4 * 3;
          } else {
           global.width=global.height * 3 / 4 * global.split_screen;
          }
         }
         global.mapxoff=global.width/global.split_screen/2;
         global.mapyoff=global.height/2;
         break;
       case KeyPress:
         redraw=1;
         if(keypress_handler(XLookupKeysym(&e.xkey,0)) == -1) return 0;
         break;
       default:
         printf("# received event with type: %d\n",e.type);
         break;
     }
    }
    if(load_stdin() || redraw) {
     printf("# entering draw_screen\n");
     draw_screen(global.dpy,w,global.gc);
     printf("# returned from draw_screen\n");
    }
    //usleep(10000);
  }
  return 0;
}
