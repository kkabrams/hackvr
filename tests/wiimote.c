#include <stdio.h>
#include <sys/time.h>

struct input_event{//totals to 24
  struct timeval time;
  unsigned short type;
  unsigned short code;
  unsigned int state;
};

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
