#!/bin/sh
#this is used on turn2line output
USAGE="usage: line2hackvr.sh name < turn2line-output"
if [ "_$1" = '_' ];then
 echo $USAGE >&2
 exit 1
fi
if [ "_$1" = "_-h" -o "_$1" = "_--help" -o "_$1" = "_-help" ];then
 echo $USAGE
 exit 0
fi
xargs -L 1 printf "%s addshape 2 2 0 %d %d 0 %d %d\n" "$1" $line
