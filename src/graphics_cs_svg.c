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
#include "math.h"
#include "common.h"
#include "graphics_c3.h"//not needed?
#include "graphics_svg.h"
#include "graphics_c2.h"
#include "graphics_cs.h"

//typedef float real; //think this conflicts?

//TODO: will have to make some pixmaps get resized when the window does.
//for now set them to be as big as you think you'll ever resize the window to.

//#define SKYRULE 90
//#define SKYW (WIDTH*5)
//#define SKYH (HEIGHT/2)

//Pixmap skypixmap;
//char sky[SKYH][SKYW];

extern struct hvr_global global;
extern struct gra_global gra_global;
struct svg_global svg_global;

#ifdef GRAPHICAL

void set_luminosity_color(int lum) {
  svg_global.foreground_color=svg_global.colors[lum%100];
}


void draw_cs_line(cs_t p1,cs_t p2) {
  char tmp[1024];
  snprintf(tmp,sizeof(tmp)-1,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"%s\" />\n"
                                    ,p1.x     ,p1.y     ,p2.x     ,p2.y         ,svg_global.foreground_color);
  strcat(svg_global.backbuffer,tmp);
}

void draw_cs_text(cs_t p,char *text) {//lol. I don't even use this anymore.
/* char t[256];
 int direction,ascent,descent;
 XFontStruct *font=XLoadQueryFont(x11_global.dpy,"fixed");
 XCharStruct overall;
 snprintf(t,sizeof(t)-1,"%s",text);
 XTextExtents(font,text,strlen(text),&direction,&ascent,&descent,&overall);
 XDrawString(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,p.x,p.y+(descent+ascent),text,strlen(text));
*/
}

void draw_cs_shape(cs_s_t s) {//this is implemented as draw_cs_line... hrm. it could be moved up to graphics.c? probl no.
  char tmp[1024];
  //cs_t smouse=c2_to_cs(gra_global.mouse);
  int i;//all cs shapes can have 1, 2, or 3+ points. guess I gotta do that logic here too.
  switch(s.len) {
   case 1:
    break;
   default:
    strcat(svg_global.backbuffer,"<polygon points=\"");
    for(i=0;i<s.len+(s.len==1);i++) {//this shape is closed!
      snprintf(tmp,sizeof(tmp)-1,"%d,%d",s.p[i].x,s.p[i].y);
      strcat(svg_global.backbuffer,tmp);
      if(i != s.len-1) strcat(svg_global.backbuffer," ");//only print space after points that have a point after them.
    }
    snprintf(tmp,sizeof(tmp)-1,"\" fill=\"dark%s\" stroke=\"%s\" />\n",svg_global.foreground_color,svg_global.foreground_color);
    strcat(svg_global.backbuffer,tmp);
    break;
  }
}

void draw_cs_filled_shape(cs_s_t s) {
  int i;
  char tmp[1024];
  switch(s.len) {
   case 1:
    break;
   default:
    strcat(svg_global.backbuffer,"<polygon points=\"");
    for(i=0;i<s.len+(s.len==1);i++) {//this shape is closed!
      snprintf(tmp,sizeof(tmp)-1,"%d,%d",s.p[i].x,s.p[i].y);
      strcat(svg_global.backbuffer,tmp);
      if(i != s.len-1) strcat(svg_global.backbuffer," ");//only print space after points that have a point after them.
    }
    snprintf(tmp,sizeof(tmp)-1,"\" fill=\"%s\" stroke=\"%s\" />\n",svg_global.foreground_color,svg_global.foreground_color);
    strcat(svg_global.backbuffer,tmp);
    break;
  }
}

//should I do clipping in each graphics lib or make graphics.c just have clipping stuff?
void clear_backbuffer() {
//  strcpy(svg_global.backbuffer,"<?xml version=\"1.0\" standalone=\"no\">\n");
  strcpy(svg_global.backbuffer,"<html><head><meta http-equiv=\"refresh\" content=\"0\" /></head><body>\n");
  strcat(svg_global.backbuffer,"<svg width=\"1024\" height=\"768\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n");
}

void set_clipping_rectangle(int x,int y,int width,int height) {
/*
  XRectangle cliprect;
  cliprect.x=0;
  cliprect.y=0;
  cliprect.width=width;
  cliprect.height=height;
  XSetClipRectangles(x11_global.dpy,x11_global.backgc,x,y,&cliprect,1,Unsorted);
*/
}

void draw_mode_copy() {
/*
  XGCValues gcval;
  gcval.function=GXcopy;
  XChangeGC(x11_global.dpy,x11_global.backgc,GCFunction,&gcval);
*/
}

void draw_mode_and() {
/*
  XGCValues gcval;
  gcval.function=GXand;
  XChangeGC(x11_global.dpy,x11_global.backgc,GCFunction,&gcval);
*/
}

void draw_mode_or() {
/*
  XGCValues gcval;
  gcval.function=GXor;
  XChangeGC(x11_global.dpy,x11_global.backgc,GCFunction,&gcval);
*/
}

void red_and_blue_magic() {
  //draw_mode_or();
}

//void draw_sky() {
//  XCopyArea(x11_global.dpy,skypixmap,x11_global.backbuffer,x11_global.backgc,((global.camera.yr.d*5)+SKYW)%SKYW,0,WIDTH,gra_global.height/2,0,0);
//}

void set_color_snow() {
  //x11_global.snow=1;//override foreground color in the draw functions. drawing different grey each time.
}

void set_ansi_color(int i) {
  svg_global.foreground_color=svg_global.ansi_color[i];
}

void set_color() {
  svg_global.foreground_color=svg_global.green;
}

void set_color_red() {
  svg_global.foreground_color=svg_global.red;
}

void set_color_blue() {
  svg_global.foreground_color=svg_global.blue;
}

void flipscreen() {
  fprintf(stderr,"# flipping screen!\n");
//  strcat(svg_global.backbuffer,"</svg>\n</xml>\n");
  strcat(svg_global.backbuffer,"</svg></body></html>\n");
  int fd=open(svg_global.filename,O_TRUNC|O_WRONLY|O_CREAT);
  write(fd,svg_global.backbuffer,strlen(svg_global.backbuffer));
  close(fd);
}

void set_aspect_ratio() {
/*
 XSizeHints *hints=XAllocSizeHints();
 hints->min_aspect.x=AR_W*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1));
 hints->min_aspect.y=AR_H;
 hints->max_aspect.x=AR_W*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1));
 hints->max_aspect.y=AR_H;
 hints->flags |= PAspect;
 XSetWMNormalHints(x11_global.dpy,x11_global.w,hints);
 XFree(hints);
*/
}

void set_demands_attention() {
/*
 XWMHints *hints=XGetWMHints(x11_global.dpy,x11_global.w);
 if(!hints) hints=XAllocWMHints();
 hints->flags |= XUrgencyHint;
 XSetWMHints(x11_global.dpy,x11_global.w,hints);
 XFree(hints);
*/
}

void set_title(char *t) {
 //XStoreName(x11_global.dpy,x11_global.w,t);
}

int graphics_sub_init() {//this returns an fd we need to keep an eye one? :/
 svg_global.filename="/tmp/hackvr.html";
 svg_global.ansi_color[0]="black";
 svg_global.ansi_color[1]="blue";
 svg_global.ansi_color[2]="green";
 svg_global.ansi_color[3]="cyan";
 svg_global.ansi_color[4]="red";
 svg_global.ansi_color[5]="magenta";
 svg_global.ansi_color[6]="yellow";
 svg_global.ansi_color[7]="white";
 svg_global.ansi_color[8]=0;
/* x11_global.fd=ConnectionNumber(x11_global.dpy);//we need to to pass to libidc
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
  fprintf(stderr,"# creating window...\n");
  x11_global.w = XCreateWindow(x11_global.dpy,DefaultRootWindow(x11_global.dpy),0,0,WIDTH*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1)),HEIGHT,1,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)),InputOutput,DefaultVisual(x11_global.dpy,DefaultScreen(x11_global.dpy))\
                   ,CWBackPixel, &attributes);
  fprintf(stderr,"# done. window id: %ld\n",x11_global.w);
  set_aspect_ratio();
  XSelectInput(x11_global.dpy, x11_global.w, HV_MOUSE_X11_EVENT_MASK|HV_X11_KB_EVENT_MASK|HV_GRAPHICS_X11_EVENT_MASK);
 }
 XMapWindow(x11_global.dpy,x11_global.w);
 set_title("hackvr");//uses the globals to know what dpy and w
 x11_global.gc=XCreateGC(x11_global.dpy,x11_global.w, 0, 0);
 x11_global.backbuffer=XCreatePixmap(x11_global.dpy,x11_global.w,MAXWIDTH,MAXHEIGHT,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)));
 x11_global.cleanbackbuffer=XCreatePixmap(x11_global.dpy,x11_global.w,MAXWIDTH,MAXHEIGHT,DefaultDepth(x11_global.dpy,DefaultScreen(x11_global.dpy)));

//backbuffer is uninitialized
 x11_global.backgc=XCreateGC(x11_global.dpy,x11_global.backbuffer,0,0);

 cursor=XCreateFontCursor(x11_global.dpy,XC_crosshair);
 XDefineCursor(x11_global.dpy, x11_global.w, cursor);
*/
 svg_global.green="green";
 svg_global.red="red";
 svg_global.blue="blue";

 return -1;//what do we return in svg's case?
}

void graphics_event_handler(struct shit *me,char *line) {//line should always be empty
  redraw();
}

#endif
