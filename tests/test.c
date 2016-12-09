#include <stdio.h>

typedef double long c3_t;
typedef double long c2_t;
typedef struct {
  int x;
  int y;
} cs_t;

cs_t derp() {
  return (cs_t){2,3};
}

int main(int argc,char *argv[]) {
 cs_t cs=derp();
 printf("%d,%d\n",cs.x,cs.y);
 switch(1) {
   case 1:
    printf("first\n");
    break;
   default:
    printf("third.\n");
    break;
 }
 return 0;
}
