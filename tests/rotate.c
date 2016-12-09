#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#define __USE_GNU //for longer math constants
#include <math.h>

//#define DEBUG

#define SPLIT_SCREEN 2
#define CAMERA_SEPARATION 3

#define DEPTH_FACTOR 0.965

#define TRIANGLES 256

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

//for one camera, not the whole thing.
//3 camera
//#define WIDTH 320
//#define HEIGHT 240
//1 camera
//#define WIDTH 800
//#define HEIGHT 600
//2 camera
#define WIDTH 400
#define HEIGHT 300

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
  int x;
  int y;
  int z;
  int xr;//rotations
  int yr;
  int zr;
} camera;

struct triangle {//use array or linked list?
  char *id;
  int x1;
  int y1;
  int z1;
  int x2;
  int y2;
  int z2;
  int x3;
  int y3;
  int z3;
//  int dist;//most recent distance calculated from the camera
};

struct mainwin {
  int x;
  int y;
  int depth;
  int mousex;
  int mousey;
  int rmousex;
  int rmousey;
  int buttonpressed;
  int width;
  int height;
  int border_width;
  int xoff;
  int math_error;
  char *user;
  XColor green;
  Colormap color_map;
  Display *dpy;
  Window w;
  GC gc;
  struct triangle *triangle[TRIANGLES];
} global;


float zmagic(int z) {
 float tmp=pow(DEPTH_FACTOR,(camera.z-z));
// float tmp=pow(DEPTH_FACTOR,(camera.z-z))/(camera.z-z);
// float tmp=pow(DEPTH_FACTOR,(camera.z-z)*(camera.z-z)*(camera.z-z));
 //measure the distance form the camera and error out if it is too far away.
// if((camera.z - z) >= 0) return(global.math_error=1);
 //return (float)1 / (float)(camera.z - z);
 return tmp;
}

int to2D(int cw,int w,int z,int d) {
  return (d/2) - (((w-cw) / zmagic(z)) * 16 );
}


float distance(int x1,int y1,int x2,int y2) {
 return sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
}

long double d2r(int d) {
 while(d<0) d+=360;
 return (long double)(d%360) / 180.0l * M_PIl;
}

int rotateXabout(int x1,int y1,int z1,int x2,int y2,int z2,int degrees) {
 long double radians=(long double)degrees / (long double)180 * M_PIl;//M_PIl for long double
 long double radius=distance(x1,z1,x2,z2);
 if(radius == 0) return x2;
 return x2 + radius * cosl(acosl(((long double)x2-(long double)x1)/radius)+radians);
}

int rotateZabout(int x1,int y1,int z1,int x2,int y2,int z2,int degrees) {
 long double radians=(long double)degrees / (long double)180 * M_PIl;//M_PIl for long double
 long double radius=distance(x1,z1,x2,z2);
 if(radius == 0) return z2;
 return z2 + radius * sinl(asinl(((long double)z2-(long double)z1)/radius)+radians);
}


int to2Dx(int x,int y,int z) {
  int newx=rotateXabout(x,y,z,camera.x,camera.y,camera.z,camera.yr);
  int newy=y;//rotateYabout(x,y,z,camera.x,camera.y,camera.z,camera.xr);
  int newz=rotateZabout(x,y,z,camera.x,camera.y,camera.z,camera.yr);
  return to2D(camera.x,newx,newz,global.width/SPLIT_SCREEN);
//  return to2D(camera.x,x,z,global.width/SPLIT_SCREEN);
}

int to2Dy(int x,int y,int z) {
  int newx=rotateXabout(x,y,z,camera.x,camera.y,camera.z,camera.yr);
  int newy=y;//rotateYabout(x,y,z,camera.x,camera.y,camera.z,camera.yr);
  int newz=rotateZabout(x,y,z,camera.x,camera.y,camera.z,camera.yr);
  return to2D(camera.y,newy,newz,global.height);
//  return to2D(camera.y,y,z,global.height);
}

void XDrawTriangle(int x1,int y1,int x2,int y2,int x3,int y3) {
 int x;
 XDrawLine(global.dpy,global.w,global.gc,x1,y1,x2,y2);
 XDrawLine(global.dpy,global.w,global.gc,x2,y2,x3,y3);
 XDrawLine(global.dpy,global.w,global.gc,x3,y3,x1,y1);
}

void XDrawFilledTriangle(int x1,int y1,int x2,int y2,int x3,int y3) {
 int x,y;
 int b;
 float m;

 XDrawLine(global.dpy,global.w,global.gc,x1,y1,x2,y2);
 XDrawLine(global.dpy,global.w,global.gc,x2,y2,x3,y3);
 XDrawLine(global.dpy,global.w,global.gc,x3,y3,x1,y1);

 m=(float)(y1-y2)/(float)(x1-x2);
 b=y1-(m*x1);
 for(x=min(x1,x2);x<max(x1,x2);x++) {
  y=m*x+b;
  XDrawLine(global.dpy,global.w,global.gc,x3,y3,x,y);
 }

 m=(float)(y2-y3)/(float)(x2-x3);
 b=y2-(m*x2);
 for(x=min(x2,x3);x<max(x2,x3);x++) {
  y=m*x+b;
  XDrawLine(global.dpy,global.w,global.gc,x1,y1,x,y);
 }

 m=(float)(y3-y1)/(float)(x3-x1);
 b=y3-(m*x3);
 for(x=min(x3,x1);x<max(x3,x1);x++) {
  y=m*x+b;
  XDrawLine(global.dpy,global.w,global.gc,x2,y2,x,y);
 }
}

void draw3Dtriangle(int x1,int y1,int z1,int x2,int y2,int z2,int x3,int y3,int z3) {
  char coords[256];
  global.math_error=0;
  int tx1=to2Dx(x1,y1,z1);
  int ty1=to2Dy(x1,y1,z1);
  //draw string...
  int tx2=to2Dx(x2,y2,z2);
  int ty2=to2Dy(x2,y2,z2);
  int tx3=to2Dx(x3,y3,z3);
  int ty3=to2Dy(x3,y3,z3);
  if(!global.math_error) {
#ifdef DEBUG
     snprintf(coords,sizeof(coords)-1,"(%d,%d,%d)",x1,y1,z1);
     XDrawString(global.dpy,global.w,global.gc,global.xoff+tx1,ty1,coords,strlen(coords));
     snprintf(coords,sizeof(coords)-1,"(%d,%d,%d)",x2,y2,z2);
     XDrawString(global.dpy,global.w,global.gc,global.xoff+tx2,ty2,coords,strlen(coords));
     snprintf(coords,sizeof(coords)-1,"(%d,%d,%d)",x3,y3,z3);
     XDrawString(global.dpy,global.w,global.gc,global.xoff+tx3,ty3,coords,strlen(coords));
#endif
    XDrawTriangle(global.xoff+tx1,ty1,global.xoff+tx2,ty2,global.xoff+tx3,ty3);
//    XDrawFilledTriangle(global.xoff+tx1,ty1,global.xoff+tx2,ty2,global.xoff+tx3,ty3);
  }
}

void draw3Dline(int x1,int y1,int z1,int x2,int y2,int z2) {
  global.math_error=0;
  int tx1=to2Dx(x1,y1,z1);
  int ty1=to2Dy(x1,y1,z1);
  int tx2=to2Dx(x2,y2,z2);
  int ty2=to2Dy(x2,y2,z2);
  if(!global.math_error) {
    XDrawLine(global.dpy,global.w,global.gc,global.xoff+tx1,ty1,global.xoff+tx2,ty2);
  }
  global.math_error=0;
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

void drawCube(int x,int y,int z,int i) {
//this is just drawing the cube.
    draw3Dline(x,y,z,x,y,z+i);
    draw3Dline(x,y,z,x,y+i,z);
    draw3Dline(x,y,z,x+i,y,z);

    draw3Dline(x+i,y+i,z+0,x+i,y+0,z+0);
    draw3Dline(x+i,y+i,z+0,x+0,y+i,z+0);

    draw3Dline(x+0,y+i,z+i,x+0,y+i,z+0);
    draw3Dline(x+0,y+i,z+i,x+0,y+0,z+i);

    draw3Dline(x+i,y+0,z+i,x+i,y+0,z+0);
    draw3Dline(x+i,y+0,z+i,x+0,y+0,z+i);

    draw3Dline(x+i,y+i,z+i,x+i,y+i,z+0);
    draw3Dline(x+i,y+i,z+i,x+i,y+0,z+i);
    draw3Dline(x+i,y+i,z+i,x+0,y+i,z+i);
}

void draw_screen(Display *dpy,Window w,GC gc) {
  int i,j,k;
  int cn=0;//camera number.
  char **files;
  static int offset=0;
  XFontStruct *font=XLoadQueryFont(dpy,"fixed");
  XCharStruct overall;
  int direction,ascent,descent;
  char coords[256];
  int x,y,z,x1,y1,x2,y2;
  XEvent e;
  XClearWindow(dpy, w);

  //struct triangle **triangle;
  //for(i=0;global.triangle[i];i++);
  //triangle=malloc(sizeof(struct triangle *) * i);
  //for(i=0;global.triangle[i];i++) {
  // triangle[i]=malloc(sizeof(struct triangle));
  // memcpy(triangle[i],global.triangle[i],sizeof(triangle));
  //}
  //triangle[i]=0;

    XDrawLine(dpy,w,gc,global.xoff,global.height/2,global.xoff+WIDTH,global.height/2);
    snprintf(coords,sizeof(coords)-1,"x: %d y: %d",global.mousex,global.mousey);
    XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
    XDrawString(dpy,w,gc,global.xoff,0+ascent,coords,strlen(coords));
    snprintf(coords,sizeof(coords)-1,"cx: %d cy: %d cz: %d",camera.x,camera.y,camera.z);
    XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
    XDrawString(dpy,w,gc,global.xoff,(descent+0+ascent)*2,coords,strlen(coords));

    snprintf(coords,sizeof(coords)-1,"xr: %d yr: %d zr: %d",camera.xr,camera.yr,camera.zr);
    XTextExtents(font,coords,strlen(coords),&direction,&ascent,&descent,&overall);
    XDrawString(dpy,w,gc,global.xoff,(descent+0+ascent)*3,coords,strlen(coords));

  j=0;
  for(i=camera.yr;i<camera.yr+270;i+=10) {
    XDrawLine(dpy,w,gc,rotateXabout(75-j,0,75-j,100,0,100,i),rotateZabout(75,0,75,100,0,100,i),100,100);
    j++;
  }

  XFlush(dpy);
}

int load_file(FILE *fp) {
 struct triangle *to;
 struct triangle t;
 char *command;
 static int i=0;//used to store the last triangle.

 for(;global.triangle[i];i++) ;//hop to the end.

 fcntl(fileno(fp),F_SETFL,O_NONBLOCK);

 for(;;i++) {
  if(feof(fp))  {
   //printf("resetting EOF\n");
   clearerr(fp);
  }
  if(fscanf(fp,"%ms %ms %d %d %d %d %d %d %d %d %d",&(t.id),&command,&(t.x1),&(t.y1),&(t.z1),&(t.x2),&(t.y2),&(t.z2),&(t.x3),&(t.y3),&(t.z3)) > 0) {
   /*if(!strcmp(command,"addsquare")) { } */
   if(!strcmp(command,"addtriangle")) {
    global.triangle[i]=malloc(sizeof(struct triangle));
    to=global.triangle[i];
    memcpy(to,&t,sizeof(t));
   }
   if(!strcmp(command,"move")) {
    for(i=0;global.triangle[i];i++) {
     if(!strcmp(global.triangle[i]->id,t.id)) {
      global.triangle[i]->x1+=t.x1;
      global.triangle[i]->y1+=t.y1;
      global.triangle[i]->z1+=t.z1;

      global.triangle[i]->x2+=t.x1;
      global.triangle[i]->y2+=t.y1;
      global.triangle[i]->z2+=t.z1;

      global.triangle[i]->x3+=t.x1;
      global.triangle[i]->y3+=t.y1;
      global.triangle[i]->z3+=t.z1;
     }
    }
   }
   global.triangle[i+1]=0;
   draw_screen(global.dpy,global.w,global.gc);
  } else {
   global.triangle[i]=0;
   break;
  }
 }
}

int export_file(FILE *fp) {
 struct triangle *to;
 int i;
 for(i=0;global.triangle[i];i++) {
  to=global.triangle[i];
  printf("%s addtriangle %d %d %d %d %d %d %d %d %d\n",to->id,to->x1,to->y1,to->z1,to->x2,to->y2,to->z2,to->x3,to->y3,to->z3);
 }
}


int main(int argc,char *argv[]) {
  char redraw=0;
  if(argc < 2) {
   fprintf(stderr,"usage: hackvr yourname < from_others > to_others\n");
   return 1;
  } else {
   global.user=strdup(argv[1]);
  }
  setbuf(stdin,0);
  setbuf(stdout,0);
  Display *dpy = XOpenDisplay(0);
  assert(dpy);
  global.dpy=dpy;
  unsigned int mask;
  int i;
  XEvent e;
  XSetWindowAttributes attributes;
  int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
  int whiteColor = //WhitePixel(dpy, DefaultScreen(dpy));
  attributes.background_pixel=blackColor;
  Window w = XCreateWindow(dpy,DefaultRootWindow(dpy),0,0,WIDTH*SPLIT_SCREEN,HEIGHT,1,DefaultDepth(dpy,DefaultScreen(dpy)),InputOutput,DefaultVisual(dpy,DefaultScreen(dpy))\
                           ,CWBackPixel, &attributes);
  global.w=w;
  global.triangle[0]=0;//we'll allocate as we need more.
  Window root,child;
  XSelectInput(dpy, w, PointerMotionMask|StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|ResizeRedirectMask|KeyPressMask);
  XMapWindow(dpy, w);
  XStoreName(dpy,w,"hackvr");
  GC gc = XCreateGC(dpy, w, 0, 0);
  global.gc=gc;
  global.color_map=DefaultColormap(dpy, DefaultScreen(dpy));
  XAllocNamedColor(dpy, global.color_map, "green", &global.green, &global.green);
  XSetForeground(dpy, gc, global.green.pixel);
//  XSetForeground(dpy, gc, whiteColor);
  for(;;) {
    load_file(stdin);
    XNextEvent(dpy, &e);
    redraw=1;
    switch(e.type) {
      case MotionNotify:
        XQueryPointer(dpy,w,&root,&child,&global.rmousex,&global.rmousey,&global.mousex,&global.mousey,&mask);
        break;
      case ButtonPress:
        global.buttonpressed=e.xbutton.button;
        break;
      case ButtonRelease:
        global.buttonpressed=0;
        break;
      case ResizeRequest:
        XGetGeometry(dpy,w,&root,&global.x,&global.y,&global.width,&global.height,&global.border_width,&global.depth);
      case KeyPress:
        switch(XLookupKeysym(&e.xkey,0)) {
          case XK_Up:
           //fix these to use calculated values down there.
           //printf("%s move 0 0 1 0 0 0 0 0 0\n",global.user);
           camera.z+=5*cosl(d2r(camera.yr));
           camera.x+=5*sinl(d2r(camera.yr)); break;
          case XK_Down:
           //printf("%s move 0 0 -1 0 0 0 0 0 0\n",global.user);
           camera.z+=5*cosl(d2r(camera.yr+180));
           camera.x+=5*sinl(d2r(camera.yr+180)); break;
          case XK_Left:
           //printf("%s move 1 0 0 0 0 0 0 0 0\n",global.user);
           camera.z+=5*cosl(d2r(camera.yr+90));
           camera.x+=5*sinl(d2r(camera.yr+90)); break;
          case XK_Right:
           //printf("%s move -1 0 0 0 0 0 0 0 0\n",global.user);
           camera.z+=5*cosl(d2r(camera.yr+270));
           camera.x+=5*sinl(d2r(camera.yr+270)); break;
          case XK_w:
           //printf("%s move 0 1 0 0 0 0 0 0 0\n",global.user);
           camera.y+=5; break;
          case XK_s:
           //printf("%s move 0 -1 0 0 0 0 0 0 0\n",global.user);
           camera.y-=5; break;
          case XK_q:
           camera.yr+=5; break;
          case XK_e:
           camera.yr-=5; break;
          case XK_Escape: return 0;
          default:
           redraw=0;
           break;
        }
      default:
        redraw=0;
        break;
    }
    //if(redraw)
    draw_screen(dpy,w,gc);
  }
  return 0;
}
