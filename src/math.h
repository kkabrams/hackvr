#ifndef _HACKVR_MATH_H_
#define _HACKVR_MATH_H_

#include "common.h" //should define c3_t

#define __USE_GNU
#include <math.h>
#undef __USE_GNU

real distance2(c2_t p1,c2_t p2);
c2_t rotate_c2(c2_t p1,c2_t p2,radians dr);
c3_t rotate_c3_xr(c3_t p1,c3_t p2,radians xr);
c3_t rotate_c3_yr(c3_t p1,c3_t p2,radians yr);
c3_t rotate_c3_zr(c3_t p1,c3_t p2,radians zr);
radians d2r(degrees d);
radians points_to_angle(c2_t p1,c2_t p2);

#endif
