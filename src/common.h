#include "config.h"
#include "math.h"

#ifndef _HACKVR_COMMON_H_
#include <X11/Xlib.h>
#define _HACKVR_COMMON_H_

#define TOP     240.0
#define BOTTOM  -240.0
#define RIGHT   320.0
#define LEFT    -320.0

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

typedef struct {
  degrees x;
  degrees y;
  degrees z;
} c3_rot_t;

typedef struct {
  char *id;
  c3_rot_t r;
  c3_t p;
} c3_group_rot_t;

typedef struct c3_line {
 char *id;
 c3_t p1;
 c3_t p2;
} cs_l_t;

struct attrib {
  char col;
  char lum;
};

typedef enum shape_flavor {POLYGON,ELLIPTIC_ARC,CUBIC_BEZIER,QUAD_BEZIER} shape_flavor;

typedef struct cs_shape {
  char *id;
  shape_flavor type;
  unsigned char len;
  cs_t p[MAX_SIDES];
  struct attrib attrib;
} cs_s_t;

typedef struct c2_shape {
  char *id;
  shape_flavor type;
  unsigned char len;
  c2_t p[MAX_SIDES];
  struct attrib attrib;
} c2_s_t;

typedef struct c3_shape {//use array or linked list?
  char *id;
  shape_flavor type;
  unsigned char len;
  c3_t p[MAX_SIDES];
  struct attrib attrib;
} c3_s_t;

struct global {
  int x;
  int y;
  char beep;//1 or 0
  int math_error;
  char *user;
  char headless;
  char debug;//flag
  char selected_object[2000];//meh
  real mmz;
  struct c3_shape *shape[SHAPES];
  int shapes;
  c3_group_rot_t *group_rot[SHAPES];//there can be less of these.
  c3_group_rot_t camera;
  real zoom;
  int derp;
  real split;
};

#endif
