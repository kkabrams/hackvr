#!/bin/bash
target="$1"
xoffset=$2
yoffset=$3
zoffset=$4
while read -rN 1 c;do
 name="$(printf "%02x" "'$c")"
 if [ "$name" = "0a" ]; then
  xoffset=-6
  yoffset="$(printf '%d - %d\n' $yoffset 10 | bc)"
 fi
 grep "^$name " font/font.hackvr | sed 's ^'"$name"' '"$target"' ' | ./tools/offsetshape.sh "$xoffset" "$yoffset" "$zoffset"
 xoffset="$(printf '%d + %d\n' $xoffset 6 | bc)"
done
