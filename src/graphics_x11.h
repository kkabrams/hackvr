#ifndef _HACKVR_GRAPHICS_X11_H_
#define _HACKVR_GRAPHICS_X11_H_

#include <X11/Xlib.h>

#define HV_GRAPHICS_X11_EVENT_MASK StructureNotifyMask|ExposureMask|VisibilityChangeMask|FocusChangeMask

struct x11_global {//stores global variables for the x11 *specific* shit.
  XColor colors[256];
  XColor ansi_color[16];
  XColor green;
  XColor red;
  XColor blue;
  XColor redblue[2];
  Colormap color_map;
  Display *dpy;
  int fd;
  Window w;
  Pixmap backbuffer;
  Pixmap cleanbackbuffer;
  GC gc;
  GC backgc;
  char snow;
  cs_t rootmouse;
  int root_window;
};

#endif
