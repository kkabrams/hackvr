#!/bin/bash
# blank lines deleteallexcept epoch
# expected input format:
#
# line1
# line2
# line3
#
# line1
# line2
while read -r line;do
 if [ "_$line" != '_' ];then
#  ../tools/obj2hackvr.pl "$line" ../meshes/cube.obj
  printf "%s addshape 18 3 8 0 0 0 8 0 0 0 0\n" "$line"
  cd ..
  printf "%s\n" "$line" | ./makelabel.sh "$line" 15 0 0
  cd filebrowser
  printf "%s move 0 %d 0 0 0 0 0 0 0\n" "$line" "$i"
  #somehow printf '%s\n' "$line" | ./testfont.sh and make its output belong to same group as $line.. sed?
  i=$[i+10]
 else
  printf "USER deleteallexcept USER\n" #deleteallexcept doesn't have gr deletions yet I think.
  i=0
 fi
done
