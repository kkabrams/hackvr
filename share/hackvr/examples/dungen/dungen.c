#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 16

//nsew
char *a[16];

char field[256];
char nfield[256];

void print_field() {
  int i;
  printf("\x1b[H");
  for(i=0;i<(16*strlen(a[field[0]]))+1;i++) printf("#");
  for(i=0;i<256;i++) {
    if(i%16 == 0) printf("#\n#");
    printf("%s",a[field[i]]);
  }
  printf("#\n");
  for(i=0;i<(16*strlen(a[field[0]]))+2;i++) printf("#");
  printf("\n");
}

void prune() {//remove paths into wall
  int i;
  memcpy(nfield,field,256);//make a copy just for comparison
  for(i=0;i<256;i++) {
    if(i%16 == 0) field[i] &= ~0x1;//remove all left exits at left of map
    else if((field[i]&0x1) && !(nfield[i-1]&0x2)) field[i] &= ~0x1;
    if(i%16 == 15) field[i] &= ~0x2;//remove all right exits at right of map.
    else if((field[i]&0x2) && !(nfield[i+1]&0x1)) field[i] &= ~0x2;
    if(i > 240) field[i] &= ~0x4;
    else if((field[i]&0x4) && !(nfield[i+16]&0x8)) field[i] &= ~0x4;
    if(i < 16) field[i] &= ~0x8;
    else if((field[i]&0x8) && !(nfield[i-16]&0x4)) field[i] &= ~0x8;
  }
}

void grow() {
  int i=0;
  memcpy(nfield,field,256);
  for(i=0;i<256;i++) {
    if(random() % 2 == 0) {
      if((field[i]&0x1) && !nfield[i-1] && i%16 != 0) nfield[i-1]=random()%16 | 0x2;
      if((field[i]&0x2) && !nfield[i+1] && i%16 != 15) nfield[i+1]=random()%16 | 0x1;
      if((field[i]&0x4) && !nfield[i+16] && i<240) nfield[i+16]=random()%16 | 0x8;
      if((field[i]&0x8) && !nfield[i-16] && i>16) nfield[i-16]=random()%16 | 0x4;
    }
  }
  memcpy(field,nfield,256);
}

int main(int argc,char *argv[]) {
  int i;
  int seed=time(0) * getpid();
  if(argc > 1) {
    seed=atoi(argv[1]);
  }
  printf("\x1b[H\x1b[2J");
  srandom(seed);
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

  field[7+16] = 0x4;//center top
  for(i=0;i<ITERATIONS;i++) {
    print_field();
//    sleep(1);
    grow();
  }
  prune();
  print_field();
  printf("seed: %d\n",seed);
}
