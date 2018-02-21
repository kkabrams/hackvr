#!/bin/sh
while true;do
 for x in $(seq 0 5 360);do
  for y in $(seq 0 5 360);do
   for z in $(seq 0 5 360);do
    echo "$1 rotate $x $y $z"
    sleep .0001
   done
  done
 done
done
