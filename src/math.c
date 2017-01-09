#include "math.h"

c3_t rotate_c3_xr(c3_t p1,c3_t p2,real xr) {//rotate y and z around camera based on xr (looking up and down)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.y,p1.z},(c2_t){p2.y,p2.z},xr);
  return (c3_t){p1.x,tmp.x,tmp.y};
}
c3_t rotate_c3_yr(c3_t p1,c3_t p2,real yr) {//rotate x and z around camera based on yr (looking left and right)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.x,p1.z},(c2_t){p2.x,p2.z},yr);
  return (c3_t){tmp.x,p1.y,tmp.y};
}
c3_t rotate_c3_zr(c3_t p1,c3_t p2,real zr) {//rotate x and y around camera based on zr (cocking your head to a side)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.x,p1.y},(c2_t){p2.x,p2.y},zr);
  return (c3_t){tmp.x,tmp.y,p1.z};
}

c2_t rotate_c2(c2_t p1,c2_t p2,real dr) {//dr is in radians
  c2_t p3;
  real d=distance2(p1,p2);
  real r=points_to_angle(p1,p2);
  r=r+dr;
  p3.x=(sinl(r) * d) + p2.x;
  p3.y=(cosl(r) * d) + p2.y;
  return p3;
}
real distance2(c2_t p1,c2_t p2) {
 return sqrtl(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) ));
}
real d2r(int d) {
 while(d<0) d+=360;
 return (real)(d%360) / (real)180 * M_PIl;
}
real points_to_angle(c2_t p1,c2_t p2) {
  real a=atan2l(p2.y-p1.y,p2.x-p1.x);
  return a>=0?a:M_PIl+M_PIl+a;
}
