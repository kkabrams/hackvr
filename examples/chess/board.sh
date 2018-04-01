#!/bin/bash

width=8
height=8
cols="a b c d e f g h"

for x in $(seq 1 $width);do
 for y in $(seq 1 $height);do
  color=$[((($y % 2) + ($x % 2))%2)+16]
### board is on the wall. draw in x and y.
#  printf "%s%s addshape %s 4 %s %s 0 %s %s 0 %s %s 0 %s %s 0\n" $(printf "%s\n" "$cols" | cut '-d ' -f$x) $y $color $x $y $[x+1] $y $[x+1] $[y+1] $x $[y+1]
### board is on floor. draw in x and z.
  printf "%s%s addshape %s 4 %s 0 %s %s 0 %s %s 0 %s %s 0 %s\n" $(printf "%s\n" "$cols" | cut '-d ' -f$x) $y $color $x $y $[x+1] $y $[x+1] $[y+1] $x $[y+1]
 done
done

row8="rook knight bishop queen king bishop knight rook"
row1="$row8"

color="19 19 0 0 0 0 20 20"

#draw pawns. triangles.
for x in $(seq $width);do
 for y in 2 7;do
  locat="$(printf "%s\n" "$cols" | cut '-d ' "-f$x")$y"
  cat "./pieces/pawn" | sed 's/^[^ ][^ ]* addshape [0-9][0-9]* /piece_'"${locat}"' addshape '"$(printf "%s\n" "$color" | cut '-d ' "-f$y")"' /g'
### board is on wall. use x and y.
#  printf "piece_%s move %s %s 0\n" "$locat" "$x" "$y"
### board is on floor. use x and z.
  printf "piece_%s move %s 0 %s\n" "$locat" "$x" "$y"
 done
 for y in 1 8;do
  locat="$(printf "%s\n" "$cols" | cut '-d ' "-f$x")$y"
  cat "./pieces/$(printf "%s\n" "$row1" | cut '-d ' "-f$x")" | sed 's/^[^ ][^ ]* addshape [0-9][0-9]* /piece_'"${locat}"' addshape '"$(printf "%s\n" "$color" | cut '-d ' "-f$y")"' /g'
### board is on wall. use x and y.
#  printf "piece_%s move %s %s 0\n" "$locat" "$x" "$y"
### board is on floor. use x and z.
  printf "piece_%s move %s 0 %s\n" "$locat" "$x" "$y"
 done
done
