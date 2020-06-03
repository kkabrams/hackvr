#define _XOPEN_SOURCE
#include <string.h>
#include "config.h"
#include "math.h"

#ifndef _HACKVR_COMMON_H_
#define _HACKVR_COMMON_H_

#include <hashtable.h>

#define TOP     240.0
#define BOTTOM  -240.0
#define RIGHT   320.0
#define LEFT    -320.0

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

typedef double real;

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

typedef struct {//x11 wants coords to be integers. let's make this that.
 int x;
 int y;
} cs_t;

typedef struct {
  degrees x;
  degrees y;
  degrees z;
} c3_rot_t;

typedef struct {
  char *id;//for the camera this is $USER, right?
  char *parent;//the position is relative to the parent's data.
  c3_rot_t r;//rotation
  c3_t p;//position
  c3_t s;//??? shape??? I dunno. really. wtf was this for? PROBABLY SCALE. DUH. that's what it is going to bed used for now anyway.
  c3_t v;//velocity
} c3_group_rel_t;

//typedef struct c3_line {//is this even used? I think I just use c3_s_t with 2 points.
// char *id;
// c3_t p1;
// c3_t p2;
//} cs_l_t;

struct attrib {
  char col;//color. not sure how I plan on using this.
  char lum;//brightness. 1 - 200 atm because X11 has grey1 - grey200
};

//not used yet. will be later I guess. if I ever get around to doing bezier curves.
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
  c3_t v;//velocities.
  struct attrib attrib;
} c3_s_t;

struct hvr_global {
  int x;
  int y;
  c3_rot_t old_r;
  c3_t old_p;
  char beep;//1 or 0
  int math_error;
  char *user;
  char headless;
  char debug;//flag
  char selected_object[2000];//meh
  int periodic_output;//if false, we output commands from keypresses as they're being sent to the command interpreter. if true we output every value amount of time. miliseconds probably.
  real mmz;
  int lps;//loops per second. same as frame per second but also works for headless.
  struct c3_shape *shape[SHAPES];
  int shapes;
  struct hashtable ht_group;
  c3_group_rel_t eye[MAX_SIDES];//lol. 1000 eyes! array of group_rels for each eye. how to arrange eyes?
  c3_group_rel_t camera;//should there be an array for this? camera has .s which is a shape struct. each point is the eye?
  real zoom;
  int derp;
  real split;
  char localecho;
  int selfpipe[2];
};

int selfcommand(char *);
void set_title(char *);//fuck if I know where the right spot to put this is.

#endif
