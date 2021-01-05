#include "common.h"
#include "math.h"
#include "graphics_cs.h"
#include "graphics_c3.h"
#include "mouse.h"

extern struct gra_global gra_global;
extern struct hvr_global global;

int c2sX(real x) { return (gra_global.width/(gra_global.split_screen / (gra_global.red_and_blue ? gra_global.split_screen: 1))) * ((x + RIGHT) / (RIGHT *2)) + gra_global.xoff; }
int s2cX(real x) { return (x/(gra_global.width/(gra_global.split_screen / (gra_global.red_and_blue?gra_global.split_screen :1))))*(RIGHT*2)-RIGHT; }

int c2sY(real y) { return gra_global.height * ((TOP-y) / (TOP*2)); }
int s2cY(real y) { return -((y/gra_global.height) * (TOP*2) - TOP); }

cs_t c2_to_cs(c2_t p) {
 return (cs_t){c2sX(p.x),c2sY(p.y)};
}
c2_t cs_to_c2(cs_t p) {
 return (c2_t){s2cX(p.x),s2cY(p.y)};
}

void draw_c2_line(c2_t p1,c2_t p2) {
  draw_cs_line(c2_to_cs(p1),c2_to_cs(p2));
}

void draw_c2_shape(c2_s_t s) {
  int i;
  cs_s_t ss;
  ss.len=s.len;
  ss.id=s.id;
  for(i=0;i<s.len+(s.len==1);i++) {
   ss.p[i]=c2_to_cs(s.p[i]);
  }
  draw_cs_shape(ss);
  if(!strcmp(global.version,"svg")) {
    return;//disable mouseover for svg
  }
//  if(cn_PnPoly(gra_global.mouse,s.p,s.len+(s.len==1))) {//if the mouse is inside the shape, we're going to draw a different outline.
  if(epoch_PnPoly(gra_global.mouse,s.p,s.len+(s.len==1))) {//if the mouse is inside the shape, we're going to draw a different outline.
    set_ansi_color(7);
    if(gra_global.mousemap[MOUSE_PRIMARY]==1) {
      printf("%s action %s\n",global.user,s.id);
      gra_global.mousemap[MOUSE_PRIMARY]=0;
    }
    //I need to debounce probably
    draw_cs_shape(ss);//draw over. dunno.
  }
}

void draw_c2_filled_shape(c2_s_t s) {
  int i;
  cs_s_t ss;
  ss.len=s.len;
  ss.id=s.id;
  for(i=0;i<s.len+(s.len==1);i++) {
   ss.p[i]=c2_to_cs(s.p[i]);
  }
  draw_cs_filled_shape(ss);
  draw_cs_shape(ss);//redraw outline last
}

void draw_c2_text(c2_t p,char *text) {
 cs_t p2=c2_to_cs(p);
 draw_cs_text(p2,text);
}
