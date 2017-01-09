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

#include "config.h"
#include "common.h"
#include "graphics.h"
#include "graphics_x11.h"

//typedef float real; //think this conflicts?

//TODO: will have to make some pixmaps get resized when the window does.
//for now set them to be as big as you think you'll ever resize the window to.

#define SKYRULE 90
#define SKYW (WIDTH*5)
#define SKYH (HEIGHT/2)

Pixmap skypixmap;
char sky[SKYH][SKYW];

extern struct global global;
extern struct gra_global gra_global;
struct x11_global x11_global;

#ifdef GRAPHICAL

/*
struct plane {
 c3_t p;
 real xr;
 real yr;
 real zr;
}

void calculate_shape_color(c3_s_t s,real d) {
//given: at least 3 points on a plane.
//needed: angles of the plane.
//
 xa=
 ya=
 za=
 for(i=0;i<;i++) {
   s.p.x
 }
}
*/
void set_color_based_on_distance(real d) {
  int i=100-((int)((d+100.0) * 16.0) % 100);
  XSetForeground(x11_global.dpy,x11_global.backgc,x11_global.colors[i].pixel);
}

void draw_cs_line(cs_t p1,cs_t p2) {
  XDrawLine(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,p1.x,p1.y,p2.x,p2.y);
}

void draw_cs_text(cs_t p,char *text) {
 char t[256];
 int direction,ascent,descent;
 XFontStruct *font=XLoadQueryFont(x11_global.dpy,"fixed");
 XCharStruct overall;
 snprintf(t,sizeof(t)-1,"%s",text);
 XTextExtents(font,text,strlen(text),&direction,&ascent,&descent,&overall);
 XDrawString(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,p.x,p.y+(descent+ascent),text,strlen(text));
}

void draw_cs_shape(cs_s_t s) {//this is implemented as draw_cs_line... hrm. it could be moved up to graphics.c? probl no.
  int h;
  int i;//all cs shapes can have 1, 2, or 3+ points. guess I gotta do that logic here too.
  switch(s.len) {
   case 1:
    //cicle
    h=max(s.p[0].y,s.p[1].y)-min(s.p[0].y,s.p[1].y);
    XDrawArc(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,s.p[0].x-h,s.p[0].y-h,h*2,h*2,0,360*64);
    break;
   default:
    for(i=0;i<s.len+(s.len==1);i++) {//this shape is closed!
      draw_cs_line(s.p[i],s.p[(i+1)%(s.len+(s.len==1))]);
    }
    break;
  }
}

void draw_cs_filled_shape(cs_s_t s) {
  int h;
  int i;
  XPoint Xp[s.len+(s.len==1)];
  for(i=0;i<s.len+(s.len==1);i++) {
    Xp[i]=(XPoint){s.p[i].x,s.p[i].y};
  }
  switch(s.len) {
   case 1:
    h=max(s.p[0].y,s.p[1].y)-min(s.p[0].y,s.p[1].y);
    XFillArc(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,s.p[0].x-h,s.p[0].y-h,h*2,h*2,0,360*64);
    break;
   default:
    XFillPolygon(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,Xp,s.len,Convex,CoordModeOrigin);
    break;
  }
}

//should I do clipping in each graphics lib or make graphics.c just have clipping stuff?
void clear_backbuffer() {
  XCopyArea(x11_global.dpy,x11_global.cleanbackbuffer,x11_global.backbuffer,x11_global.gc,0,0,gra_global.width,gra_global.height,0,0);
}

void set_clipping_rectangle(int x,int y,int width,int height) {
  XRectangle cliprect;
  cliprect.x=x;
  cliprect.y=y;
  cliprect.width=width;
  cliprect.height=height;
  XSetClipRectangles(x11_global.dpy,x11_global.backgc,x,y,&cliprect,1,Unsorted);
}

void red_and_blue_magic() {
  XGCValues gcval;
  gcval.function=GXor;
  XChangeGC(x11_global.dpy,x11_global.backgc,GCFunction,&gcval);
}

void draw_sky() {
  XCopyArea(x11_global.dpy,skypixmap,x11_global.backbuffer,x11_global.backgc,((camera.yr.d*5)+SKYW)%SKYW,0,WIDTH,gra_global.height/2,0,0);
}

void set_color() {
  XSetForeground(x11_global.dpy,x11_global.backgc,x11_global.green.pixel);
}

void flipscreen() {
  XCopyArea(x11_global.dpy,x11_global.backbuffer,x11_global.w,x11_global.gc,0,0,gra_global.width,gra_global.height,0,0);
}

void set_aspect_ratio() {
 XSizeHints *hints=XAllocSizeHints();
 hints->min_aspect.x=AR_W*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1));
 hints->min_aspect.y=AR_H;
 hints->max_aspect.x=AR_W*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1));
 hints->max_aspect.y=AR_H;
 hints->flags=PAspect;
 XSetWMNormalHints(x11_global.dpy,x11_global.w,hints);
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
    tmpx=WALK_SPEED*sinl(d2r(camera.yr.d+90));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr.d+90));
    camera.p.x+=tmpx;
    camera.p.z+=tmpz;
    snprintf(line,sizeof(line)-1,"%s move %Lf 0 %Lf 0 0 0 0 0 0\n",global.user,tmpx,tmpz);
    selfcommand(line);
    break;
   case XK_Down:
    tmpx=WALK_SPEED*sinl(d2r(camera.yr.d+270));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr.d+270));
    camera.p.x+=tmpx;
    camera.p.z+=tmpz;
    snprintf(line,sizeof(line)-1,"%s move %Lf 0 %Lf 0 0 0 0 0 0\n",global.user,tmpx,tmpz);
    selfcommand(line);
    break;
   case XK_Left:
    tmpx=WALK_SPEED*sinl(d2r(camera.yr.d));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr.d));
    camera.p.x+=tmpx;
    camera.p.z+=tmpz;
    snprintf(line,sizeof(line)-1,"%s move %Lf 0 %Lf 0 0 0 0 0 0\n",global.user,tmpx,tmpz);
    selfcommand(line);
    break;
   case XK_Right:
    tmpx=WALK_SPEED*sinl(d2r(camera.yr.d+180));
    tmpz=WALK_SPEED*cosl(d2r(camera.yr.d+180));
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
    camera.xr.d+=5;
    while(camera.xr.d > 360) camera.xr.d-=360;
    break;
   case XK_y:
    camera.xr.d-=5;
    while(camera.xr.d < 0) camera.xr.d+=360;
    break;
   case XK_q:
    camera.yr.d+=5;
    while(camera.yr.d > 360) camera.yr.d-=360;
    break;
   case XK_e:
    camera.yr.d-=5;
    while(camera.yr.d < 0) camera.yr.d+=360;
    break;
   case XK_u:
    camera.zr.d+=5;
    while(camera.zr.d > 360) camera.zr.d-=360;
    break;
   case XK_o:
    camera.zr.d-=5;
    while(camera.zr.d < 0) camera.zr.d+=360;
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
    gra_global.drawminimap += 1;
    gra_global.drawminimap %= 4;
    break;
   case XK_a:
    gra_global.drawsky ^= 1;
    break;
   case XK_3:
    gra_global.draw3d += 1;
    gra_global.draw3d %= 4;
    break;
   case XK_Escape:
    return -1;
   default:
    return 0;
    break;
 }
 return 1;
}
#endif

int graphics_init() {
 int i,j;
 char tmp[64];
 Cursor cursor;
 XSetWindowAttributes attributes;
// Window root,child;//why do I have this?
//  XColor toss;
 printf("# Opening X Display... (%s)\n",getenv("DISPLAY"));
 if((x11_global.dpy = XOpenDisplay(0)) == NULL) return printf("# failure.\n"),2;
 else printf("# done.\n");
 x11_global.color_map=DefaultColormap(x11_global.dpy, DefaultScreen(x11_global.dpy));
 printf("# generating grays...\n");
 for(i=0;i<=100;i++) {
  snprintf(tmp,sizeof(tmp),"gray%d",i);
  XAllocNamedColor(x11_global.dpy,x11_global.color_map,tmp,&x11_global.colors[i],&x11_global.colors[i]);
 }
 printf("# done.\n");
 assert(x11_global.dpy);
 gra_global.split_screen=SPLIT_SCREEN;
 gra_global.split_flip=-1;
 gra_global.split=5;
 x11_global.root_window=0;
 gra_global.red_and_blue=RED_AND_BLUE;
 //global.colors[0]=BlackPixel(x11_global.dpy,DefaultScreen(x11_global.dpy));
// int whiteColor = //WhitePixel(x11_global.dpy, DefaultScreen(x11_global.dpy));
 attributes.background_pixel=x11_global.colors[0].pixel;
 if(x11_global.root_window) {
  x11_global.w = DefaultRootWindow(x11_global.dpy); //this is still buggy.
 } else {
  x11_global.w = XCreateWindow(x11_global.dpy,DefaultRootWindow(x11_global.dpy),0,0,WIDTH*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1)),HEIGHT,1,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)),InputOutput,DefaultVisual(x11_global.dpy,DefaultScreen(x11_global.dpy))\
                   ,CWBackPixel, &attributes);
  set_aspect_ratio();
  XSelectInput(x11_global.dpy, x11_global.w, PointerMotionMask|StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|KeyPressMask|ExposureMask);
 }
 XMapWindow(x11_global.dpy,x11_global.w);
 XStoreName(x11_global.dpy,x11_global.w,"hackvr");
 gra_global.greyscale=1;
 gra_global.zsort=1;
 global.shape[0]=0;//we'll allocate as we need more.
 x11_global.gc=XCreateGC(x11_global.dpy,x11_global.w, 0, 0);

 if(gra_global.red_and_blue) {
  gra_global.width=WIDTH;
 } else {
  gra_global.width=WIDTH*gra_global.split_screen;
 }
 gra_global.height=HEIGHT;

 x11_global.backbuffer=XCreatePixmap(x11_global.dpy,x11_global.w,gra_global.width,gra_global.height,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)));
 x11_global.cleanbackbuffer=XCreatePixmap(x11_global.dpy,x11_global.w,gra_global.width,gra_global.height,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)));

//backbuffer is uninitialized
 x11_global.backgc=XCreateGC(x11_global.dpy,x11_global.backbuffer,0,0);

 cursor=XCreateFontCursor(x11_global.dpy,XC_crosshair);
 XDefineCursor(x11_global.dpy, x11_global.w, cursor);

 XAllocNamedColor(x11_global.dpy, x11_global.color_map, "green", &x11_global.green, &x11_global.green);
 XAllocNamedColor(x11_global.dpy, x11_global.color_map, "red", &x11_global.red, &x11_global.red);
 XAllocNamedColor(x11_global.dpy, x11_global.color_map, "blue", &x11_global.blue, &x11_global.blue);
 XAllocNamedColor(x11_global.dpy, x11_global.color_map, "red", &x11_global.redblue[0], &x11_global.redblue[0]);
 XAllocNamedColor(x11_global.dpy, x11_global.color_map, "blue", &x11_global.redblue[1], &x11_global.redblue[1]);
 XSetForeground(x11_global.dpy, x11_global.gc, x11_global.green.pixel);
 XSetForeground(x11_global.dpy, x11_global.backgc, x11_global.colors[0].pixel);//black. we're about to draw the blank background using this.
 XSetBackground(x11_global.dpy, x11_global.gc, x11_global.colors[160].pixel);
 XSetBackground(x11_global.dpy, x11_global.backgc, x11_global.colors[140].pixel);

 XFillRectangle(x11_global.dpy, x11_global.cleanbackbuffer,x11_global.backgc,0,0,gra_global.width,gra_global.height);
 XSetForeground(x11_global.dpy, x11_global.backgc,x11_global.green.pixel);

//  XSetForeground(x11_global.dpy, gc, whiteColor);
// this was taking a "long" time.
 printf("# generating sky... ");
 skypixmap=XCreatePixmap(x11_global.dpy,x11_global.w,SKYW,SKYH,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)));
 for(i=0;i<SKYH;i++) {
  for(j=0;j<SKYW;j++) {
   if(i==0) sky[i][j]=rand()%2;
   else {
    sky[i][j]=(SKYRULE >> ((!!sky[i-1][(j+(SKYW)-1)%(SKYW)]<<2) | (!!sky[i-1][j]<<1) | (!!sky[i-1][j+1%(SKYW)])) & 1);
    if(sky[i][j]) {
     XDrawPoint(x11_global.dpy,skypixmap,x11_global.backgc,j,i);
    }
   }
  }
 }
 printf("done.\n");

 gra_global.mapxoff=gra_global.width/2;
 gra_global.mapyoff=gra_global.height/2;
 gra_global.drawminimap=DEFAULT_MINIMAP;
 gra_global.draw3d=1;
 gra_global.force_redraw=FORCE_REDRAW;//use this for checking proper fps I guess.

//this should be in graphics.c ?
 camera.zoom=30.0l;
 camera.xr.d=270;
 camera.yr.d=90;
 camera.zr.d=0;
 global.mmz=1;
 camera.p.x=0;
 camera.p.z=-6;
 camera.p.y=5;
 return 0;
}

int graphics_event_handler() {
 int redraw=0;
 XEvent e;
 Window child,root;
 //what sets mask?
 unsigned int mask;
 while(XPending(x11_global.dpy)) {//these are taking too long?
  XNextEvent(x11_global.dpy, &e);
//     printf("# handling event with type: %d\n",e.type);
  switch(e.type) {
//       case Expose:
//         if(e.xexpose.count == 0) redraw=1;
//         break;
    case MotionNotify:
      printf("# MotionNotify\n");
      XQueryPointer(x11_global.dpy,x11_global.w,&root,&child,&gra_global.rmousex,&gra_global.rmousey,&gra_global.mousex,&gra_global.mousey,&mask);
      redraw=1;
      break;
    case ButtonPress:
      printf("# ButtonPress\n");
      redraw=1;
      gra_global.buttonpressed=e.xbutton.button;//what's this for? mouse?
      break;
    case ButtonRelease:
      printf("# ButtonRelease\n");
      redraw=1;
      gra_global.buttonpressed=0;//what's this for???
      break;
    case ConfigureNotify:
      printf("# ConfigureNotify\n");
      redraw=1;
      XGetGeometry(x11_global.dpy,x11_global.w,&root,&global.x,&global.y,&gra_global.width,&gra_global.height,&gra_global.border_width,&gra_global.depth);
      if(gra_global.height * AR_W / AR_H != gra_global.width / (gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1))) {
       // height / AR_H * AR_W = width / (ss / (rab ? ss : 1))
       printf("# %d != %d for some reason. probably your WM not respecting aspect ratio hints or calculating based on them differently. (would cause an off-by-one or so)\n",gra_global.height * AR_W / AR_H , gra_global.width / (gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1)));

       if(gra_global.width / (gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1)) * AR_H / AR_W < gra_global.height) {
        gra_global.height=gra_global.width / (gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1)) * AR_H / AR_W;
       } else {
        gra_global.width=gra_global.height * AR_H / AR_W * (gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1));
       }
      }
      gra_global.mapxoff=gra_global.width/(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1))/2;
      gra_global.mapyoff=gra_global.height/2;
      break;
    case KeyPress:
      printf("# KeyPress\n");
      redraw=1;
      if(keypress_handler(XLookupKeysym(&e.xkey,0)) == -1) {
       printf("# exiting\n");
       return -1;
      }
      break;
    default:
//      printf("# received unknown event with type: %d\n",e.type);
      break;
  }
 }
 return redraw;
}
