#!/usr/bin/env bash
z=0
a=1
b=1

for i in $(seq 1 10);do
  if [ $[$i % 2] = 0 ];then
    x=$[$z]
    y=0
  else
    x=0
    y=$[$z]
  fi

  w=$a
  h=$a

  echo fib_$a addshape 2 4 $x $y 0 $[$x+$w] $y 0 $[$x+$w] $[$y+$h] 0 $x $[$y+$h] 0
  z=$a
  s=$[$a+$b]
  a=$b
  b=$s

done
