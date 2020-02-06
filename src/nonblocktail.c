#include <stdio.h>
#include <idc.h>
#include <fcntl.h>

extern struct idc_global idc;

void handler(struct shit *me,char *line) {
  if(!line) return;
  if(line) printf("%s\n",line);
}

int main(int argc,char *argv[]) {
  int i;
  for(i=0;i<100;i++) {
    idc.fds[i].fd=-1;
  }
  for(argv++,argc--;argc;argc--,argv++) {
    idc.fds[add_fd(open(*argv,O_RDONLY),handler)].keep_open=1;
  }
  select_on_everything();
}
