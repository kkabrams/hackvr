#!/bin/sh
#this is used on turn2line output
if [ "_$1" = '_' ];then
 echo 'usage: line2hackvr.sh name' >&2
 exit 1
fi
xargs -L 1 printf "%s addshape 2 0 %d %d 0 %d %d\n" "$1" $line
