#include <stdio.h>
#include <string.h>
#include <unistd.h>

//nsew
char field[256];

#define NORTH_EXIT 8
#define SOUTH_EXIT 4
#define EAST_EXIT 2
#define WEST_EXIT 1

int deg[]={0,180,270,90};
int doorx[]={};
int doory[]={};

void print_field() {
  int i,d;
  int x,y;
  for(i=0;i<256;i++) {
    //we need to draw the room here.
    //each room will be... 16x16? sure....
    //we can use bitmasks to find which sides of the room need to be whole walls
    //and which will contain passages to the next rooms
    //whole walls are a single rectangle
    //we need to loop over each wall and check if it has a passage through it
    if(field[i]) {
      for(d=0;d<4;d++) {//loop over the 4 possible directions/walls
        x=(i%16) * 16 - (7 * 16);
        y=(i/16) * -16 + 16;
        if((field[i] & (1<<(3-d))) > 0) {//if this has an exit in this bit...
          printf("wall_%d_%d addshape 2 4  -7 0 7  -7 8 7  -2 8 7  -2 0 7\n",i,d);//we need 4 parts for the doorway
          printf("wall_%d_%d addshape 2 4  2 0 7  2 8 7  7 8 7  7 0 7\n",i,d);
          if(d % 2) {//only do the south and west doorjams. every door has an opposite door that matches it anyway
            printf("wall_%d_%d addshape 2 4  -2 0 7  -2 8 7  -2 8 9  -2 0 9\n",i,d);
            printf("wall_%d_%d addshape 2 4  2 0 9  2 8 9  2 8 7  2 0 7\n",i,d);
            printf("door_close_%d_%d addshape 3 4  0 0 0  0 8 0  4 8 0  4 0 0\n",i,d);//door needs to be built to where its hinge is at 0,y,0, then moved into place so opening will work.
            printf("door_close_%d_%d rotate 0 %d 0\n",i,d,deg[d]);
            if(d == 1) printf("door_close_%d_%d move %d 0 %d\n",i,d,x+2,y-8);
            if(d == 3) printf("door_close_%d_%d move %d 0 %d\n",i,d,x-8,y-2);
          }
        } else {//no door in this wall
          printf("wall_%d_%d addshape 2 4  -7 0 7  -7 8 7  7 8 7  7 0 7\n",i,d);//solid wall
        }
        printf("wall_%d_%d rotate 0 %d 0\n",i,d,deg[d]);//rotate this wall into position
        printf("wall_%d_%d move %d 0 %d\n",i,d,x,y);//and put it in the right spot ofc
        //the door needs to move either -2, +8
      }
    }
  }
}

int main(int argc,char *argv[]) {
  if(read(0,field,sizeof(field)) > 0) {
    print_field();
    return 0;
  }
  return 1;
}
