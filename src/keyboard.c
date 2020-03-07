#include <stdio.h>
#include "keyboard.h" //HVK values
#include "common.h"

extern struct hvr_global global;

#include "graphics_c3.h"
extern struct gra_global gra_global;

char keyboard_map[1024]={0};

//return 0 is all is fine, -1 if we need to exit
void keyboard_event_handler() {
  int k;
  char line[2560];
  c3_group_rot_t *gr;
  int e;//it is HVK_ values. positive or negative for press and release. 0 for nothing.
  while((e=get_keyboard_event())) {
    k=(e<0?-e:e);//absolute value
    //fprintf(stderr,"# key: %d\n",k);
    if(e < 0) { //key release
      keyboard_map[k]=-1;
      //update keyboard state map?
      //let's do stuff for each key released?
    }
    else { //key is now held down
      keyboard_map[k]=1;
      //update keyboard state map?
      //fire what happens when the key is pressed down
    }
  }
  if(keyboard_map[HVK_ENTER]==1) {
    keyboard_map[HVK_ENTER]=0;//don't repeat this.
    snprintf(line,sizeof(line)-1,"%s action %s\n",global.user,global.selected_object);
    selfcommand(line);
  }
  if(keyboard_map[HVK_JUMP]==1) {//if we hold down jump we don't jump higher. or more. or again. just once.
    keyboard_map[HVK_JUMP]=0;
    fprintf(stderr,"# JUMP!\n");
    //jump uses velocity.
    gr=get_group_relative(global.camera.id);//should we make the currently selected object jump? does the camera id follow the controlled object?
    if(gr) gr->v.y=-5;
    else fprintf(stderr,"# camera doesn't have a group relative so it can't jump!\n");
  }

//// keys that keep doing stuff if they're held down.
  if(keyboard_map[HVK_FORWARD]==1) {//do velocity? maybe...
    keyboard_map[HVK_FORWARD]=0;
    snprintf(line,sizeof(line)-1,"%s move forward\n",global.user);
    selfcommand(line);//moving forward at a speed based on the framerate... :/
  }
  if(keyboard_map[HVK_BACKWARD]==1) {
    keyboard_map[HVK_BACKWARD]=0;
    snprintf(line,sizeof(line)-1,"%s move backward\n",global.user);
    selfcommand(line);
  }
  if(keyboard_map[HVK_LEFT]==1) {
    keyboard_map[HVK_LEFT]=0;
    snprintf(line,sizeof(line)-1,"%s move left\n",global.user);
    selfcommand(line);
  }
  if(keyboard_map[HVK_RIGHT]==1) {
    keyboard_map[HVK_RIGHT]=0;
    snprintf(line,sizeof(line)-1,"%s move right\n",global.user);
    selfcommand(line);
  }
  if(keyboard_map[HVK_UP]==1) {
    keyboard_map[HVK_UP]=0;
    snprintf(line,sizeof(line)-1,"%s move up\n",global.user);
    selfcommand(line);
  }
  if(keyboard_map[HVK_DOWN]==1) {
    keyboard_map[HVK_DOWN]=0;
    snprintf(line,sizeof(line)-1,"%s move down\n",global.user);
    selfcommand(line);
  }
  //don't forget to add the rotation keys.
  if(keyboard_map[HVK_TURNLEFT] == 1) {
    keyboard_map[HVK_TURNLEFT]=0;
    snprintf(line,sizeof(line)-1,"%s rotate +0 +-%d +0\n",global.user,5);
    selfcommand(line);
  }
  if(keyboard_map[HVK_TURNRIGHT] == 1) {
    keyboard_map[HVK_TURNRIGHT]=0;
    snprintf(line,sizeof(line)-1,"%s rotate +0 +%d +0\n",global.user,5);
    selfcommand(line);
  }
  // ^ here
  if(keyboard_map[HVK_DEBUG]==1) {
    keyboard_map[HVK_DEBUG]=0;//make sure we don't hold-down toggle it. just on press.
    global.debug ^= 1;
  }
  if(keyboard_map[HVK_ESCAPE]==1) {
    keyboard_map[HVK_ESCAPE]=0;
    snprintf(line,sizeof(line)-1,"%s quit\n",global.user);
    selfcommand(line);
  }
}
