#!/usr/bin/python
###
### so... instead of x,y,z then more x,y,z we take x,y,z then x,y,z rotations and a distance.
### so input would look like... 0 0 0  0 0 90 10
### and the output should be... addshape 0 2  0 0 0  0 0 10
import sys
import math
x=int(sys.argv[1]);
y=int(sys.argv[2]);
z=int(sys.argv[3]);
d=int(sys.argv[4]);
r=int(sys.argv[5]);
#print("{} {} {}\n".format(x, y, z))
x=math.sin(d * 2.0 * math.pi / 360.0)*r
y=math.cos(d * 2.0 * math.pi / 360.0)*r
print("{} {} 0".format(x,y))
