
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

GLenum doubleBuffer;
GLubyte ubImage[65536];

void draw_cs_line(cs_t p1,cs_t p2) {
 
}

void draw_cs_text() {

}

void draw_c3_shape() {
 //oh lawdy... this should be draw_cs_shape...
}

static void
graphics_init(void)
{
  GLenum type;
  int j;
  GLubyte *img;
  GLsizei imgWidth = 128;

  type = GLUT_RGB;
  type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
  glutInitDisplayMode(type);
  glutCreateWindow("ABGR extension");
  if (!glutExtensionSupported("GL_EXT_abgr")) {
    printf("Couldn't find abgr extension.\n");
    exit(0);
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, 1.0, 0.1, 1000.0);
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_DITHER);

  glutKeyboardFunc(keypress_handler);
  glutDisplayFunc(draw_screen);

}

/* ARGSUSED1 */
static void
keypress_handler(unsigned char key, int x, int y)
{
  switch (key) {
  case 27:
    exit(0);
  }
}

static void draw_screen(void)
{

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);


  glRasterPos3f(0.2, -0.8, -1.5);
  glDrawPixels(128, 128, GL_RGBA, GL_UNSIGNED_BYTE, ubImage);

  if (doubleBuffer) {
    glutSwapBuffers();
  } else {
    glFlush();
  }
}

static void
Args(int argc, char **argv)
{
  GLint i;

  doubleBuffer = GL_TRUE;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-sb") == 0) {
      doubleBuffer = GL_FALSE;
    } else if (strcmp(argv[i], "-db") == 0) {
      doubleBuffer = GL_TRUE;
    }
  }
}

void graphics_event_handler() {
  glutMainLoopUpdate();
  return 0;             /* ANSI C requires main to return int. */
}
