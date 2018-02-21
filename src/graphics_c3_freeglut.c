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

#include <GL/freeglut_std.h>
#include <GL/freeglut_ext.h>
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


#if 0 //going to have to figure out all this stuff again if I'm using a 3d library. :/
/*
real distance2(c2_t p1,c2_t p2) {
 return sqrt(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) ));
}
*/
real distance3(c3_t p1,c3_t p2) {
 return sqrt(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) )+( (p1.z-p2.z)*(p1.z-p2.z) ));
}

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

void draw_minimap_shape(c3_s_t s) {//this should replace the draw_minimap_line and actually do circles.
/*
  //hrm... should I have functions for converting c3 points into minimap points?
  c2_s_t s2;
  s2.len=s.len;
  s2.id=s.id;
  int i=0;
  //if(gra_global.drawminimap == 1) {
    for(i=0;i<s.len+(s.len==1);i++) {
      s2.p[i]=(c2_t){(s.p[i].z - global.camera.p.z) * global.mmz,(s.p[i].x - global.camera.p.x) * global.mmz};
    }
    draw_c2_shape(s2);
 // }
*/
}

void draw_c3_shape(c3_s_t s) {//outlined. needs to be filled? //draw minimap shit in here too? probably...
/*
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
     if(gr) {
      //s2.p[i]=c3_add(gr->p,rotate_c3_yr(s.p[i],gr->p,d2r(gr->r.y)));
      s2.p[i]=c3_add(gr->p,rotate_c3_xr(
                           rotate_c3_yr(
                           rotate_c3_zr(s.p[i],gr->p,d2r(gr->r.z)
                                             ),gr->p,d2r(gr->r.y)
                                             ),gr->p,d2r(gr->r.x)
                                       )
                    );
     } else {
      s2.p[i]=s.p[i];
     }
   }
  }
  if(s.len == 1) {
    real dist=distance3(s.p[0],s.p[1]);
    s2.p[0]=s.p[0];
    s2.p[1]=c3_add(s.p[0],(c3_t){dist,0,0});
  }
  //all s2 needs to bet set before this loop.
  for(i=0;i<s.len+(s.len==1);i++) {//
   r=points_to_angle((c2_t){global.camera.p.x,global.camera.p.z},(c2_t){s2.p[i].x,s2.p[i].z});
   if(between_angles(r2d(r),(360-global.camera.r.y.d-45+360+90)%360,(360-global.camera.r.y.d+45+360+90)%360)) {
    drawthefucker=1;//damn it. somewhere in this shape needs to be drawn.
   }
  }
  if(!drawthefucker) return;//fuck yeah. didn't have to do all that other crap.
  for(i=0;i<s.len+(s.len==1);i++) {
//   if(s.len > 1) draw_minimap_line(s2.p[i],s2.p[(i+1)%s2.len]);//we shouldn't draw circles in here. //yeah. we probably should.
   s3.p[i]=c3_to_c2(s2.p[i]);//we need to convert all points in the shape if we have to draw any parts of it.
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
*/
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
 real curdist=0;
 real maxdist=0;
 c3_group_rot_t *gr=get_group_rotation(s->id);
 for(i=0;i< s->len+(s->len==1);i++) {
  if(gr) {
   curdist=shitdist2(global.camera.p,rotate_c3_yr(c3_add(gr->p,s->p[i]),gr->p,d2r(gr->r.y)));
  } else {
   curdist=shitdist2(global.camera.p,s->p[i]);//if there's no gr we're basically just this point. no rotation, not offests.
  }
  if(curdist > maxdist) maxdist=curdist;
 }
 return maxdist;
 //averaging it works ok, but I've decided to now use the farthest to say the distance something is.
 //why the fuck did we get group rotations each... nevermind. this code just sucks.
 /*
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

#endif

//functions needed by hackvr.c directly.
void set_aspect_ratio() {
}

int graphics_event_handler() {
  return 0;//dunno
}

int graphics_sub_init() {
 //create the window and shit.
 int argc=1;
 char *argv[]={"hackvr"};
 glutInit(&argc,argv);
 glutInitDisplayMode(GLUT_DOUBLE);
 glutInitWindowSize(640,480);
 glutInitWindowPosition(0,0);
 glutCreateWindow("hackvr");
 //glutDisplayFunc();
 //glutReshapeFunc();
 //initGL();
 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
 glClearDepth(1.0f);
 glEnable(GL_DEPTH_TEST);
 glDepthFunc(GL_LEQUAL);
 glShadeModel(GL_SMOOTH);//look up alternatives to all of these functions.
 glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
 return 0;//dunno
}

void draw_screen() {//welp... do something here.
  int i,j;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  //for each object:
  for(i=0;global.shape[i];i++) {//let's see if drawing not in groups is slow as fuck.
   glLoadIdentity();//resets the current matrix to default. not translation or rotation will be applied to the shapes going in...
   glTranslatef(1.5f, 0.0f, -7.0f);//group_rotation data goes into this
   switch(global.shape[i]->len) {
    case 0:
     continue;
    case 1://circle... skip for now.
     continue;
    case 2://line. dunno.
     continue;
    case 3:
     glBegin(GL_TRIANGLES);
     break;
    case 4:
     glBegin(GL_QUADS);
     break;
    default:
     continue;//fuck it.
   }
   for(j=0;j<global.shape[i]->len;j++) {
    glColor3f(1.0f,0.0f,0.0f);//red
    glVertex3f(global.shape[i]->p[j].x,global.shape[i]->p[j].y,global.shape[i]->p[j].z);
    glEnd();
   }
   //now to draw all the shapes inside this object... can I draw the shape out of order?
  }
  //
  glutSwapBuffers();
  //glutMainLoopEvent();//what do we need this for? calling the callbacks? I could do that...
}

int graphics_init() {
//some of these values set 
 global.zoom=25.0l;//I think if this is set to 1, then 1 3d unit is 1 2d unit?
 global.camera.r.x.d=0;
 global.camera.r.y.d=0;//we should be facing east.
 global.camera.r.z.d=0;
 global.mmz=1;//this is minimap zoom.

 global.shape[0]=0;//we'll allocate as we need more.
 global.camera.id=strdup(global.user);//make a copy so if we change global.user later we can reattach to this camera.
 global.group_rot[0]=&global.camera;//why do we have the camera in here? we need to prevent this from getting deleted.
 global.group_rot[1]=0;

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
