#!/usr/bin/env python

import math

def fp(p):
    return " {} {} {}".format(p[0],p[1],p[2])

def print_rect(r):
    print("gate addshape 3 4 {} {} {} {}".format(fp(r[0]),fp(r[1]),fp(r[2]),fp(r[3])))

d=1

sides = 16.0
step = 360.0/sides
rot=0.0
zoff = 0.0
while rot < 360:
#    print("rot: {}".format(rot))
    x1=math.sin(rot * math.pi / 180.0)
    y1=math.cos(rot * math.pi / 180.0)
    rot = rot + step
    x2=math.sin(rot * math.pi / 180.0)
    y2=math.cos(rot * math.pi / 180.0)

#    print("{} {}".format(x1,y1))

    ### now to calculate points for the 4 rectangles we need to draw...
    v1 = x1 * (d+30.0) # inner first point
    v2 = x1 * (d+32.0) # outer first point
    v3 = x1 * (d+31.0) # center first point

    v4 = y1 * (d+30.0)
    v5 = y1 * (d+32.0)
    v6 = y1 * (d+31.0)

    v7 = x2 * (d+30.0)
    v8 = x2 * (d+32.0)
    v9 = x2 * (d+31.0)

    v10 = y2 * (d+30.0)
    v11 = y2 * (d+32.0)
    v12 = y2 * (d+31.0)

    p1 = (v1, v4, zoff + 0.0)
    p2 = (v2, v5, zoff + 0.0)
    p3 = (v3, v6, zoff + 1.0)
    p4 = (v3, v6, zoff - 1.0)

    p5 = (v7, v10, zoff + 0.0)
    p6 = (v8, v11, zoff + 0.0)
    p7 = (v9, v12, zoff + 1.0)
    p8 = (v9, v12, zoff - 1.0)

    r1 = (p1, p5, p7, p3) #inner near
    r2 = (p1, p5, p8, p4) #inner far
    r3 = (p2, p6, p7, p3) #outer near
    r4 = (p2, p6, p8, p4) #outer far

    ### first rectange is...
    print_rect(r1)
    print_rect(r2)
    print_rect(r3)
    print_rect(r4)
