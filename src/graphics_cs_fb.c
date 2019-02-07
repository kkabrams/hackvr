#define _POSIX_C_SOURCE 200809L //for fileno and strdup
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "config.h"
#include "common.h"
#include "graphics_c3.h"//not needed?
#include "graphics_fb.h"
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
struct fb_global fb_global;

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

void set_luminosity_color(int lum) {//need to come up with a good range for this.
  fb_global.current_color=((200-lum) << 16) | ((200-lum) << 8) | lum;
}

void draw_cs_point(int x,int y) {//this should write to a backbuffer then I can memcopy it over.
 unsigned int i=y * fb_global.info.xres + x;
 if(x > fb_global.info.xres) return;
 if(y > fb_global.info.yres) return;
 if(i > fb_global.fblen) return;
 //hack to test it with. remove me later.
 fb_global.current_color=-1;
 int derp;
 derp=(fb_global.current_color == -1) ? (rand()) : fb_global.current_color;
 switch(fb_global.draw_mode) {
  case DRAW_MODE_COPY:
   fb_global.backbuf[i] = derp;
   break;
  case DRAW_MODE_OR:
   fb_global.backbuf[i] |= derp;
   break;
  case DRAW_MODE_AND:
   fb_global.backbuf[i] &= derp;
   break;
  case DRAW_MODE_CLEAR:
   fb_global.backbuf[i]=0;
   break;
  default:
   fprintf(stderr,"# derp. unknown draw_mode %d\n",fb_global.draw_mode);
   break;
 }
}

void draw_cs_arc() {//now... how to draw an arc.

}

void draw_cs_line(cs_t p1,cs_t p2) {//error somewhere in here. derp...
  int x,y;
  real m;
  int b;
  int xd,yd;
  if(p1.x == p2.x) {
    for(y=min(p1.y,p2.y);y<max(p1.y,p2.y);y++) {
      draw_cs_point(p1.x,y);
    }
  }
  if(p1.y == p2.y) {
    for(x=min(p1.x,p2.x);x<max(p1.x,p2.x);x++) {
      draw_cs_point(x,p1.y);
    }
  }
  xd=p1.x<p2.x?1:-1;
  yd=p1.y<p2.y?1:-1;
  if(max(p1.x,p2.x)-min(p1.x,p2.x) >= max(p1.y,p2.y)-min(p1.y,p2.y)) { //loop over x. like normal math. :P y=mx+b stuff.
   m=((real)(p1.y-p2.y))/((real)(p1.x-p2.x));
   b=(int)((real)p1.y - (m * (real)p1.x));
   for(x=p1.x;x!=p2.x;x+=xd) {
    y=(int)(m * (real)x + (real)b); 
    draw_cs_point(x,y);
   }
  } else { //loop over y
   m=((real)(p1.x-p2.x))/((real)(p1.y-p2.y));
   b=(int)((real)p1.x - (m * (real)p1.y));
   for(y=p1.y;y!=p2.y;y+=yd) {
     x=(int)(m * (real)y + (real)b);
     draw_cs_point(x,y);
   }
  }
  //now for the fun part.
}

void draw_cs_text(cs_t p,char *text) {
/* char t[256];
 int direction,ascent,descent;
 XFontStruct *font=XLoadQueryFont(x11_global.dpy,"fixed");
 XCharStruct overall;
 snprintf(t,sizeof(t)-1,"%s",text);
 XTextExtents(font,text,strlen(text),&direction,&ascent,&descent,&overall);
 XDrawString(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,p.x,p.y+(descent+ascent),text,strlen(text));*/
}

void draw_cs_shape(cs_s_t s) {//this is implemented as draw_cs_line... hrm. it could be moved up to graphics.c? probl no.
  //test in here whether a mouse click is within this shape's... bounding box? sure.
  cs_s_t bb;//bounding box
  int minx=s.p[0].x;
  int miny=s.p[0].y;
  int maxx=s.p[0].x;
  int maxy=s.p[0].y;
  //int h;
  int i;//all cs shapes can have 1, 2, or 3+ points. guess I gotta do that logic here too.
  switch(s.len) {
   case 1:
    //circle
    //h=max(s.p[0].x,s.p[1].x)-min(s.p[0].x,s.p[1].x);
    //XDrawArc(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,s.p[0].x-h,s.p[0].y-h,h*2,h*2,0,360*64);
    break;
   default:
    for(i=0;i<s.len+(s.len==1);i++) {//this shape is closed!
      minx=(s.p[i].x<minx)?s.p[i].x:minx;
      miny=(s.p[i].y<miny)?s.p[i].y:miny;
      maxx=(s.p[i].x>maxx)?s.p[i].x:maxx;
      maxy=(s.p[i].y>maxy)?s.p[i].y:maxy;
    //  draw_cs_line(s.p[i],s.p[(i+1)%(s.len+(s.len==1))]);
    }
    if(gra_global.mouse.x >= minx &&
         gra_global.mouse.y >= miny &&
         gra_global.mouse.x <= maxx &&
         gra_global.mouse.y <= maxy) {
      if(gra_global.mousemap[0] == -1) {//if we're inside the bounding box let's make SOMETHING happen.
          gra_global.mousemap[0]=0;
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

int x_from_y(cs_t p1,cs_t p2,int y) {//get the value of x given a y within a line.
 real m;
 int b;
 int x;
// y=mx+b MAGIC
 if(p1.x == p2.x) return p1.x;//if this happens then we have a verticle line and can just shortcut this shit.
 if(p1.y == p2.y) {//if this happens we have a horizontal line we're trying to find the X of based on a 'y' that probably isn't in the line...
  return 0;
 }//return p1.x;//WE SHOULD NOT GET HERE. fuck if I know.
// y=mx+b
// y-b=mx
// x=(y-b)/m
// b=y-mx
 m=((real)(p1.y-p2.y))/((real)(p1.x-p2.x));
 b=(int)((real)p1.y - (m * (real)p1.x));
 x=(int)(((real)(y-b))/m);
 if(!x) printf("x == %d y=%d m=%f b=%d\n",x,y,m,b);
 return x;
}

void draw_cs_filled_shape(cs_s_t s) {//no circle handling atm. and only convex polygons.
 int maxmax=0;
 int minmin=0;
 cs_t p1;
 cs_t p2;
 int i;
 int y;
 for(i=0;i<s.len;i++) {//find the minimum and maximum points. minmin is smallest x and y. maxmax is biggest x and y.
   if(s.p[i].y < s.p[minmin].y || (s.p[i].y == s.p[minmin].y && s.p[i].x < s.p[minmin].x)) minmin=i;
   if(s.p[i].y > s.p[maxmax].y || (s.p[i].y==s.p[maxmax].y && s.p[i].x > s.p[maxmax].x)) maxmax=i;
 }
 int a1=minmin;
 int a2=(minmin+1)%s.len;
 int b1=minmin;
 int b2=(minmin+s.len-1)%s.len;
 for(y=s.p[minmin].y;y<s.p[maxmax].y;y++) {//loop from minimum y to maximum y.
  //now we need to trace lines from minmin to what minmin connects to. minmin+1 and minmin-1
  while(y == s.p[a2].y) { a1=a2; a2=(a2+1)%s.len; }
  while(y == s.p[b2].y) { b1=b2; b2=(b2+s.len-1)%s.len; }
  if(s.p[a1].y != s.p[a2].y) p1=(cs_t){x_from_y(s.p[a1],s.p[a2],y),y};
  if(s.p[b1].y != s.p[b2].y) p2=(cs_t){x_from_y(s.p[b1],s.p[b2],y),y};
  draw_cs_line(p1,p2);//the two y values are always the same here.
 }
}
/*
void draw_cs_filled_shape(cs_s_t s) {
  int h;
  int i;
  //XPoint Xp[s.len+(s.len==1)];
  for(i=0;i<s.len+(s.len==1);i++) {
    //Xp[i]=(XPoint){s.p[i].x,s.p[i].y};
  }
  switch(s.len) {
   case 1:
    h=max(s.p[0].y,s.p[1].y)-min(s.p[0].y,s.p[1].y);
    h=max(s.p[0].x,s.p[1].x)-min(s.p[0].x,s.p[0].x);
    //XFillArc(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,s.p[0].x-h,s.p[0].y-h,h*2,h*2,0,360*64);
    break;
   default:
    //XFillPolygon(x11_global.dpy,x11_global.backbuffer,x11_global.backgc,Xp,s.len,Convex,CoordModeOrigin);
    break;
  }
}*/

//should I do clipping in each graphics lib or make graphics.c just have clipping stuff?
void clear_backbuffer() {
  //XCopyArea(x11_global.dpy,x11_global.cleanbackbuffer,x11_global.backbuffer,x11_global.gc,0,0,gra_global.width,gra_global.height,0,0);
  memset(fb_global.backbuf,0,fb_global.fblen);
}

void set_clipping_rectangle(int x,int y,int width,int height) {
  //XRectangle cliprect;
  //cliprect.x=0;
  //cliprect.y=0;
  //cliprect.width=width;
  //cliprect.height=height;
  //XSetClipRectangles(x11_global.dpy,x11_global.backgc,x,y,&cliprect,1,Unsorted);
}

void draw_mode_copy() {
  fb_global.draw_mode=DRAW_MODE_COPY;
}

void draw_mode_and() {
  fb_global.draw_mode=DRAW_MODE_AND;
}

void draw_mode_or() {
  fb_global.draw_mode=DRAW_MODE_OR;
}

void red_and_blue_magic() {
  draw_mode_or();
}

//void draw_sky() {
//  XCopyArea(x11_global.dpy,skypixmap,x11_global.backbuffer,x11_global.backgc,((global.camera.yr.d*5)+SKYW)%SKYW,0,WIDTH,gra_global.height/2,0,0);
//}

void set_color_snow() {
  fb_global.current_color=-1;//kek
}

void set_ansi_color(int i) {
  fb_global.current_color=(i&4?0xff0000:0x0) | (i&2 ?0xff00:0x0) | (i&1?0xff:0x0);// :D
}

void set_color() {
  fb_global.current_color=0x00ff00;
}

void set_color_red() {
  fb_global.current_color=0xff0000;
}

void set_color_blue() {
  fb_global.current_color=0x0000ff;
}

void flipscreen() {
  memcpy(fb_global.buf,fb_global.backbuf,fb_global.fblen);//
}

void set_aspect_ratio() {
 //XSizeHints *hints=XAllocSizeHints();
 //hints->min_aspect.x=AR_W*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1));
 //hints->min_aspect.y=AR_H;
 //hints->max_aspect.x=AR_W*(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen : 1));
 //hints->max_aspect.y=AR_H;
 //hints->flags |= PAspect;
 //XSetWMNormalHints(x11_global.dpy,x11_global.w,hints);
 //XFree(hints);
}

void set_demands_attention() {
 //XWMHints *hints=XGetWMHints(x11_global.dpy,x11_global.w);
 //if(!hints) hints=XAllocWMHints();
 //hints->flags |= XUrgencyHint;
 //XSetWMHints(x11_global.dpy,x11_global.w,hints);
 //XFree(hints);
}

#endif

int graphics_sub_init() {//some of this is keyboard init... should it be moved out? probably.
  //int i;
  int x,y;
  fb_global.fb=open("/dev/fb0",O_RDWR);
  fb_global.kb=open("/dev/input/event0",O_RDWR);
  memset(fb_global.keystate,0,sizeof(fb_global.keystate));
  fcntl(fb_global.kb,F_SETFL,O_NONBLOCK);
  assert(fb_global.fb > 0);
  assert( 0 == ioctl(fb_global.fb,FBIOGET_VSCREENINFO,&fb_global.info));
  fb_global.fblen = 4 * fb_global.info.xres * fb_global.info.yres;
  fb_global.buf = mmap(NULL,fb_global.fblen,PROT_READ | PROT_WRITE,MAP_SHARED, fb_global.fb,0);
  fb_global.backbuf = malloc(fb_global.fblen);
  assert(fb_global.buf != MAP_FAILED);
  //now... how to generate 200 greys....
  //heh.
  for(x=0;x<fb_global.info.xres;x++) {
   for(y=0;y<fb_global.info.yres;y++) {
    fb_global.current_color=rand();
    draw_cs_point(x,y);
   }
  }
  flipscreen();
  gra_global.width=fb_global.info.xres;
  gra_global.height=fb_global.info.yres;
  return 0;
}

/* this need a framebuffer equivalent
 for(i=0;i<=100;i++) {
  snprintf(tmp,sizeof(tmp),"gray%d",i);
  XAllocNamedColor(x11_global.dpy,x11_global.color_map,tmp,&x11_global.colors[i],&x11_global.colors[i]);
 }
 return 0;//we're fine
}
*/

//    printf("  %d <-- len\t:code:%d\tvalue:%d\ttype:%d\n",l,ie.code,ie.value,ie.type);

#define KEY_IS_DOWN(a) (fb_global.keystate[a/8] & (1<< (a % 8)))

int graphics_event_handler(int world_changed) { //should calling draw_screen be in here?
#if 0
 int redraw=0;
  char line[2048];
  char line2[1025];
  int len;
 struct input_event ie;
 memset(&ie,0,sizeof(ie));
 int l;
  real tmpx;
  real tmpz;
  radians tmprad;
  radians tmprad2;
  int i;
 //update the keystates...
 ioctl(fb_global.kb,EVIOCGKEY(sizeof(fb_global.keystate)),fb_global.keystate);//update keystate
 read(fb_global.kb,&ie,sizeof(ie));//????
//need to not be using KEY_IS_DOWN so that it gets these events instead of just checking when it gets to this loop.
 if(KEY_IS_DOWN(KEY_ESC)) exit(0);
 if(KEY_IS_DOWN(KEY_ENTER)) {
      snprintf(line,sizeof(line)-1,"%s action %s\n",global.user,global.selected_object);
      selfcommand(line);
 }
 if(KEY_IS_DOWN(KEY_UP)) {
      tmprad=d2r((degrees){global.camera.r.y.d});//if the angle is 0...
      tmprad2=d2r((degrees){global.camera.r.y.d});
      tmpx=WALK_SPEED*sin(tmprad.r);//cos(0)==1
      tmpz=WALK_SPEED*cos(tmprad2.r);//sin(0)==0
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      selfcommand(line);
 }
 if(KEY_IS_DOWN(KEY_DOWN)) {
      tmprad=d2r((degrees){global.camera.r.y.d+180});
      tmprad2=d2r((degrees){global.camera.r.y.d+180});
      tmpx=WALK_SPEED*sin(tmprad.r);
      tmpz=WALK_SPEED*cos(tmprad2.r);
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      selfcommand(line);
 }
 if(KEY_IS_DOWN(KEY_LEFT)) {
      tmprad=d2r((degrees){global.camera.r.y.d+90});
      tmprad2=d2r((degrees){global.camera.r.y.d+90});
      tmpx=WALK_SPEED*sin(tmprad.r);
      tmpz=WALK_SPEED*cos(tmprad2.r);
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      selfcommand(line);
 }
     if(KEY_IS_DOWN(KEY_RIGHT)) {
      tmprad=d2r((degrees){global.camera.r.y.d+270});
      tmprad2=d2r((degrees){global.camera.r.y.d+270});
      tmpx=WALK_SPEED*sin(tmprad.r);
      tmpz=WALK_SPEED*cos(tmprad2.r);
      snprintf(line,sizeof(line)-1,"%s move +%f +0 +%f\n",global.user,tmpx,tmpz);
      selfcommand(line);
      }
     if(KEY_IS_DOWN(KEY_W)) {
      snprintf(line,sizeof(line)-1,"%s move +0 +1 +0\n",global.user);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_S)) {
      snprintf(line,sizeof(line)-1,"%s move +0 +-1 +0\n",global.user);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_R)) {
      snprintf(line,sizeof(line)-1,"%s rotate +%d +0 +0\n",global.user,ROTATE_STEP);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_Y)) {
      snprintf(line,sizeof(line)-1,"%s rotate +%d +0 +0\n",global.user,-ROTATE_STEP);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_Q)) {
      snprintf(line,sizeof(line)-1,"%s rotate +0 +%d +0\n",global.user,ROTATE_STEP);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_E)) {
      snprintf(line,sizeof(line)-1,"%s rotate +0 +%d +0\n",global.user,-ROTATE_STEP);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_U)) {
      snprintf(line,sizeof(line)-1,"%s rotate +0 +0 +%d\n",global.user,ROTATE_STEP);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_O)) {
      snprintf(line,sizeof(line)-1,"%s rotate +0 +0 +%d\n",global.user,-ROTATE_STEP);
      selfcommand(line);
     }
     if(KEY_IS_DOWN(KEY_P)) gra_global.split+=.1;
     if(KEY_IS_DOWN(KEY_L)) gra_global.split-=.1;
     if(KEY_IS_DOWN(KEY_Z)) global.zoom+=1;
     if(KEY_IS_DOWN(KEY_X)) {
      global.zoom-=1;
      if(global.zoom < 1) global.zoom=1;
     }
     if(KEY_IS_DOWN(KEY_C)) global.mmz*=1.1;
     if(KEY_IS_DOWN(KEY_V)) global.mmz/=1.1;
     if(KEY_IS_DOWN(KEY_H)) global.split+=1;
     if(KEY_IS_DOWN(KEY_J)) global.split-=1;
     if(KEY_IS_DOWN(KEY_6)) gra_global.maxshapes+=10;
     if(KEY_IS_DOWN(KEY_7)) gra_global.maxshapes-=10;
     if(KEY_IS_DOWN(KEY_D)) global.debug ^= 1;
     if(KEY_IS_DOWN(KEY_F)) global.derp ^= 1;
     if(KEY_IS_DOWN(KEY_M)) {
      gra_global.drawminimap += 1;
      gra_global.drawminimap %= 4;
     }
     if(KEY_IS_DOWN(KEY_A)) gra_global.drawsky ^= 1;
     if(KEY_IS_DOWN(KEY_3)) {
      gra_global.draw3d += 1;
      gra_global.draw3d %= 4;
     }
//LONG comment. then rest of function. don't end it too early, derp.
/*
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

 //then do stuff based on what keystates are set.
 //... loop over the whole array?
 //have an array of deltas?
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
 }
*/
#endif
 if(world_changed) {
  gra_global.input_mode=0;
  draw_screen();//includes its own flip.
 }
 return 1;//redraw;
}
