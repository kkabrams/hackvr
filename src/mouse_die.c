#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>


//#define MOUSEDEV "/dev/input/mouse0" //just one of the many possibly connected mice. (just in case you want to use one mouse for one thing and another mouse for something else)
#define MOUSEDEV "/dev/input/mice" //all the mice connected act as one.

int mfd = -1;

//#define "mouse.h" //I guess

struct wtf {
  unsigned char type;
  char dx;
  char dy;
};

int mouse_init() {
  if((mfd=open(MOUSEDEV,O_RDWR)) == -1) {
    fprintf(stderr,"# failed to open mouse: %d\n",mfd);
  }
  return mfd;
}

int mouse_event_handler() {
  struct wtf ie;
  int l;
  memset(&ie,0,sizeof(ie));
  if(mfd == -1) {
    mfd=open(MOUSEDEV,O_RDWR);
    //probably don't need nonblock anymore.
    //fcntl(mfd,F_SETFL,O_NONBLOCK);
  }
  if(mfd == -1) {
    fprintf(stderr,"# mouse shit fucked up.\n");
    return 1;
  }
  if((l=read(mfd,&ie,sizeof(ie))) > 0) {
    //type == 8 and a or of some bits to say which direction.
    fprintf(stderr,"# mouse debug: type:\t%d\tdx:%d\tdy:%d\n",ie.type,ie.dx,ie.dy);
  }
  return 0;
}
