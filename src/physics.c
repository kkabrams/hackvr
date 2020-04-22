#include "common.h"

extern struct hvr_global global;
extern struct gra_global gra_global;

#define MINIMUM_Y 10
#define GRAVITY 9.8

//collision detection and gravity
//I need to get a sphere of each object then compare distances and radii.
//maye let each object have a collision box type. either rectangular prism or sphere.
//sphere would be fine for players.

/// VELOCITIES:
/*
foward/backward, left/right, up/down, x, y, z
rotation-relative-x,rry,rrz
*/
int apply_physics() {
  //int i;
  //we can just assume things will float if they don't have a group_rot
  /*
  for(i=0;global.group_rot[i]  && i < MAXSHAPES;i++) {//this should be applied to group_rots
    if(!strcmp(global.group_rot[i]->id,global.user)) {//only apply gravity to the camera.
      global.group_rot[i]->v.y += (GRAVITY / (float)(global.lps?global.lps:1)); //heh. "fps" needs a headless equivalent now.
      global.group_rot[i]->p.y -= global.group_rot[i]->v.y;
      if(global.group_rot[i]->p.y < MINIMUM_Y) {//we've moved so we need to output a move command?
        global.group_rot[i]->v.y=0;
        global.group_rot[i]->p.y=MINIMUM_Y;
      }
    }
  }*/
  //lol. not right now.
  return 1;
}
