#define _POSIX_C_SOURCE 200809L //for fileno and strdup
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

#include "config.h"
#include "common.h"
#include "graphics_c3.h"//not needed?
#include "graphics_x11.h"
#include "graphics_cs.h"

//typedef float real; //think this conflicts?

//TODO: will have to make some pixmaps get resized when the window does.
//for now set them to be as big as you think you'll ever resize the window to.

//#define SKYRULE 90
//#define SKYW (WIDTH*5)
//#define SKYH (HEIGHT/2)

//Pixmap skypixmap;
//char sky[SKYH][SKYW];

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

void set_luminosity_color(int lum) {
  XSetForeground(x11_global.dpy,x11_global.backgc,x11_global.colors[lum%100].pixel);
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
  //test in here whether a mouse click is within this shape's... bounding box? sure.
  cs_s_t bb;//bounding box
  int minx=s.p[0].x;
  int miny=s.p[0].y;
  int maxx=s.p[0].x;
  int maxy=s.p[0].y;
  int h;
  int i;//all cs shapes can have 1, 2, or 3+ points. guess I gotta do that logic here too.
  switch(s.len) {
   case 1:
    //circle
    h=max(s.p[0].x,s.p[1].x)-min(s.p[0].x,s.p[1].x);
    XDrawArc(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,s.p[0].x-h,s.p[0].y-h,h*2,h*2,0,360*64);
    if(distance2((c2_t){s.p[0].x,s.p[1].y},(c2_t){gra_global.mousex,gra_global.mousey} ) < h) {
     if(gra_global.buttonpressed) {
       printf("%s action %s\n",global.user,s.id);
     }
     XDrawArc(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,s.p[0].x-h-2,s.p[0].y-h-2,h*2+4,h*2+4,0,360*64);
    }
    break;
   default:
    for(i=0;i<s.len+(s.len==1);i++) {//this shape is closed!
      minx=(s.p[i].x<minx)?s.p[i].x:minx;
      miny=(s.p[i].y<miny)?s.p[i].y:miny;
      maxx=(s.p[i].x>maxx)?s.p[i].x:maxx;
      maxy=(s.p[i].y>maxy)?s.p[i].y:maxy;
      draw_cs_line(s.p[i],s.p[(i+1)%(s.len+(s.len==1))]);
    }
    if(gra_global.mousex >= minx &&
         gra_global.mousey >= miny &&
         gra_global.mousex <= maxx &&
         gra_global.mousey <= maxy) {
      if(gra_global.buttonpressed) {//if we're inside the bounding box let's make SOMETHING happen.
          printf("%s action %s\n",global.user,s.id);
        }
      if(!strncmp(s.id,"term",4)) {
       gra_global.input_mode=1;
      }
      bb.id=strdup("boundingbox");
      bb.len=4;
      bb.p[0].x=minx;
      bb.p[0].y=miny;
      bb.p[1].x=minx;
      bb.p[1].y=maxy;
      bb.p[2].x=maxx;
      bb.p[2].y=maxy;
      bb.p[3].x=maxx;
      bb.p[3].y=miny;
      draw_cs_filled_shape(bb);
      free(bb.id);
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
    h=max(s.p[0].x,s.p[1].x)-min(s.p[0].x,s.p[0].x);
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
  cliprect.x=0;
  cliprect.y=0;
  cliprect.width=width;
  cliprect.height=height;
  XSetClipRectangles(x11_global.dpy,x11_global.backgc,x,y,&cliprect,1,Unsorted);
}

void draw_mode_copy() {
  XGCValues gcval;
  gcval.function=GXcopy;
  XChangeGC(x11_global.dpy,x11_global.backgc,GCFunction,&gcval);
}

void draw_mode_and() {
  XGCValues gcval;
  gcval.function=GXand;
  XChangeGC(x11_global.dpy,x11_global.backgc,GCFunction,&gcval);
}

void draw_mode_or() {
  XGCValues gcval;
  gcval.function=GXor;
  XChangeGC(x11_global.dpy,x11_global.backgc,GCFunction,&gcval);
}

void red_and_blue_magic() {
  draw_mode_or();
}

//void draw_sky() {
//  XCopyArea(x11_global.dpy,skypixmap,x11_global.backbuffer,x11_global.backgc,((global.camera.yr.d*5)+SKYW)%SKYW,0,WIDTH,gra_global.height/2,0,0);
//}

void set_ansi_color(int i) {
  XSetForeground(x11_global.dpy,x11_global.backgc,x11_global.ansi_color[i].pixel);
}

void set_color() {
  XSetForeground(x11_global.dpy,x11_global.backgc,x11_global.green.pixel);
}

void set_color_red() {
  XSetForeground(x11_global.dpy,x11_global.backgc,x11_global.red.pixel);
}

void set_color_blue() {
  XSetForeground(x11_global.dpy,x11_global.backgc,x11_global.blue.pixel);
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
 hints->flags |= PAspect;
 XSetWMNormalHints(x11_global.dpy,x11_global.w,hints);
 XFree(hints);
}

void set_demands_attention() {
 XWMHints *hints=XGetWMHints(x11_global.dpy,x11_global.w);
 if(!hints) hints=XAllocWMHints();
 hints->flags |= XUrgencyHint;
 XSetWMHints(x11_global.dpy,x11_global.w,hints);
 XFree(hints);
}

void x11_keypress_handler(XKeyEvent *xkey,int x,int y) {
  char line[2048];
  char line2[1025];
  int len;
  radians tmprad;
  radians tmprad2;
  real tmpx;
  int i;
  int sym=XLookupKeysym(xkey,0);
  real tmpz;
  switch(gra_global.input_mode) {
   case 0:
    switch(sym) {
     case XK_Return:
      snprintf(line,sizeof(line)-1,"%s action %s\n",global.user,global.selected_object);
      selfcommand(line);
      break;
     case XK_Up:
      tmprad=d2r((degrees){global.camera.r.y.d});//if the angle is 0...
      tmprad2=d2r((degrees){global.camera.r.y.d});
      tmpx=WALK_SPEED*sin(tmprad.r);//cos(0)==1
      tmpz=WALK_SPEED*cos(tmprad2.r);//sin(0)==0
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      //snprintf(line,sizeof(line)-1,"%s move forward\n",global.user);
      selfcommand(line);
      break;
     case XK_Down:
      tmprad=d2r((degrees){global.camera.r.y.d+180});
      tmprad2=d2r((degrees){global.camera.r.y.d+180});
      tmpx=WALK_SPEED*sin(tmprad.r);
      tmpz=WALK_SPEED*cos(tmprad2.r);
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      selfcommand(line);
      break;
     case XK_Left:
      tmprad=d2r((degrees){global.camera.r.y.d+90});
      tmprad2=d2r((degrees){global.camera.r.y.d+90});
      tmpx=WALK_SPEED*sin(tmprad.r);
      tmpz=WALK_SPEED*cos(tmprad2.r);
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      selfcommand(line);
      break;
     case XK_Right:
      tmprad=d2r((degrees){global.camera.r.y.d+270});
      tmprad2=d2r((degrees){global.camera.r.y.d+270});
      tmpx=WALK_SPEED*sin(tmprad.r);
      tmpz=WALK_SPEED*cos(tmprad2.r);
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      selfcommand(line);
      break;
     case XK_w:
      snprintf(line,sizeof(line)-1,"%s move +0 +1 +0\n",global.user);
      selfcommand(line);
      break;
     case XK_s:
      snprintf(line,sizeof(line)-1,"%s move +0 +-1 +0\n",global.user);
      selfcommand(line);
      break;
     case XK_r:
      snprintf(line,sizeof(line)-1,"%s rotate +%d +0 +0\n",global.user,+ROTATE_STEP);
      selfcommand(line);
      break;
     case XK_y:
      snprintf(line,sizeof(line)-1,"%s rotate +%d +0 +0\n",global.user,-ROTATE_STEP);
      selfcommand(line);
      break;
     case XK_q:
      snprintf(line,sizeof(line)-1,"%s rotate +0 +%d +0\n",global.user,+ROTATE_STEP);
      selfcommand(line);
      break;
     case XK_e:
      snprintf(line,sizeof(line)-1,"%s rotate +0 +%d +0\n",global.user,-ROTATE_STEP);
      selfcommand(line);
      break;
     case XK_u:
      snprintf(line,sizeof(line)-1,"%s rotate +0 +0 +%d\n",global.user,ROTATE_STEP);
      selfcommand(line);
      break;
     case XK_o:
      snprintf(line,sizeof(line)-1,"%s rotate +0 +0 +%d\n",global.user,-ROTATE_STEP);
      selfcommand(line);
      break;
     case XK_p:
      gra_global.split+=.1;
      break;
     case XK_l:
      gra_global.split-=.1;
      break;
     case XK_z: global.zoom+=1; break;
     case XK_x:
      global.zoom-=1;
      if(global.zoom < 1) global.zoom=1;
      break;
     case XK_c: global.mmz*=1.1; break;
     case XK_v: global.mmz/=1.1; break;
     case XK_h: global.split+=1; break;
     case XK_j: global.split-=1; break;
     case XK_6: gra_global.maxshapes+=10; break;
     case XK_7: gra_global.maxshapes-=10; break;
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
      exit(0);
     default:
      break;
    }
    break;
   default:
    switch(sym) {
     case XK_Return:
      strcpy(line,"\n");
      len=1;
      break;
     case XK_Left://hack. probably just replace this with printf()s
      strcpy(line,"\x1b[D");
      len=3;
      break;
     case XK_Right:
      strcpy(line,"\x1b[C");
      len=3;
      break;
     case XK_Down:
      strcpy(line,"\x1b[B");
      len=3;
      break;
     case XK_Up:
      strcpy(line,"\x1b[A");
      len=3;
      break;
     default:
      len=XLookupString(xkey,line,1023,NULL,NULL);
      break;
    }
    for(i=0;i/2 < len;i++) line2[i]="0123456789abcdef"[(line[i/2]>>(4*(1-(i%2)))) % 16];
    line2[i]=0;
    printf("%s data %s\n",global.user,line2);
    break;
 }
}
#endif

int graphics_sub_init() {
 char *ansi_color[]={"black","red","green","yellow","blue","magenta","cyan","white",0};
 int i;
 char tmp[64];
 Cursor cursor;
 XSetWindowAttributes attributes;
// Window root,child;//why do I have this?
//  XColor toss;
 fprintf(stderr,"# Opening X Display... (%s)\n",getenv("DISPLAY"));
 if((x11_global.dpy = XOpenDisplay(0)) == NULL) {
  fprintf(stderr,"# failure.\n");
  exit(1);
 }
 else fprintf(stderr,"# done.\n");
 x11_global.color_map=DefaultColormap(x11_global.dpy, DefaultScreen(x11_global.dpy));
 fprintf(stderr,"# generating colors...\n");
 for(i=0;ansi_color[i];i++) {
  XAllocNamedColor(x11_global.dpy,x11_global.color_map,ansi_color[i],&x11_global.ansi_color[i],&x11_global.ansi_color[i]);
 }
 for(i=0;i<=100;i++) {
  snprintf(tmp,sizeof(tmp),"gray%d",i);
  XAllocNamedColor(x11_global.dpy,x11_global.color_map,tmp,&x11_global.colors[i],&x11_global.colors[i]);
 }
 fprintf(stderr,"# done.\n");
 assert(x11_global.dpy);
 x11_global.root_window=0;
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
 x11_global.gc=XCreateGC(x11_global.dpy,x11_global.w, 0, 0);
 x11_global.backbuffer=XCreatePixmap(x11_global.dpy,x11_global.w,MAXWIDTH,MAXHEIGHT,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)));
 x11_global.cleanbackbuffer=XCreatePixmap(x11_global.dpy,x11_global.w,MAXWIDTH,MAXHEIGHT,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)));

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

 XFillRectangle(x11_global.dpy, x11_global.cleanbackbuffer,x11_global.backgc,0,0,MAXWIDTH,MAXHEIGHT);
 XSetForeground(x11_global.dpy, x11_global.backgc,x11_global.green.pixel);

//  XSetForeground(x11_global.dpy, gc, whiteColor);
// this was taking a "long" time.
/*
 fprintf(stderr,"# generating sky... ");
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
 fprintf(stderr,"# done.\n");
*/
 return 0;//we're fine
}

int graphics_event_handler(int world_changed) { //should calling draw_screen be in here?
 int redraw=0;
 XEvent e;
 Window child,root;
 //what sets mask?
 char motionnotify=0;
 unsigned int mask;
 if(global.beep) {
  global.beep=0;
  XBell(x11_global.dpy,1000);
  set_demands_attention();
 }
 while(XPending(x11_global.dpy)) {//these are taking too long?
  XNextEvent(x11_global.dpy, &e);
//     fprintf(stderr,"# handling event with type: %d\n",e.type);
  switch(e.type) {
//       case Expose:
//         if(e.xexpose.count == 0) redraw=1;
//         break;
    case MotionNotify:
      if(global.debug >= 2) fprintf(stderr,"# MotionNotify\n");
      motionnotify=1;
      break;
    case ButtonPress:
      if(global.debug >= 2) fprintf(stderr,"# ButtonPress\n");
      redraw=1;
      gra_global.buttonpressed=e.xbutton.button;//what's this for? mouse?
      break;
    case ButtonRelease:
      if(global.debug >= 2) fprintf(stderr,"# ButtonRelease\n");
      redraw=1;
      gra_global.buttonpressed=0;//what's this for???
      break;
    case ConfigureNotify:
      if(global.debug >= 2) fprintf(stderr,"# ConfigureNotify\n");
      redraw=1;
      XGetGeometry(x11_global.dpy,x11_global.w,&root,&global.x,&global.y,&gra_global.width,&gra_global.height,&gra_global.border_width,&gra_global.depth);
      if(gra_global.height * AR_W / AR_H != gra_global.width / (gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1))) {
       // height / AR_H * AR_W = width / (ss / (rab ? ss : 1))
       if(global.debug >= 2) {
        fprintf(stderr,"# %d != %d for some reason. probably your WM not respecting aspect ratio hints or calculating based on them differently. (would cause an off-by-one or so)\n",gra_global.height * AR_W / AR_H , gra_global.width / (gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1)));
       }
       if(gra_global.width / (gra_global.red_and_blue ? 1 : gra_global.split_screen) * AR_H / AR_W < gra_global.height) {
        gra_global.height=gra_global.width / (gra_global.red_and_blue ? 1 : gra_global.split_screen) * AR_H / AR_W;
       } else {
        gra_global.width=gra_global.height * AR_W / AR_H * (gra_global.red_and_blue ? 1 : gra_global.split_screen);
       }
      }
      gra_global.mapxoff=gra_global.width/(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1))/2;
      gra_global.mapyoff=gra_global.height/2;
      break;
    case KeyPress:
      if(global.debug >= 2) fprintf(stderr,"# KeyPress\n");
      redraw=1;
      x11_keypress_handler(&e.xkey,gra_global.mousex,gra_global.mousey);
      break;
    default:
//      fprintf(stderr,"# received unknown event with type: %d\n",e.type);
      break;
  }
 }
 if(motionnotify) {
  XQueryPointer(x11_global.dpy,x11_global.w,&root,&child,&gra_global.rmousex,&gra_global.rmousey,&gra_global.mousex,&gra_global.mousey,&mask);
  redraw=1;
//  global.camera.r.x.d=gra_global.mousey - (HEIGHT/2);
//  global.camera.r.y.d=gra_global.mousex - (LEFT/2);
 }
 //redraw=1;//meh.
 if(redraw || world_changed) { 
  gra_global.input_mode=0;
  draw_screen();
 }
 return redraw;
}
