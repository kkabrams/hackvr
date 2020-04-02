#ifndef _HACKVR_GRAPHICS_H_
#define _HACKVR_GRAPHICS_H_

#include <idc.h>

struct gra_global {
  unsigned int width;
  unsigned int height;
  c2_t mouse;
  int greyscale;
  int xoff;
  int split_screen;
  real split;
  int split_flip;
  char force_redraw;
  char red_and_blue;
  unsigned int depth;//???
  unsigned int border_width;
  char zsort;
  char drawminimap;
  char input_mode;
  char drawsky;
  char draw3d;
  int redraw[2];//pipe fd
  int mapxoff;
  int mapyoff;
  int mousemap[10];//you're only going to have 10 buttons on your mouse... right?
  c2_t dragstart[10];//mouse coordinates where a drag was started
  c3_rot_t oldcamera;
  int maxshapes;
  int fieldofview;
};

typedef struct {
 struct c3_shape *s;
 real d;
} zsort_t;

int graphics_init();
int graphics_sub_init();
void graphics_event_handler(struct shit *me,char *line);
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
void redraw();

#endif
