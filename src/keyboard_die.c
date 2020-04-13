#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#include "keyboard.h"

#define KBDEV "/dev/input/event0"

int kbfd = -1;

int keyboard_init() {
  if((kbfd=open(KBDEV,O_RDWR)) == -1) {
    fprintf(stderr,"# failed to open keyboard: %s\n",KBDEV);
  }
  return kbfd;
}

/*void keyboard_event_handler(struct *me,char *junk) {
  //wtf goes here?
}*/

hvk_t die_keypress_handler(unsigned short code) {
  switch(code) {
    case KEY_W: return HVK_FORWARD;
    case KEY_S: return HVK_BACKWARD;
    case KEY_A: return HVK_LEFT;
    case KEY_D: return HVK_RIGHT;
    case KEY_SPACE: return HVK_JUMP;
    case KEY_ESC: return HVK_ESCAPE;
  }
  return 0;//I dunno.
}

hvk_t get_keyboard_event() {
  struct input_event ie;
  int l;
  memset(&ie,0,sizeof(ie));
  if(kbfd == -1) {
    kbfd=open(KBDEV,O_RDWR);
    //probably not needed anymore
    //fcntl(kbfd,F_SETFL,O_NONBLOCK);
  }
  if(kbfd == -1) {
    fprintf(stderr,"# keyboard shit fucked up. probably permissions error.\n");
    return 1;
  }
  if((l=read(kbfd,&ie,sizeof(ie))) > 0) {
    if(ie.type == 1) {
     fprintf(stderr,"# value: %d code: %d type: %d\n",ie.value,ie.code,ie.type);
     if(ie.value == 0) { //release
      return -die_keypress_handler(ie.code);
     }
     if(ie.value == 1) { //press
      return die_keypress_handler(ie.code);
     }
     if(ie.value == 2) { //repeated from being held down
      //we should return die_keypress_handler(ie.code) if we want to allow outside repeats to be treated like a rapid-fire button.
     }
    }
  }
  return 0;
}
