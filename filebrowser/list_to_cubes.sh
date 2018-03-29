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
j=1
r=200
while read -r line;do
 if [ "_$line" != '_' ];then
#  ../tools/obj2hackvr.pl "$line" ../meshes/cube.obj
  #turn line into hex to prevent stupid shit
  hexline="$(printf "%s" "$line" | xxd -p | tr -d '\n')"
  printf "%s addshape 18 3 8 %d %d 0 %d %d 0 %d %d\n" "$hexline" "$j" "$r" "$[$j+8]" "$r" "$j" "$r"
  cd ..
  printf "%s\n" "$line" | tee /dev/stderr | ./makelabel.sh "$hexline" 15 $j $r
  cd filebrowser
  #printf "%s move 0 %d %d 0 0 0 0 0 0\n" "$hexline" "$i" "$r"
  printf "%s rotate 0 %d 0\n" "$hexline" "$[i * 3]"
  #somehow printf '%s\n' "$line" | ./testfont.sh and make its output belong to same group as $line.. sed?
  i=$[i+10]
  j=$[j+4]
 else
  cat camera.pos
  printf "USER deleteallexcept USER\n" #deleteallexcept doesn't have gr deletions yet I think.
  j=0
  i=0
 fi
done
