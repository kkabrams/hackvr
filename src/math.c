#include <string.h>
#include "common.h"
#include "math.h"

extern struct hvr_global global;

//might change this to use hashtables for faster lookups.
c3_group_rot_t *get_group_relative(char *id) {//crashes in here somehwere...
  int i;
  for(i=0;global.group_rot[i];i++) {
    if(!strcmp(global.group_rot[i]->id,id)) {//should I use glob here and return an array?
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
// if you pass (1,0),(0,0),0 it should output 1,0
// if you pass (0,1),(0,0),0 it should output 0,1
c2_t rotate_c2(c2_t p1,c2_t p2,radians dr) {
  c2_t p3;
  real d=distance2(p1,p2);
  radians r=points_to_angle(p2,p1);
  r.r=r.r+dr.r;
  p3.x=(cos(r.r) * d) + p2.x;
  p3.y=(sin(r.r) * d) + p2.y;
  return p3;
}
real distance2(c2_t p1,c2_t p2) {
 return sqrt(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) ));
}

degrees r2d(radians r) {
 return (degrees){(r.r * (real)180 / M_PI) };
}

radians d2r(degrees d) {
 while(d.d<0) d.d+=360;
 return (radians){(real)(d.d%360) / (real)180 * M_PI};
}
//the angle from the first point to the second point. not the other way around.
radians points_to_angle(c2_t p1,c2_t p2) {
  real a=atan2(p2.y-p1.y,p2.x-p1.x);
  return (radians){a>=0?a:M_PI+M_PI+a};
}

int between_angles(degrees d,real lower,real upper) {
  //lower may be higher than upper.
  //because lower is < 0 which wraps to higher. lower is 270, upper is 90. 270-90 is in front.
  if(lower > upper) {
    if(d.d > lower) return 1;
    if(d.d < upper) return 1;
  }
  if(upper > lower) {
    if(d.d > lower && d.d < upper) return 1;
  }
  return 0;
}

int points_on_same_side_of_line(c2_t p1,c2_t p2,c2_t p3,c2_t p4) {//we're going to check if... well, the name says it all.
//  if(s.len != 2) //well... I guess we'll just use the first two points anyway.
  degrees la=r2d(points_to_angle(p3,p4));
  degrees p1a=r2d(points_to_angle(p3,p1));
  degrees p2a=r2d(points_to_angle(p3,p2));
  //p1aa=
  //now...
  //between angles. if the line's angle is between the points' angles they're on opposite sides
  if(between_angles(la,p1a.d,p2a.d)) {
    return 1;
  }
  return 0;
}

//if a shape's angle between points continues to grow only in one direction the shape is convex.
//like for a square the angles would be 90,90,90,90 or -90,-90,-90,-90
//but if we end up with something like 90,90,-90,90,90,90 then we have concave.
//this could be used in a loop and then cut polygons into convex before loading them into the shapes array.
//not sure how I'm going to do this for 3d shapes... not needed I guess. this is for mouse-inside-shape
//after it has already been flattened. so it can't be used before they're loaded.
//has to be done at run-time with the 2d shapes that are in either c2 or cs format.
int is_shape_concave() {
 //a;
 //b;
 //a=
 //for(i=1;i<s.len;i++) {
 // b=points_to_angle(s.p[i],s.p[(i+1)%s.len]);
 // if(b < a) //figure out which direction the points are bending and if we bend the other direction then we're concave.
 //}
 return 0;
}

int point_inside_concave_shape(c2_t p1,c2_s_t s) {//hrm...
  //the point must be on the inside side of all lines to be inside.
  //if(s.len == 1) //as long as distance from the center is less than the radius we're in.
  //else {
  int i;
  for(i=0;i<s.len;i++) {
    //if(!points_on_same_side_of_line()) {
    //  return 0;
    //}
  }
  return 1;
}
