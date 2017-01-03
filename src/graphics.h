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
  int split;
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
};

typedef struct {
 struct c3_shape *s;
 real d;
} zsort_t;

int graphics_init();
int graphics_event_handler();
void draw_screen();
void set_aspect_ratio();
int selfcommand(char *s);
cs_t c3_to_cs(c3_t p);
real d2r(int d);
real shitdist(struct c3_shape s,c3_t p);


int compar(zsort_t *a,zsort_t *b);

#endif
