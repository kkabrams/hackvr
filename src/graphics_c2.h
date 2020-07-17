#ifndef _HACKVR_GRAPHICS_C2_H_
#define _HACKVR_GRAPHICS_C2_H_

#include "math.h"

#define TOP     240.0
#define BOTTOM  -240.0
#define RIGHT   320.0
#define LEFT    -320.0

int c2sX(real x);
int s2cX(real x);
int c2sY(real y);
int s2cY(real y);
cs_t c2_to_cs(c2_t p);
c2_t cs_to_c2(cs_t p);
void draw_c2_line(c2_t p1,c2_t p2);
void draw_c2_shape(c2_s_t s);
void draw_c2_filled_shape(c2_s_t s);
void draw_c2_text(c2_t p,char *text);

#endif
