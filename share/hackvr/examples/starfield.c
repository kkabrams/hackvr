#include <stdio.h>

int main(int argc,char *argv[]) {

  int i,x,y,z;
  for(i=0;i<100;i++) {
    x=(rand()%1000)-500;
    y=(rand()%1000)-500;
    z=(rand()%1000)-500;
    printf("star%d addshape %d 1 %d %d %d %d %d %d\n",i,(rand()%7)+17,x,y,z,x+rand()%10,y,z);
  } 
}
