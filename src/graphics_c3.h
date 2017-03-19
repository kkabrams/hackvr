#ifndef _HACKVR_GRAPHICS_H_
#define _HACKVR_GRAPHICS_H_

//#include <X11/Xlib.h>//this should be in the x11 one, not this one.

struct gra_global {
  unsigned int width;
  unsigned int height;
  int mousex;
  int mousey;
  int greyscale;
  int xoff;
  int split_screen;
  real split;
  int split_flip;
  char force_redraw;
  char red_and_blue;
  unsigned int depth;//???
  unsigned int border_width;
  int oldtime;
  int fps;
  int oldfps;
  char zsort;
  char drawminimap;
  char drawsky;
  char draw3d;
  int mapxoff;
  int mapyoff;
  int rmousex;
  int rmousey;
  int buttonpressed;
  int maxshapes;
};

typedef struct {
 struct c3_shape *s;
 real d;
} zsort_t;

int graphics_init();
int graphics_sub_init();
int graphics_event_handler();
void draw_screen();
void set_aspect_ratio();
int selfcommand(char *s);
cs_t c3_to_cs(c3_t p);
radians d2r(degrees d);
real shitdist(struct c3_shape *s,c3_t p);
c3_t rotate_c3_xr(c3_t p1,c3_t p2,radians xr);
c3_t rotate_c3_yr(c3_t p1,c3_t p2,radians yr);
c3_t rotate_c3_zr(c3_t p1,c3_t p2,radians zr);

int compar(zsort_t *a,zsort_t *b);

#endif
