#!/bin/bash
xoffset=0
yoffset=0
while read -N 1 c;do
 name="$(printf "%02x" "'$c")"
 if [ "$name" = "0a" ]; then
  xoffset=-6
  yoffset="$(printf '%d - %d\n' $yoffset 10 | bc)"
 fi
 grep "^$name " font/font.hackvr | sed 's/^'"$name"'/'"${name}_${xoffset}_${yoffset}"'/g'
 printf "%s_%s_%s move %d %d 0\n" "$name" "$xoffset" "$yoffset" "$xoffset" "$yoffset"
 xoffset="$(printf '%d + %d\n' $xoffset 6 | bc)"
done
