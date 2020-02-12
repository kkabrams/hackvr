#!/usr/bin/env python3
import math

for i in range(1,13):
  x=math.floor(math.sin(math.pi / 6 * i) * 1000) / 100
  y=math.floor(math.cos(math.pi / 6 * i) * 1000) / 100
  print("_clock_face_digit_{}_ addshape 2 4 {} {} 0 {} {} 0 {} {} 0 {} {} 0".format(i,x-1,y-1,x+1,y-1,x+1,y+1,x-1,y+1))
  print("_clock_face_hand_hour_ addshape 2 3 -1 -1 0   0 5 0  1 -1 0")
  print("_clock_face_hand_minute_ addshape 2 3 -.75 -.75 0   0 8 0  .75 -.75 0")
  print("_clock_face_hand_second_ addshape 2 3 -.25 -.25 0   0 10 0  .25 -.25 0")
