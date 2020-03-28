#include <stdio.h>
#include <string.h>

#include "common.h"
#include "math.h"

extern struct hvr_global global;

//might change this to use hashtables for faster lookups.
c3_group_rot_t *get_group_relative(char *id) {//crashes in here somehwere...
  int i;
  c3_group_rot_t *gr;
  struct entry *tmp;
  if((tmp=ht_getnode(&global.ht_group,id))) {
    gr=tmp->target;
    return gr;
  }//if this didn't work, do fallback...
  for(i=0;global.group_rot[i];i++) {
    if(!strcmp(global.group_rot[i]->id,id)) {//should I use glob here and return an array?
      if(gr != global.group_rot[i]) {
        fprintf(stderr,"# %s ? %s ? %s\n",tmp->original,gr->id,global.group_rot[i]->id);
        fprintf(stderr,"# %16x != %16x. wtf?\n",gr,global.group_rot[i]);
      }
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

void print_point(c3_t p) {
  printf("%lF %lF %lF\n",p.x,p.y,p.z);
}

c3_s_t apply_group_relative(c3_s_t s,c3_group_rot_t *group_rot) {
  c3_s_t s2;
  c3_group_rot_t *gr;
  int i;
  s2.len=s.len;
  s2.id=s.id;
  s2.attrib=s.attrib;
  if(group_rot) {
   gr=group_rot;
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
      //s2.p[i]=c3_add(gr->p,rotate_c3_yr(s.p[i],gr->p,d2r(gr->r.y)));
      s2.p[i]=c3_add(gr->p,rotate_c3_xr(
                           rotate_c3_yr(
                           rotate_c3_zr(s.p[i],(c3_t){0,0,0},d2r(gr->r.z)
                                             ),(c3_t){0,0,0},d2r(gr->r.y)
                                             ),(c3_t){0,0,0},d2r(gr->r.x)
                                       )
                    );
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
isLeft( cs_t P0, cs_t P1, cs_t P2 )
{
    return ( (P1.x - P0.x) * (P2.y - P0.y)
            - (P2.x -  P0.x) * (P1.y - P0.y) );
}
//===================================================================


// cn_PnPoly(): crossing number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  0 = outside, 1 = inside
// This code is patterned after [Franklin, 2000]
int
cn_PnPoly( cs_t P, cs_t *V, int n )
{
    int    cn = 0;    // the  crossing number counter

    // loop through all edges of the polygon
    for (int i=0; i<n; i++) {    // edge from V[i]  to V[i+1]
       if (((V[i].y <= P.y) && (V[i+1].y > P.y))     // an upward crossing
        || ((V[i].y > P.y) && (V[i+1].y <=  P.y))) { // a downward crossing
            // compute  the actual edge-ray intersect x-coordinate
            float vt = (float)(P.y  - V[i].y) / (V[i+1].y - V[i].y);
            if (P.x <  V[i].x + vt * (V[i+1].x - V[i].x)) // P.x < intersect
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
wn_PnPoly( cs_t P, cs_t *V, int n )
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
