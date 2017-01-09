#ifndef _HACKVR_MATH_H_
#define _HACKVR_MATH_H_

#include "common.h" //should define c3_t

#define __USE_GNU
#include <math.h>
#undef __USE_GNU

real distance2(c2_t p1,c2_t p2);
c2_t rotate_c2(c2_t p1,c2_t p2,real dr);
c3_t rotate_c3_xr(c3_t p1,c3_t p2,real xr);
c3_t rotate_c3_yr(c3_t p1,c3_t p2,real yr);
c3_t rotate_c3_zr(c3_t p1,c3_t p2,real zr);
real d2r(int d);
real points_to_angle(c2_t p1,c2_t p2);

#endif
