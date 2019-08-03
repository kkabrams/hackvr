#include <idc.h>
#include "common.h"
#include "graphics_x11.h"
#include "keyboard_x11.h"
#include "mouse_x11.h"
#include "input.h"

extern struct x11_global x11_global;

int input_init() {
  return x11_global.fd;
}

void input_event_handler(struct shit *me,char *line) {
  if(keyboard_event_handler(me,line)) {
  }
  if(mouse_event_handler(me,line)) {
  }
}
