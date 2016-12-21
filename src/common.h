#ifndef _HACKVR_COMMON_H_
#include <X11/Xlib.h>
#define _HACKVR_COMMON_H_

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

typedef double long real;

typedef struct {
 real x;
 real y;
 real z;
} c3_t;

typedef struct {
 real x;
 real y;
} c2_t;

typedef struct {
 short x;
 short y;
} cs_t;

struct camera {
  c3_t p;
  int xr;//rotations
  int yr;
  int zr;
  real zoom;
} camera;

struct c3_line {
 char *id;
 c3_t p1;
 c3_t p2;
};

struct c3_shape {//use array or linked list?
  char *id;
  unsigned char len;
  c3_t p[MAX_SIDES];
};

struct mainwin {
  int x;
  int y;
  unsigned int depth;
  int mousex;
  int mousey;
  int rmousex;
  int rmousey;
  int buttonpressed;
  unsigned int width;
  unsigned int height;
  unsigned int border_width;
  XColor colors[256];
  int xoff;
  int math_error;
  int mapxoff;
  int mapyoff;
  int split_screen;
  int split_flip;//1 or -1
  char *user;
  char greyscale;
  char headless;
  char drawminimap;//flag
  char draw3d;//flag
  char debug;//flag
  char drawsky;//flag
  char zsort;
  char red_and_blue;
  char force_redraw;
  char selected_object[256];//meh
  real mmz;
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
  struct c3_shape *shape[SHAPES];
  int shapes;
  int derp;
  int root_window;
  real split;
  int oldfps;
  int fps;
  int oldtime;
};

#endif
