#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#include "graphics_c3.h"
#include "mouse.h"

extern struct hvr_global global;
extern struct gra_global gra_global;

//#define MOUSEDEV "/dev/input/mouse0" //just one of the many possibly connected mice. (just in case you want to use one mouse for one thing and another mouse for something else)
#define MOUSEDEV "/dev/input/event19"

int mfd = -1;

//#define "mouse.h" //I guess

struct wtf {//totals to 24
  struct timeval time;
  unsigned short type;
  unsigned short code;
  unsigned int state;
};

int mouse_init() {
  if((mfd=open(MOUSEDEV,O_RDWR)) == -1) {
    fprintf(stderr,"# failed to open mouse: %d\n",mfd);
  }
  return mfd;
}

//buttons are:
//0 for left click, 1 for right click, 2 is middle-click

//in the hackvr mouse map, right is 2, left is 0 and center is 1?
#define DIE_MOUSE_LEFT 0
#define DIE_MOUSE_RIGHT 1
#define DIE_MOUSE_CENTER 2
//it doesn't care if you have X11 buttons swapped around ofc.
char die2map(char d) {//edit this function if you want to change your primary and secondary mouse button.
  switch(d) {
    case DIE_MOUSE_LEFT: return MOUSE_PRIMARY;
    case DIE_MOUSE_RIGHT: return MOUSE_SECONDARY;
    case DIE_MOUSE_CENTER: return MOUSE_TERTIARY;
    default: return -1;
  }
}

int mouse_event_handler() {
  struct wtf ie;
  int butt;
  int l;
  int i;//this is a DIE_MOUSE value
  unsigned char m;//this is a hackvr mouse map value.
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
    if(ie.code == 3) global.camera.r.x.d=ie.state;
    if(ie.code == 4) global.camera.r.y.d=ie.state;
    if(ie.code == 5) global.camera.r.z.d=ie.state;
/*
    for(i=0;i<3;i++) {//we need to loop over all buttons each event. :/
      butt=ie.type & 0x07 & (1<<i);//lowest 3 bits are possible mouse button states
      m=die2map(i);
      if(m == -1) {
        continue;//not gonna even try.
      }
      if(butt) {//this button is down
        fprintf(stderr,"# mouse button %d is down.\n",butt-1);
	if(gra_global.mousemap[m] != 1) {//freshly down. save coords and set it to down.
          gra_global.dragstart[m]=gra_global.mouse;
          gra_global.mousemap[m]=1;
          redrawplzkthx=1;
        }
      } else {
        if(gra_global.mousemap[m]) {//only if it was pressed we mark it as "released"
	  fprintf(stderr,"# mouse button %d is being marked as released.\n",(1<<i)-1);
          gra_global.mousemap[m]=-1;
	  if(m == MOUSE_SECONDARY) {//do this for secondary button release
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
      if(gra_global.mouse.x < LEFT) gra_global.mouse.x = LEFT;
      if(gra_global.mouse.x > RIGHT) gra_global.mouse.x = RIGHT;
      gra_global.mouse.y+=ie.dy;
      if(gra_global.mouse.y < BOTTOM) gra_global.mouse.y = BOTTOM;
      if(gra_global.mouse.y > TOP) gra_global.mouse.y = TOP;
      fprintf(stderr,"# mouse: x:%F y:%F\n",gra_global.mouse.x,gra_global.mouse.y);
      if(gra_global.mousemap[MOUSE_SECONDARY] == 1) {//if "right" click is held down... this is somehow primary click
        global.camera.r.x.d=gra_global.oldcamera.x.d + (gra_global.mouse.y - gra_global.dragstart[MOUSE_SECONDARY].y);
        global.camera.r.y.d=gra_global.oldcamera.y.d + (gra_global.mouse.x - gra_global.dragstart[MOUSE_SECONDARY].x);
      }
      redrawplzkthx=1;
    }
*/
  }
  if(redrawplzkthx) {
    redraw();
    return 1;
  }
  return 0;
}
/*
#include <stdio.h>
#include <sys/time.h>


int main(int argc,char *argv[]) {
  int i;
  struct input_event ie;
  int x,y,z;
  while(!feof(stdin)) {
    read(0,&ie,sizeof(struct input_event));
    printf("type: %d :: ",ie.type);
    if(ie.code == 3) x=ie.state;
    if(ie.code == 4) y=ie.state;
    if(ie.code == 5) z=ie.state;
    printf("%15d %15d %15d",x,y,z);
    //for(i=0;i<16;i++) {
    //  printf(" %02x",(unsigned char)ae.derp[i]);
    //}
    printf("\n");
  }
}
*/
