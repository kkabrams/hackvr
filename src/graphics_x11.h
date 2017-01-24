#ifndef _HACKVR_GRAPHICS_X11_H_
#define _HACKVR_GRAPHICS_X11_H_

#include <X11/Xlib.h>

struct x11_global {//stores global variables for the x11 *specific* shit.
  XColor colors[256];
  XColor green;
  XColor red;
  XColor blue;
  XColor redblue[2];
  Colormap color_map;
  Display *dpy;
  Window w;
  Pixmap backbuffer;
  Pixmap cleanbackbuffer;
  GC gc;
  GC backgc;
  int root_window;
};

#endif
