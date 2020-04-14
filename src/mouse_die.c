#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#include "graphics_c3.h"

extern struct hvr_global global;
extern struct gra_global gra_global;

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
  int butt;
  int l;
  int i;
  int redrawplzkthx=0;
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
    //these types are movement: 8 40 56 24
    for(i=0;i<4;i++) {//we need to loop over all buttons each event. :/
      butt=ie.type & 0x07 & (1<<i);
      if(butt) {//this button is down
        fprintf(stderr,"# mouse button %d is down.\n",butt-1);
	if(gra_global.mousemap[butt-1] != 1) {//freshly down. save coords and set it to down.
          gra_global.dragstart[butt-1]=gra_global.mouse;
          gra_global.mousemap[butt-1]=1;
	  redrawplzkthx=1;
	}
      } else {
        if(gra_global.mousemap[(1<<i)-1]) {//only if it was pressed we mark it as "released"
	  fprintf(stderr,"# mouse button %d is being marked as released.\n",(1<<i)-1);
          gra_global.mousemap[(1<<i)-1]=-1;
	  if((1<<i)-1 == 1) {//do this for right-click only.
	    gra_global.oldcamera = global.camera.r;//we're done dragging, so set this as where the camera points at for next drag around
	  }
	  redrawplzkthx=1;
	  //this is where camera rotation based on dragging is done?
	}
      }
    }
    if(ie.dx || ie.dy) {
      fprintf(stderr,"# mouse debug: type:\t%d\tdx:%d\tdy:%d\n",ie.type,ie.dx,ie.dy);
      gra_global.mouse.x+=ie.dx;
      gra_global.mouse.y+=ie.dy;
      fprintf(stderr,"# mouse: x:%F y:%F\n",gra_global.mouse.x,gra_global.mouse.y);
      if(gra_global.mousemap[1] == 1) {//if "right" click is held down
        global.camera.r.x.d=gra_global.oldcamera.x.d + (gra_global.mouse.y - gra_global.dragstart[1].y);
        global.camera.r.y.d=gra_global.oldcamera.y.d + (gra_global.mouse.x - gra_global.dragstart[1].x);
      }
      redrawplzkthx=1;
    }
  }
  if(redrawplzkthx) {
    redraw();
    return 1;
  }
  return 0;
}
