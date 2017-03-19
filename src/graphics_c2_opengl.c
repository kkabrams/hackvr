#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include "common.h"
#include "graphics_cs.h"
#include "graphics_c2.h"
#include "graphics_c3.h"

extern struct gra_global gra_global;

GLenum doubleBuffer;
GLubyte ubImage[65536];

#define RIGHT   320.0
#define LEFT    -320.0
#define TOP     240.0
#define BOTTOM  -240.0

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

void set_aspect_ratio(int w,int h) {
  glViewport(0,0,w,h);
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();
//  glOrtho(LEFT,RIGHT,BOTTOM,TOP,0,0);
//  glTranslatef(0,-h,0);
}

void set_color_based_on_distance(real d) {
 //scale this color based on distance... closer is lighter.
// float g;
// g=d/100.0
 glColor3f(0.0, 1.0, 0.0);
}

void set_clipping_rectangle(int x,int y,int w,int h) {

}

void draw_c2_line(c2_t p1,c2_t p2) {
 //glBegin(GL_TRIANGLES);
 glBegin(GL_LINES);
 glColor3f(0.0, 1.0, 0.0);
 glVertex2i(p1.x,p1.y);
 glVertex2i(p2.x,p2.y);
 //glVertex2i(p2.x,p2.y);
 glEnd();
}

void draw_c2_text(c2_t p,char *s) {
// printf("@%lF,%lF: %s\n",p.x,p.y,s);
}

void draw_c2_shape(c2_s_t s) {
  int i;
//  printf("drawing shaep!\n");
  for(i=0;i<s.len+(s.len==1);i++) {//this shape is closed!
    draw_c2_line(s.p[i],s.p[(i+1)%(s.len+(s.len==1))]);
  }
/*  glBegin(GL_LINE_STRIP);
  glColor3f(0.0, 1.0, 0.0);
  for(i=0;i<s.len+(s.len==1);i++) {
    glVertex2i(s.p[i].x,s.p[i].y);
  }
  glEnd();
*/
}

void draw_c2_filled_shape(c2_s_t s) {
  draw_c2_shape(s);
//  glBegin(GL_POLYGON);
//  glVertex2s();
//  glEnd();
}

void set_color() {
 glColor3f(0.0, 1.0, 0.0);
}

void set_color_red() {
 glColor3f(1.0, 0.0, 0.0);
}

void set_color_blue() {
 glColor3f(0.0, 0.0, 1.0);
}

void clear_backbuffer() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
//  glFlush();
}

void flipscreen() {
//  glutSwapBuffers();
  glFlush();
}

void red_and_blue_magic() {

}

void keypress_handler(unsigned char key, int x, int y) {//x and y are mouse positions.
  switch (key) {
  case 27:
    exit(0);
  }
}

/* draw_screen is in graphics.c
void draw_screen(void)
{


}*/


void derp_screen() {
 printf("derpsceen being called.\n");
 clear_backbuffer();
 set_color();

 //this should work for draw_c2_line, not cs_line... merp.
// draw_c2_line((c2_t){LEFT,0},(c2_t){RIGHT,0}); //horizon
// draw_cs_line((cs_t){0,0},(cs_t){1,1});
// draw_cs_line((cs_t){-1,0},(cs_t){0,1});
// draw_cs_line((cs_t){0,0},(cs_t){-1,1});
 /*draw_cs_line((cs_t){10,10},(cs_t){200,210});
 draw_cs_line((cs_t){50,10},(cs_t){200,210});
 draw_cs_line((cs_t){0,60},(cs_t){200,210});*/
 flipscreen();
}

void empty() {
}

int graphics_sub_init(void)
{
//  GLenum type;

//  type = GLUT_RGB;
//  type |= GLUT_DOUBLE;
  int argc=0;
  char *argv[]={"derp",0};
  glutInit(&argc,argv);
//  glutInitDisplayMode(type);
  glutCreateWindow("hackvr opengl and glut testing");
  glutInitWindowSize(320,240);
  glScalef((1.0/320.0),(1.0/240.0),-1);//only do this once!
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();
//  gluPerspective(60.0, 1.0, 0.1, 1000.0);
//  glMatrixMode(GL_MODELVIEW);
//  glDisable(GL_DITHER);

  printf("test");

  glutKeyboardFunc(keypress_handler);
  glutDisplayFunc(draw_screen);//this is probably what should happen...
  glutReshapeFunc(set_aspect_ratio);
  return 0;//we're fine
}


int graphics_event_handler() {//return values: -1 exit, 0 don't redraw, anything else redraw
 glutMainLoopEvent();
 glutPostRedisplay();//heh. wonder if this works...
 return 1;
}

/*
void main(int argc,char *argv[]) {
  graphics_init();
  while(1) {
    graphics_event_handler();
  }
}
*/
