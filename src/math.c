#define _POSIX_C_SOURCE 200809L //for strdup
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "math.h"

extern struct hvr_global global;

//ONE OF THESE DAYS I NEED TO RENAME ALL THE GROUP_ROT SHIT TO GROUP_REL since it doesn't just have rotations anymore.

//this function needs to create a group_relative if one doesn't already exist.
c3_group_rel_t *get_group_relative(char *id) {
  if(!id) return 0;//null argument gets null return
  c3_group_rel_t *gr;
  if((gr=ht_getvalue(&global.ht_group,id))) {
    if(gr) return gr;
  }
  //if we got here, we need to make a new one.
  gr=malloc(sizeof(c3_group_rel_t));//just exit on malloc error? sure...
  assert(gr && "failed to malloc something I don't want to figure out how to work around if it doesn't malloc.");
  memset(gr,0,sizeof(c3_group_rel_t));
  gr->s=(c3_t){1,1,1};//scale needs to be all 1s, since it is multiplied against the coordinates.
  gr->id=strdup(id);//don't forget to free this when it gets deleted.
  if(ht_setkey(&global.ht_group,id,gr)) {
    abort();
  }
  return gr;
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

void print_point(c3_t p) {
  printf("%lF %lF %lF\n",p.x,p.y,p.z);
}

c3_t c3_multiply(c3_t p,c3_t s) {
  c3_t r;
  r.x = p.x * s.x;
  r.y = p.y * s.y;
  r.z = p.z * s.z;
  return r;
}

// hand pieces will first rotate around 0,0,0.
// arm pieces will first rotate around 0,0,0.
// torso pieces will first rotate around 0,0,0.
// arm pieces will be added to torso rotations
// hand pieces will be addd to arm rotations.
// arm pieces will rotate around origin...
// I dunno....
c3_t point_apply_group_relative(c3_t p,c3_group_rel_t *gr) {
  if(!gr) { return p; }
  return c3_add(point_apply_group_relative(gr->p,get_group_relative(gr->parent)),
                rotate_c3_xr(
                  rotate_c3_yr(
                    rotate_c3_zr(
                      c3_multiply(p,gr->s),(c3_t){0,0,0},d2r(gr->r.z)
                    ),(c3_t){0,0,0},d2r(gr->r.y)
                  ),(c3_t){0,0,0},d2r(gr->r.x)
                )
               );
}

c3_s_t apply_group_relative(c3_s_t s,c3_group_rel_t *group_rel) {
  c3_s_t s2;
  c3_group_rel_t *gr;
  int i;
  s2.len=s.len;
  s2.id=s.id;
  s2.attrib=s.attrib;
  if(group_rel) {
   gr=group_rel;
  } else {
   gr=get_group_relative(s.id);
  }
  if(!gr) { //shit. we have a shape but not relatives for it. oh well.
    //printf("# no group relative.\n");
    return s;
  }
  if(s.len < 1) {//wtf is wrong with this shape?
    //printf("# wtf?\n");
    return s;
  }
  if(s.len == 1) {//we're a circle. we /really/ have two points stored though.
    real dist=distance3(s.p[0],s.p[1]);
    s2.p[0]=s.p[0];
    s2.p[1]=c3_add(s.p[0],(c3_t){dist,0,0});
  }
  if(s.len > 1) {//we're a polygon.
   for(i=0;i<s.len+(s.len==1);i++) {//apply the group's rotation and store in s2.
    if(!strcmp(s.id,global.user)) {//we need to rotate camera objects (an avatar maybe) only along the y axis, and in the opposite direction than everything else rotates
     s2.p[i]=c3_add(gr->p,rotate_c3_yr(s.p[i],(c3_t){0,0,0},d2r((degrees){0-(gr->r.y.d)})));
    } else {
     if(gr) {
      s2.p[i]=point_apply_group_relative(s.p[i],gr);
     } else {
      s2.p[i]=s.p[i];
     }
    }
   }
  }
  return s2;
}

c3_t c3_add(c3_t p1,c3_t p2) {
 return (c3_t){p1.x+p2.x,p1.y+p2.y,p1.z+p2.z};
}

//this could probably be a macro, or inline or whatever. and also work for cs_t
c2_t c2_add(c2_t p1,c2_t p2) {
 return (c2_t){p1.x+p2.x,p1.y+p2.y};
}

real distance3(c3_t p1,c3_t p2) {
 return sqrt(( (p1.x-p2.x)*(p1.x-p2.x) )+( (p1.y-p2.y)*(p1.y-p2.y) )+( (p1.z-p2.z)*(p1.z-p2.z) ));
}

/* epoch copied this from: http://geomalgorithms.com/a03-_inclusion.html */
/* then edited it to work with hackvr ofc */

// Copyright 2000 softSurfer, 2012 Dan Sunday
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
 

// a Point is defined by its coordinates {int x, y;}
//===================================================================
 

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
//    See: Algorithm 1 "Area of Triangles and Polygons"
inline int
isLeft( c2_t P0, c2_t P1, c2_t P2 )
{
    return ( (P1.x - P0.x) * (P2.y - P0.y)
            - (P2.x -  P0.x) * (P1.y - P0.y) );
}
//===================================================================


// cn_PnPoly(): crossing number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
// hacked on by epoch to make the previous statement automatic. i+1 changed to i+1%n
//      Return:  0 = outside, 1 = inside
// This code is patterned after [Franklin, 2000]

//pretending like this will work fine with a real instead of an int... probably fine.
int
cn_PnPoly( c2_t P, c2_t *V, int n )
{
    int    cn = 0;    // the  crossing number counter

    // loop through all edges of the polygon
    for (int i=0; i<n; i++) {    // edge from V[i]  to V[i+1]
       if (((V[i].y <= P.y) && (V[i+1%n].y > P.y))     // an upward crossing
        || ((V[i].y > P.y) && (V[i+1%n].y <=  P.y))) { // a downward crossing
            // compute  the actual edge-ray intersect x-coordinate
            float vt = (float)(P.y  - V[i].y) / (V[i+1%n].y - V[i].y);
            if (P.x <  V[i].x + vt * (V[i+1%n].x - V[i].x)) // P.x < intersect
                 ++cn;   // a valid crossing of y=P.y right of P.x
        }
    }
    return (cn&1);    // 0 if even (out), and 1 if  odd (in)

}
//===================================================================


// wn_PnPoly(): winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  wn = the winding number (=0 only when P is outside)

int
wn_PnPoly( c2_t P, c2_t *V, int n )
{
    int    wn = 0;    // the  winding number counter

    // loop through all edges of the polygon
    for (int i=0; i<(n-1); i++) {   // edge from V[i] to  V[i+1]
        if (V[i].y <= P.y) {          // start y <= P.y
            if (V[i+1].y  > P.y)      // an upward crossing
                 if (isLeft( V[i], V[i+1], P) > 0)  // P left of  edge
                     ++wn;            // have  a valid up intersect
        }
        else {                        // start y > P.y (no test needed)
            if (V[i+1].y  <= P.y)     // a downward crossing
                 if (isLeft( V[i], V[i+1], P) < 0)  // P right of  edge
                     --wn;            // have  a valid down intersect
        }
    }
    return wn;
}
//===================================================================

int epoch_PnPoly( c2_t P, c2_t *V, int n ) {
  cs_t min,max;
  int i;
  min.x=V[0].x;
  min.y=V[0].y;
  max.x=V[0].x;
  max.y=V[0].y;
  for(i=0;i<n;i++) {
    min.x=min(min.x,V[i].x);
    max.x=max(max.x,V[i].x);
    min.y=min(min.y,V[i].y);
    max.y=max(max.y,V[i].y);
  }
  return (P.x <= max.x && P.x >= min.x && P.y >= min.y && P.y <= max.y);
}
