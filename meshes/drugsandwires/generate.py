#!/usr/bin/python -u

from math import cos
from math import sin
from math import radians
from time import sleep

def addvector(p,c,theta):
 return (p[0]+cos(radians(theta))*c,p[1]+sin(radians(theta))*c)

def addquad(D,G,J,M):
 print("dnw addshape 2 4 {} {} 0 {} {} 0 {} {} 0 {} {} 0\n".format(D[0],D[1],G[0],G[1],J[0],J[1],M[0],M[1]))

def main():
 c=1
 org=(0,0)

 r=7
 theta=0

 ## we should have a triangle of A B C after this.
 A=addvector(org,c,theta)
 B=addvector(org,c,theta+120)
 C=addvector(org,c,theta+240)

 ## these three points are along that triangle
 ## but are 1/5 of the way towards one of the points.
 D=map(lambda x,y:(4*x + y)/5 ,A,B)
 E=map(lambda x,y:(4*x + y)/5 ,B,C)
 F=map(lambda x,y:(4*x + y)/5 ,C,A)

 G=addvector(D,c*r,theta+150)
 H=addvector(E,c*r,theta+270)
 I=addvector(F,c*r,theta+30)

 ### points A B and C won't be used in the outputted quads.
 J=addvector(D,c,theta+30)
 K=addvector(E,c,theta+150)
 L=addvector(F,c,theta+270)

 M=addvector(J,c*r,theta+150)
 N=addvector(K,c*r,theta+270)
 O=addvector(L,c*r,theta+30)

 print("dnw addshape 2 1  0 0 1  {} 0 1\n".format(r))
 print("dnw addshape 0 1  0 0 1  {} 0 1\n".format(r-c))

 addquad(D,J,M,G)
 addquad(E,K,N,H)
 addquad(F,L,O,I)

#uncomment me to make logo spin
# rot=0
# while(1):
#  print("dnw rotate 0 0 {}\n".format(rot))
#  sleep(.005)
#  rot+=1
#  rot%=360

main()
