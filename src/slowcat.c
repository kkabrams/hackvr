#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[]) {
 short in;
 FILE *fp=stdin;
 if(argc < 2 || (argc >= 2 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")))) {
  fprintf(stderr,"usage: slowcat delay [file1] [file2] [...]\n");
  return 1;
 }
 if(argc > 2) {
  fp=fopen(argv[2],"r");
 }
 do {
  while((in=fgetc(fp)) != -1) {
   printf("%c",in);
   if(in == '\n') {
    fflush(stdout);
    usleep(atoi(argv[1]));
   }
  }
  fclose(fp);
  argv++;
 } while((fp=fopen(argv[1],"r")));
 return 0;
}
