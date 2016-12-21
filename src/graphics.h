#ifndef _HACKVR_GRAPHICS_H_
#define _HACKVR_GRAPHICS_H_

#include <X11/Xlib.h>

int x11_init();
int x11_event_handler();
void draw_screen(Display *dpy,Window w,GC gc);
void set_aspect_ratio();

#endif
