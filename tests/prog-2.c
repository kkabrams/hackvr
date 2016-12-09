#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
  XColor green;
  Colormap color_map;
} global;

int mycmp(const void *a,const void *b) {
// printf("comparing '%s' and '%s'\n",*(char **)a,*(char **)b);
 return strcmp(*(char **)a,*(char **)b);
}

char **ls(char *dir) {
  DIR *fd;
  char **files=malloc(sizeof(char *) * 1000);
  struct dirent *d;
  int i;
  fd = opendir(".");
  if (fd) {
    for (i=0;(d = readdir(fd)) != NULL;i++) {
      files[i]=strdup(d->d_name);
    }
    files[i]=0;
    closedir(fd);
  }
  //now to sort files in numerical order.
//       void qsort(void *base, size_t nmemb, size_t size,
//                  int (*compar)(const void *, const void *));
  qsort(files,i,sizeof(char *),mycmp);
  return files;
}

void freels(char **files) {
  int i;
  for(i=0;files[i];i++) {
    free(files[i]);
  }
  free(files);
}

void draw_screen(Display *dpy,Window w,GC gc) {
  int i;
  char **files;
  static int offset=0;
  XFontStruct *font=XLoadQueryFont(dpy,"fixed");
  XCharStruct overall;
  int direction,ascent,descent;
  char coords[256];
  int x,y;
  XEvent e;
  XClearWindow(dpy, w);
  snprintf(coords,sizeof(coords)-1,"x: %d y: %d",global.mousex,global.mousey);
  files=ls(".");
  for(i=0;files[i];i++) {
    XTextExtents(font,files[i],strlen(files[i]),&direction,&ascent,&descent,&overall);
    x=30;
    y=((ascent+descent)*(i+3))+offset;
    XDrawString(dpy,w,gc,x,y,files[i],strlen(files[i]));
    if(global.mousex > x && global.mousex < x+overall.width && global.mousey > y-ascent && global.mousey < y+descent) {
     if(global.buttonpressed) {
       if(chdir(files[i])) if(!fork()) execlp("xdg-open","xdg-open",files[i],0);
       else offset=0;
     }
     //top line
     XDrawLine(dpy,w,gc,0,y-ascent,x+overall.width,y-ascent);
     //bottom line
     XDrawLine(dpy,w,gc,x,y+descent,global.width,y+descent);
     //left line
     XDrawLine(dpy,w,gc,x,y-ascent,x,y+descent);
     //right line
     XDrawLine(dpy,w,gc,x+overall.width,y-ascent,x+overall.width,y+descent);
    }
  }
  //  ||
  // \||/
  //  \/
  x=4;
  y=global.height-16;

  XDrawLine(dpy,w,gc,x+4+1,y+0+1,x+4+1,y+8+1);
  XDrawLine(dpy,w,gc,x+0+1,y+4+1,x+4+1,y+8+1);
  XDrawLine(dpy,w,gc,x+8+1,y+4+1,x+4+1,y+8+1);

  if(global.mousex > x && global.mousex < x+10 && global.mousey > y && global.mousey < y+10) {
   if(global.buttonpressed) {
    offset-=40;
   }
   XDrawLine(dpy,w,gc,x,y,x+10,y);
   XDrawLine(dpy,w,gc,x,y+10,x+10,y+10);
   XDrawLine(dpy,w,gc,x,y,x,y+10);
   XDrawLine(dpy,w,gc,x+10,y,x+10,y+10);
  }
  //  /\
  // /||\
  //  ||
  x=4;
  y=16;

  XDrawLine(dpy,w,gc,x+4+1,y+0+1,x+4+1,y+8+1);
  XDrawLine(dpy,w,gc,x+0+1,y+4+1,x+4+1,y+0+1);
  XDrawLine(dpy,w,gc,x+8+1,y+4+1,x+4+1,y+0+1);

  if(global.mousex > x && global.mousex < x+10 && global.mousey > y && global.mousey < y+10) {
   if(global.buttonpressed) {
    offset+=40;
   }
   XDrawLine(dpy,w,gc,x,y,x+10,y);
   XDrawLine(dpy,w,gc,x,y+10,x+10,y+10);
   XDrawLine(dpy,w,gc,x,y,x,y+10);
   XDrawLine(dpy,w,gc,x+10,y,x+10,y+10);
  }

  XDrawString(dpy,w,gc,0,0+ascent,coords,strlen(coords));
  XFlush(dpy);
}

int main(int argc,char *argv[])
{
  Display *dpy = XOpenDisplay(0);
  assert(dpy);
  unsigned int mask;
  XEvent e;
  XSetWindowAttributes attributes;
  int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
  int whiteColor = //WhitePixel(dpy, DefaultScreen(dpy));
  attributes.background_pixel=blackColor;
  Window w = XCreateWindow(dpy,DefaultRootWindow(dpy),0,0,500,400,1,DefaultDepth(dpy,DefaultScreen(dpy)),InputOutput,DefaultVisual(dpy,DefaultScreen(dpy))\
                           ,CWBackPixel, &attributes);
  Window root,child;
  XSelectInput(dpy, w, PointerMotionMask|StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|ResizeRedirectMask);
  XMapWindow(dpy, w);
  XStoreName(dpy,w,"hackhackhack");
  GC gc = XCreateGC(dpy, w, 0, 0);
  global.color_map=DefaultColormap(dpy, DefaultScreen(dpy));
  XAllocNamedColor(dpy, global.color_map, "green", &global.green, &global.green);
  XSetForeground(dpy, gc, global.green.pixel);
//  XSetForeground(dpy, gc, whiteColor);
  for(;;) {
    draw_screen(dpy,w,gc);
    XNextEvent(dpy, &e);
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
      default:
        break;
    }
  }
  return 0;
}
