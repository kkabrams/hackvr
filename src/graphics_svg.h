#ifndef _HACKVR_GRAPHICS_SVG_H_
#define _HACKVR_GRAPHICS_SVG_H_

struct svg_global {//stores global variables for the x11 *specific* shit.
  char backbuffer[1024 * 1024 * 1024];//1 meg should be fine?
  char *colors[256];
  char *ansi_color[16];
  char *foreground_color;
  char *filename;
  char *red;
  char *green;
  char *blue;
};

#endif
