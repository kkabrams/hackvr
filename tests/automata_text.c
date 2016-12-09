#include <stdio.h>
#include <string.h>

#define HE he
#define WI wi
#define RULE rule


int main(int argc,char *argv[]) {
  int i,j;
  int rule=atoi(argv[1]);
  char str[256];//lel. close enough.
  int wi=read(0,str,sizeof(str)-1)*8;
  int he=atoi(argv[2]);
  char f[HE+1][WI+1];
  f[0][WI/2]=1;
  for(i=0;i<HE;i++) {
   for(j=0;j<WI;j++) {
    if(i==0) f[i][j]=(str[j/8]>>(j%8))%2;
    else f[i][j]=(RULE >> ((!!f[(i+HE-1)%HE][(j+WI-1)%WI]<<2) | (!!f[(i+HE-1)%HE][j]<<1) | (!!f[(i+HE-1)%HE][(j+1)%WI])) & 1);
   }
  }
  for(i=0;i<HE;i++) {
   for(j=0;j<WI;j++) {
    if(f[i][j]) {
     printf("%c",f[i][j]+'0');
    } else {
     printf(" ");
    }
   }
   printf("\n");
  }
 return 0;
}
