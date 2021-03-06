#ifndef _HACKVR_GRAPHICS_CS_H_
#define _HACKVR_GRAPHICS_CS_H_

#include "math.h"
#include <idc.h>

void draw_cs_line(cs_t p1,cs_t p2);
void draw_cs_text(cs_t p,char *text);
void draw_cs_shape(cs_s_t s);
void draw_cs_filled_shape(cs_s_t s);
void set_aspect_ratio();
void set_color_based_on_distance(real);
void set_luminosity_color(int);
void flipscreen();
void set_ansi_color(int);
void set_color();
void set_color_red();
void set_color_blue();
void clear_backbuffer();
void keypress_handler(unsigned char sym,int x,int y);
int graphics_init();
void graphics_event_handler(struct shit *me,char *line);
void set_clipping_rectangle(int x,int y,int width,int height);
void red_and_blue_magic();
void draw_mode_and();
void draw_mode_set();
void draw_mode_or();
void draw_mode_copy();

#endif
