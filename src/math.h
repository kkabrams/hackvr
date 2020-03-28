#ifndef _HACKVR_MATH_H_
#define _HACKVR_MATH_H_

#include "common.h" //should define c3_t

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

real distance2(c2_t p1,c2_t p2);
c2_t rotate_c2(c2_t p1,c2_t p2,radians dr);
c3_t rotate_c3_xr(c3_t p1,c3_t p2,radians xr);
c3_t rotate_c3_yr(c3_t p1,c3_t p2,radians yr);
c3_t rotate_c3_zr(c3_t p1,c3_t p2,radians zr);
c3_group_rot_t *get_group_relative(char *id);
radians d2r(degrees d);
degrees r2d(radians r);
radians points_to_angle(c2_t p1,c2_t p2);
int between_angles(degrees d,real lower,real upper);
c3_s_t apply_group_relative(c3_s_t s,c3_group_rot_t *group_rot);
c3_t c3_add(c3_t p1,c3_t p2);
real distance3(c3_t p1,c3_t p2);
void print_point(c3_t p);

int isLeft(cs_t, cs_t, cs_t);
int cn_PnPoly(cs_t, cs_t *, int);
int wn_PnPoly(cs_t, cs_t *, int);

#endif
