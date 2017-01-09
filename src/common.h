#include "config.h"

#ifndef _HACKVR_COMMON_H_
#include <X11/Xlib.h>
#define _HACKVR_COMMON_H_

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

typedef double long real;

typedef struct {
 real r;
} radians;

typedef struct {
 int d;
} degrees;

typedef struct {
 real x;
 real y;
 real z;
} c3_t;

typedef struct {
 real x;
 real y;
} c2_t;

typedef struct {
 short x;
 short y;
} cs_t;

struct camera {
  c3_t p;
  degrees xr;//rotations
  degrees yr;
  degrees zr;
  real zoom;
} camera;

typedef struct c3_line {
 char *id;
 c3_t p1;
 c3_t p2;
} cs_l_t;

typedef struct cs_shape {
  char *id;
  unsigned char len;
  cs_t p[MAX_SIDES];
} cs_s_t;

typedef struct c2_shape {
  char *id;
  unsigned char len;
  c2_t p[MAX_SIDES];
} c2_s_t;

typedef struct c3_shape {//use array or linked list?
  char *id;
  unsigned char len;
  c3_t p[MAX_SIDES];
} c3_s_t;

struct global {
  int x;
  int y;
  int math_error;
  char *user;
  char headless;
  char debug;//flag
  char selected_object[256];//meh
  real mmz;
  struct c3_shape *shape[SHAPES];
  int shapes;
  int derp;
  real split;
};

#endif
