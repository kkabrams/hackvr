#!/bin/sh
#this is used on turn2line output
xargs -L 1 printf "dragon addshape 2 0 %d %d 0 %d %d\n" $line
