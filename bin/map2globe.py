#!/usr/bin/python3

import sys
import math

plat=0
plon=0
lat=0
lon=0
x="0"
y="0"
z="0"

#lon goes from -180 to +180
#lat goes from -90 to +90
# / 90 * math.pi
#they need to be in the range 0 to 2pi?

if(len(sys.argv) < 2):
 sys.stderr.write("usage: map2globe.py name < map.dat\n")
 sys.exit(1)

name=sys.argv[1]

print("{} addshape 4 1  0 0 0  1 0 0".format(name))
rotation=0

while(1):
 #print (plat,plon,lat,lon)
 try:
  line=input()
  line.strip('\r\n')
  (lon,lat)=map(float,filter(None,line.split(' ')))
  slat=(((lat)/180)*math.pi)
  slon=(((lon)/180)*math.pi)
  (x,y,z)=map(str,(math.cos(slat)*math.cos(slon), math.sin(slat),math.cos(slat)*math.sin(slon)))
#  (x,y,z)=map(str,(slon,slat,0))
 except EOFError:
  print("# reached EOF")
  break
 except ValueError:
  #ignore this line
  print("# read an invalid line: " + line)
  if(line == ""):
   (lat,lon,x,y,z)=(0,0,"","","")
 except:
  e = sys.exc_info()[0]
  print("# error: " + str(e))
  break
 if(lat and lon and x and y and z and plat and plon and px and py and pz): #if the previouses exist
  print("{} addshape 2 2 {} {} {} {} {} {}".format(name,x,y,z,px,py,pz))

#set previouses to currents
 (plat,plon,px,py,pz)=(lat,lon,x,y,z)

print("# done")
