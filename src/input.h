#ifndef _HV_MOUSE_H_
#define _HV_MOUSE_H_

//these might as well be in keyboard.h and mouse.h
//but I don't have separate files for those yet.
//all keyboard and mouse drivers will need to implement these functions

#include <idc.h>

//these *_init() will return the fd that will have the keyboard and mouse events
int mouse_init();
int keyboard_init();
//these two functions are libidc handlers
void mouse_event_handler(struct shit *,char *);
void keyboard_event_handler(struct shit *,char *);

#endif
