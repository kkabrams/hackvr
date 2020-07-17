#include <stdio.h>//for fprintf stderr. remove me later.
#include "mouse_x11.h"
#include "common.h"
#include "graphics_c3.h"
#include "graphics_c2.h"
#include "graphics_x11.h"
#include "mouse.h"

extern struct hvr_global global;
extern struct gra_global gra_global;
extern struct x11_global x11_global;

int mouse_init() {
  return x11_global.fd;
}

#define X11_MOUSE_PRIMARY 1
#define X11_MOUSE_SECONDARY 3
#define X11_MOUSE_TERTIARY 2
#define X11_MOUSE_SCROLLUP 4
#define X11_MOUSE_SCROLLDOWN 5

char x112map(char x) {
  switch(x) {
    case X11_MOUSE_PRIMARY: return MOUSE_PRIMARY;
    case X11_MOUSE_TERTIARY: return MOUSE_TERTIARY;//middle-click
    case X11_MOUSE_SECONDARY: return MOUSE_SECONDARY;
    case X11_MOUSE_SCROLLUP: return MOUSE_SCROLLUP;
    case X11_MOUSE_SCROLLDOWN: return MOUSE_SCROLLDOWN;
    default: return -1;
  }
  return -1;
}

int mouse_event_handler() {//this returns HVM_ key + for buttondown and - for buttonup... set the mousex and mousey in here?
  XEvent e;
  c3_group_rel_t *gr;
  Window root,child;//just dimmies
  unsigned int mask;//just dummies
  char motion_notify=0;
  char redrawplzkthx=0;
  int butt;
  cs_t mouse;
  cs_t rmouse;
//  char cmd[512];
  while(XCheckMaskEvent(x11_global.dpy,HV_MOUSE_X11_EVENT_MASK,&e)) {//we want to collapse mouse stuff to one for each loop.
    switch(e.type) {
      case ButtonPress: //e.xbutton.button == 1 for first button. we don't need to start at 1. let's start at 0 with the -1 //scroll wheel up is 3, down is 4
        if((butt=x112map(e.xbutton.button)) == -1) {
          continue;//we don't know how to handle this button. :/
        }
        fprintf(stderr,"# x11 button: %d is %d in hackvr\n",e.xbutton.button,butt);
        if(butt == MOUSE_SCROLLUP) {//scroll wheel up
          selfcommand("epoch move forward\n");//need to implement this syntax in hackvr
        }
        if(butt == MOUSE_SCROLLDOWN) {//scroll wheel down
          selfcommand("epoch move backward\n");
        }
        if(butt == MOUSE_TERTIARY) {//middle-click
          gra_global.input_mode ^= 1;
          printf("# gra_global.input_mode == %d\n",gra_global.input_mode);
        }
        printf("# button press %d\n",butt);
        gra_global.dragstart[butt]=gra_global.mouse;
        gra_global.mousemap[butt]=1;
        redrawplzkthx=1;
        break;
      case ButtonRelease:
        if((butt=x112map(e.xbutton.button)) == -1) {
          continue;//we don't know how to handle this button. :/ 
        }
        printf("# button release %d\n",butt);
        gra_global.mousemap[butt]=-1;//we can trigger on -1 or on 1 then set back to 0 to prevent double-trigger
        gr=get_group_relative(global.user);
        gra_global.oldcamera=gr->r;
        redrawplzkthx=1;
        break;
      case MotionNotify:
        motion_notify=1;
        redrawplzkthx=1;
        break;
      default://fuck if I know
	break;
    }
  }
  if(motion_notify) {
    //mouse.x and mouse.y are both ints now...
    //fprintf(stderr,"# mouse.x: %f mouse.y: %f\n# width: %u height: %u\n",gra_global.mouse.x,gra_global.mouse.y,gra_global.width,gra_global.height);
    XQueryPointer(x11_global.dpy,x11_global.w,&root,&child,&rmouse.x,&rmouse.y,&mouse.x,&mouse.y,&mask);
    gra_global.mouse=cs_to_c2(mouse);
    //return 1;
    //we're not going to set camera based on mousex and y here.
    //but just so I can get it out of the graphics code...
    //self-command myself to rotate myself?
    //snprintf(cmd,sizeof(cmd),"%s rotate +%d +%d +%d\n",global.user,3,11,1);
    //selfcommand(cmd);
    //global.camera.r.x.d=(gra_global.height - gra_global.mouse.y);//up and down camera controls backwards
    //fprintf(stderr,"# mouse.x: %f mouse.y: %f\n# width: %u height: %u\n",gra_glo bal.mouse.x,gra_global.mouse.y,gra_global.width,gra_global.height);
    //this /was/ using a 2 for the mousemap index... so 2 is "right" in hackvr.
    if(gra_global.mousemap[MOUSE_SECONDARY] == 1) {//if "right" click is held down
      gr=get_group_relative(global.user);
      gr->r.x.d=gra_global.oldcamera.x.d + (gra_global.mouse.y - gra_global.dragstart[2].y);
      gr->r.y.d=gra_global.oldcamera.y.d + (gra_global.mouse.x - gra_global.dragstart[2].x);
      //global.camera.r.x.d=gra_global.oldcamera.x.d + (gra_global.mouse.y - gra_global.dragstart[2].y);
      //global.camera.r.y.d=gra_global.oldcamera.y.d + (gra_global.mouse.x - gra_global.dragstart[2].x);
    }
    //do we need to redraw on mouse movement?
    //probably.
    redrawplzkthx=1;
  }
  if(redrawplzkthx) {
    redraw();
    return 1;
  }
  return 0;
}
