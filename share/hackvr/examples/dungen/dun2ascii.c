#include <stdio.h>
#include <string.h>
#include <unistd.h>

//nsew
char *a[16];

char field[256];

void print_field() {
  int i;
  fprintf(stderr,"\x1b[H");
  for(i=0;i<(16*strlen(a[field[0]]))+1;i++) fprintf(stderr,"#");
  for(i=0;i<256;i++) {
    if(i%16 == 0) fprintf(stderr,"#\n#");
    fprintf(stderr,"%s",a[field[i]]);
  }
  fprintf(stderr,"#\n");
  for(i=0;i<(16*strlen(a[field[0]]))+2;i++) fprintf(stderr,"#");
  fprintf(stderr,"\n");
}

int main(int argc,char *argv[]) {
  int i;
  fprintf(stderr,"\x1b[H\x1b[2J");
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
    sleep(1);
  }
}
