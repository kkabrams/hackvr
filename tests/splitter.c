char **line_splitter(char *line,int *rlen) {
 char **a;
 int i=0,j=0;
 int len;
 len=1;
 for(i=0;line[i] && line[i] == ' ';i++);//skip leading space
 for(;line[i];) {
  for(;line[i] && line[i] != ' ';i++);//skip rest of data
  for(;line[i] && line[i] == ' ';i++);//skip rest of space
  len++;
 }
 a=malloc(sizeof(char *) * len+1);
 a[len]=0;
 len=0;//reuse!
 for(i=0;line[i] && line[i] == ' ';i++);//skip leading space
 a[len]=line+i;
 for(;;) {
  for(;line[i] && line[i] != ' ';i++);//skip rest of data
  if(line[i]) {
   line[i]=0;
   i++;
  } else {
   //we're at the end! I guess return this shit.
   len++;
   a[len]=0;
   *rlen=len;
   return a;
  }
  for(;line[i] && line[i] == ' ';i++);//skip rest of space
  if(line[i]) {
   len++;
   a[len]=line+i;
  } else {
   len++;
   a[len]=0;
   *rlen=len;
   return a;
  }
 }
 *rlen=len;
 return a;
}

int main(int argc,char *argv[]) {
 char **a;
 int len;
 int i;
 a=line_splitter(strdup(argv[1]),&len);
 for(i=0;i<len;i++) {
  printf("a[%d]=\"%s\"\n",i,a[i]);
 }
 return 0;
}
