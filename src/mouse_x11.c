#include "mouse_x11.h"
#include "common.h"
#include "graphics_c3.h"
#include "graphics_c2.h"
#include "graphics_x11.h"

extern struct global global;
extern struct gra_global gra_global;
extern struct x11_global x11_global;

int mouse_init() {
  return x11_global.fd;
}

int mouse_event_handler() {//this returns HVM_ key + for buttondown and - for buttonup... set the mousex and mousey in here?
  XEvent e;
  Window root,child;//just dimmies
  unsigned int mask;//just dummies
  char motion_notify=0;
  char redrawplzkthx=0;
  cs_t mouse;
  cs_t rmouse;
  while(XCheckMaskEvent(x11_global.dpy,HV_MOUSE_X11_EVENT_MASK,&e)) {//we want to collapse mouse stuff to one for each loop.
    switch(e.type) {
      case ButtonPress: //e.xbutton.button == 1 for first button. we don't need to start at 1. let's start at 0 with the -1
        gra_global.mousemap[e.xbutton.button-1]=1;
        redrawplzkthx=1;
        break;
      case ButtonRelease:
        gra_global.mousemap[e.xbutton.button-1]=-1;//we can trigger on -1 or on 1 then set back to 0 to prevent double-trigger
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
    //mouse.x and mouse.y are shorts. this function expects ints. why are these shorts?
    XQueryPointer(x11_global.dpy,x11_global.w,&root,&child,&rmouse.x,&rmouse.y,&mouse.x,&mouse.y,&mask);
    gra_global.mouse=cs_to_c2(mouse);
    //return 1;
    //we're not going to set camera based on mousex and y here.
    //but just so I can get it out of the graphics code...
    //global.camera.r.x.d=((gra_global.height/2) - gra_global.mouse.y);
    //global.camera.r.y.d=(gra_global.mouse.x - (gra_global.width/2));
  }
  if(redrawplzkthx) {
    redraw();
    return 1;
  }
  return 0;
}
