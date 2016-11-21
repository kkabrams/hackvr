#!/bin/bash
while read -r line;do
 if [ "_$line" != '_' ];then
#  ../tools/obj2hackvr.pl "$line" ../meshes/cube.obj
  printf "%s addtriangle 1 0 0 0 1 0 0 0 0\n" "$line"
  printf "%s move 0 %d 0 0 0 0 0 0 0\n" "$line" "$i"
  i=$[i+2]
 else
  printf "epoch deleteallexcept epoch\n"
  i=0
 fi
done
