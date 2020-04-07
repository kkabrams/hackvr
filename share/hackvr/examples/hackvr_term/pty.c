#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *argv[]) {
  char *pts;
  char in[256];//I dunno.
  int r;
  int pid;
  int eof1,eof2;
  int master,slave;
  master=open("/dev/ptmx",O_RDWR);
  if(master == -1) return 1;
  pts=ptsname(master);
//  printf("%s\n",pts);
//  system("ls -l /dev/pts/*");
  grantpt(master);
  unlockpt(master);
//  system("ls -l /dev/pts/*");
  if(pts == NULL) return 2;
  slave=open(pts,O_RDWR);
  if(slave == -1) {
   perror("open");
   return 3;
  }
  argv++;
  fcntl(master,F_SETFL,O_NONBLOCK);
//  fcntl(slave,F_SETFL,O_NONBLOCK);
  fcntl(0,F_SETFL,O_NONBLOCK);
  fcntl(1,F_SETFL,O_NONBLOCK);
  fcntl(2,F_SETFL,O_NONBLOCK);
  switch(pid=fork()) {
    case -1:
      return 4;//fork failed
    case 0://child
      setsid();
      close(master);
      dup2(slave,0);
      dup2(slave,1);
      dup2(slave,2);
      execv(argv[0],argv);//execute arguments.
      return 5;//exec failed
    default:
      break;
  }
eof1=0;
eof2=0;
  for(;!eof1 && !eof2;) {
    if(waitpid(-1,0,WNOHANG) > 0) {
      return 0;//fuck if I know.
      //we got a dead child. let's bail.
    }
    switch(r=read(0,&in,1)) {
     case 0: eof1=1;//EOF
     case -1: break;//EAGAIN probably.
     default: write(master,in,r);
    }
    switch(r=read(master,&in,1)) {
     case 0: eof2=1;//EOF
     case -1: break;//EAGAIN probably
     default: write(1,in,r);
    }
    usleep(100);//kek
  }
}
