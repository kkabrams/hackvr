#include <string.h>
#include "common.h"
#include "math.h"

extern struct global global;

//might change this to use hashtables for faster lookups.
c3_group_rot_t *get_group_rotation(char *id) {
  int i;
  for(i=0;global.group_rot[i];i++) {
    if(!strcmp(global.group_rot[i]->id,id)) {
      return global.group_rot[i];
    }
  }
  return 0;//need to be sure to check return value for this function!
}

c3_t rotate_c3_xr(c3_t p1,c3_t p2,radians xr) {//rotate y and z around camera based on xr (looking up and down)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.y,p1.z},(c2_t){p2.y,p2.z},xr);
  return (c3_t){p1.x,tmp.x,tmp.y};
}
c3_t rotate_c3_yr(c3_t p1,c3_t p2,radians yr) {//rotate x and z around camera based on yr (looking left and right)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.x,p1.z},(c2_t){p2.x,p2.z},yr);
  return (c3_t){tmp.x,p1.y,tmp.y};
}
c3_t rotate_c3_zr(c3_t p1,c3_t p2,radians zr) {//rotate x and y around camera based on zr (cocking your head to a side)
  c2_t tmp;
  tmp=rotate_c2((c2_t){p1.x,p1.y},(c2_t){p2.x,p2.y},zr);
  return (c3_t){tmp.x,tmp.y,p1.z};
}

//sin(0) = 0
//sin(90deg) = 1
//cos(0) = 1
//cos(90deg) = 0
//// rotate first point about second point
c2_t rotate_c2(c2_t p1,c2_t p2,radians dr) {
  c2_t p3;
  real d=distance2(p1,p2);
  radians r=points_to_angle(p2,p1);
  r.r=r.r+dr.r;
  p3.x=(cosl(r.r) * d) + p2.x;
  p3.y=(sinl(r.r) * d) + p2.y;
  return p3;
}
real distance2(c2_t p1,c2_t p2) {
 return sqrtl(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) ));
}

degrees r2d(radians r) {
 return (degrees){(r.r * (real)180 / M_PIl) };
}

radians d2r(degrees d) {
 while(d.d<0) d.d+=360;
 return (radians){(real)(d.d%360) / (real)180 * M_PIl};
}
//the angle from the first point to the second point. not the other way around.
radians points_to_angle(c2_t p1,c2_t p2) {
  real a=atan2l(p2.y-p1.y,p2.x-p1.x);
  return (radians){a>=0?a:M_PIl+M_PIl+a};
}
