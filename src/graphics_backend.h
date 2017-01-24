#ifndef _HACKVR_GRAPHICS_BACKEND_H_
#define _HACKVR_GRAPHICS_BACKEND_H_

//these are just the functions that all backends need to implement to work with hackvr.
//the list of functions are subject to change.

void draw_cs_line(cs_t p1,cs_t p2);
void draw_cs_text(cs_t p,char *text);
void draw_cs_shape(cs_s_t s);
void draw_cs_filled_shape(cs_s_t s);
void set_aspect_ratio();
void set_color_based_on_distance(real d);
void flipscreen();
void set_color();
void set_color_red();
void set_color_blue();
void clear_backbuffer();
int keypress_handler(int sym);
int graphics_init();
int graphics_event_handler();
void set_clipping_rectangle(int x,int y,int width,int height);
void red_and_blue_magic();

#endif
