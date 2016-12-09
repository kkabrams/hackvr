#include <stdio.h>
#include <stdlib.h>

char *read_line_hack(FILE *fp,int len) {
 short in;
 char *t;
 if((in=fgetc(fp)) == '\n') {
  t=malloc(len+1);
  t[len]=0;
  return t;
 }
 t=read_line_hack(fp,len+1);
 t[len]=in;
 return t;
}

int main() {
 char *t=read_line_hack(stdin,0);
 printf("%s\n",t);
}
