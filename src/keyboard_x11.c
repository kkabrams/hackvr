//so... this gets called... where? input.c????
#include <stdio.h>
#include "common.h"
#include <X11/keysym.h>
#include <X11/Xutil.h>//XLookupString()
#include "input.h" //we need to call the function that does stuff with the keys?
#include "keyboard.h"
#include "keyboard_x11.h" //for event mask and key definitions

extern struct hvr_global global;

#include "graphics_c3.h"
extern struct gra_global gra_global;

#include "graphics_x11.h"
extern struct x11_global x11_global;

int keyboard_init() {//return the fd that events should be read from.
  return x11_global.fd;//good for now?
}

//I need some xkey -> HVK conversion
hvk_t x11_keypress_handler(XKeyEvent *);
hvk_t x11_passthru(XKeyEvent *);

hvk_t get_keyboard_event() {//this returns a HVK_ key + for keydown and - for keyup?
  XEvent e;
  if(XCheckMaskEvent(x11_global.dpy,HV_X11_KB_EVENT_MASK,&e)) {
    switch(e.type) {
      case KeyPress:
        if(gra_global.input_mode == 0) return x11_keypress_handler(&e.xkey);
        else return x11_passthru(&e.xkey);
      case KeyRelease:
        if(gra_global.input_mode == 0) return -x11_keypress_handler(&e.xkey);
      default:
        return 0;//I have not idea how this happened.
    }
  }
  return 0;
}

hvk_t x11_passthru(XKeyEvent *xkey) {
  int i,len,sym=XLookupKeysym(xkey,0);
  char line[8];
  char line2[16];
  switch(sym) {
    case XK_Return:
      strcpy(line,"\n");
      len=1;
      break;
    case XK_Left://hack. probably just replace this with printf()s
      strcpy(line,"\x1b[D");
      len=3;
      break;
    case XK_Right:
      strcpy(line,"\x1b[C");
      len=3;
      break;
    case XK_Down:
      strcpy(line,"\x1b[B");
      len=3;
      break;
    case XK_Up:
      strcpy(line,"\x1b[A");
      len=3;
      break;
    default:
      len=XLookupString(xkey,line,1023,NULL,NULL);
      break;
  }
  for(i=0;i/2 < len;i++) {
    line2[i]="0123456789abcdef"[(line[i/2]>>(4*(1-(i%2)))) % 16];
  }
  line2[i]=0;
  printf("%s data %s\n",global.user,line2);
  return 0;
}

hvk_t x11_keypress_handler(XKeyEvent *xkey) {//this only needs to return HVK_ keys based on the XKeyEvent's value... this could be a sparse array.
  int sym=XLookupKeysym(xkey,0);
  switch(sym) {
    case XK_r: return HVK_UP;
    case XK_w: return HVK_FORWARD;
    case XK_space: return HVK_JUMP;
    case XK_s: return HVK_BACKWARD;
    case XK_a: return HVK_LEFT;
    case XK_d: return HVK_RIGHT;
    case XK_Escape: return HVK_ESCAPE;
    case XK_f: return HVK_MAGIC;
    default: return 0;//HVK_NONE;//0
  }
  return 0;
}
