#include <stdio.h>
#include <string.h>
#include <unistd.h>

//nsew
char *a[16];

char field[256];

#define NORTH_EXIT 8
#define SOUTH_EXIT 4
#define EAST_EXIT 2
#define WEST_EXIT 1

int deg[]={0,180,270,90};

void print_field() {
  int i,d;
  int row;
  for(i=0;i<256;i++) {
    if(i%16 == 0) {//next row of data.
      //start position is... 7+16, which is second row, 7th column
      row++;
    }
    //we need to draw the room here.
    //each room will be... 16x16? sure....
    //we can use bitmasks to find with sides of the room need to be whole walls
    //and which will contain passages to the next rooms
    //whole walls are a single rectangle that goes from z0 to z... 8 I guess
    //we need to loop over each wall and check if it has a passage through it now
    if(field[i]) {
      for(d=0;d<4;d++) {
        if((field[i] & (1<<(3-d))) > 0) {//if this has an exit in this bit...
          printf("wall_%d_%d addshape 2 4  -7 0 7  -7 8 7  -2 8 7  -2 0 7\n",i,d);//we need 4 parts for the doorway
          printf("wall_%d_%d addshape 2 4  2 0 7  2 8 7  7 8 7  7 0 7\n",i,d);
          printf("wall_%d_%d addshape 2 4  -2 0 7  -2 8 7  -2 8 8  -2 0 8\n",i,d);
          printf("wall_%d_%d addshape 2 4  2 0 8  2 8 8  2 8 7  2 0 7\n",i,d);
        } else {
          printf("wall_%d_%d addshape 2 4  -7 0 7  -7 8 7  7 8 7  7 0 7\n",i,d);//solid wall
        }
        printf("wall_%d_%d rotate 0 %d 0\n",i,d,deg[d]);
        printf("wall_%d_%d move %d 0 %d\n",i,d, (i%16) * 16 - (7 * 16),-(i/16) * 16 + (16));
      }
    }
  }
  
}

int main(int argc,char *argv[]) {
  int i;
  //fprintf(stderr,"\x1b[H\x1b[2J");
  a[0x0]="   ";
  a[0x1]="-  ";
  a[0x2]="  -";
  a[0x3]="---";
  a[0x4]=" . ";
  a[0x5]="-. ";
  a[0x6]=" .-";
  a[0x7]="-.-";
  a[0x8]=" ' ";
  a[0x9]="-' ";
  a[0xa]=" '-";
  a[0xb]="-'-";
  a[0xc]=" | ";
  a[0xd]="-| ";
  a[0xe]=" |-";
  a[0xf]="-|-";
  while(read(0,field,sizeof(field)) > 0) {
    print_field();
    break;
    //sleep(1);
  }
  return 0;
}
