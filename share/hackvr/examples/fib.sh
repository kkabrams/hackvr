#!/bin/sh
s=1 1 2  3  5  8  13
x=0 0 1  0 -5 -5 
y=0 1 0 -3 -3  0 

for i in 1 1 2 3 5 8 13 21 34 55 89;do
  x=0 
  y=0
  w=i
  h=i
  addshape 2 4 x y 0 x+w y 0 x+w y+h 0 x y+h 0
done
