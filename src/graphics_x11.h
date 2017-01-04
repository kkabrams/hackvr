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

void draw_cs_line(cs_t p1,cs_t p2);
void draw_cs_text(cs_t p,char *text);
void draw_cs_shape(cs_s_t s);
void draw_cs_filled_shape(cs_s_t s);
void set_aspect_ratio();
void set_color_based_on_distance(real d);
void flipscreen();
void set_color();
void clear_backbuffer();
int keypress_handler(int sym);
int graphics_init();
int graphics_event_handler();
void set_clipping_rectangle(int x,int y,int width,int height);
void red_and_blue_magic();

#endif
