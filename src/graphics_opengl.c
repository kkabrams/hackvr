#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include "common.h"

GLenum doubleBuffer;
GLubyte ubImage[65536];

void set_aspect_ratio(int w,int h) {
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,w,0,h,-1,1);
//  glScalef(1,-1,1);
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

void draw_cs_line(cs_t p1,cs_t p2) {
 glBegin(GL_LINES);
 glColor3f(0.0, 1.0, 0.0);
 glVertex2i(p1.x,p1.y);
 glVertex2i(p2.x,p2.y);
 glEnd();
}

void draw_cs_text() {
 
}

void draw_cs_shape(cs_s_t s) {
  int i;
  for(i=0;i<s.len+(s.len==1);i++) {//this shape is closed!
    draw_cs_line(s.p[i],s.p[(i+1)%(s.len+(s.len==1))]);
  }
/*  glBegin(GL_LINE_STRIP);
  glColor3f(0.0, 1.0, 0.0);
  for(i=0;i<s.len+(s.len==1);i++) {
    glVertex2i(s.p[i].x,s.p[i].y);
  }
  glEnd();
*/
}

void draw_cs_filled_shape(cs_s_t s) {
  draw_cs_shape(s);
//  glBegin(GL_POLYGON);
//  glVertex2s();
//  glEnd();
}

void set_color() {
 glColor3f(0.0, 1.0, 0.0);
}

void clear_backbuffer() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void flipscreen() {
//  glutSwapBuffers();
  glFlush();
}

void red_and_blue_magic() {

}

void keypress_handler(unsigned char key, int x, int y) {
  switch (key) {
  case 27:
    exit(0);
  }
}

/* draw_screen is in graphics.c
void draw_screen(void)
{


}*/


/*void derp_screen() {
  glClear(GL_COLOR_BUFFER_BIT);

  draw_cs_line((cs_t){0,10},(cs_t){200,210});

// clear_backbuffer();
// set_color();
 flipscreen();
}*/

void graphics_init(void)
{
//  GLenum type;

//  type = GLUT_RGB;
//  type |= GLUT_DOUBLE;
  int argc=0;
  char *argv[]={"derp",0};
  glutInit(&argc,argv);
//  glutInitDisplayMode(type);
  glutCreateWindow("hackvr opengl and glut testing");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, 1.0, 0.1, 1000.0);
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_DITHER);

  glutKeyboardFunc(keypress_handler);
//  glutDisplayFunc(derp_screen);
  glutReshapeFunc(set_aspect_ratio);
}


void graphics_event_handler() {
  glutMainLoopEvent();
}

/*
void main(int argc,char *argv[]) {
  graphics_init();
  while(1) {
    graphics_event_handler();
  }
}
*/
