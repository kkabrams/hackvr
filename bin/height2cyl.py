#!/usr/bin/env python3
import random
import math
import sys
from PIL import Image
#we're going to make triangles. lots of triangles.
#since the heightmap is in a square pattern
#we're going to have a point that is the average
#of between the 4 around it.
#subpixel[x,y] = pixel[x,y] pixel[x+1,y] pixel[x,y+1] pixel[x+1,y+1]
#we can do this first.
#then we draw 4 triangles that go around subpixel[x,y]
#triangle pixel[x,y]     pixel[x+1,y]   subpixel[x,y]
#triagnle pixel[x+1,y]   pixel[x+1,y+1] subpixel[x,y]
#triagnle pixel[x+1,y+1] pixel[x,y+1]   subpixel[x,y]
#triagnle pixel[x,y+1]   pixel[x,y]     subpixel[x,y]

width=32
height=16

im=Image.open(sys.argv[1]).convert('L').resize((width,height)) #load image as greyscale
#im=Image.open(sys.argv[1]).convert('L') #load image as greyscale
width=im.width
height=im.height
pixel_derp=im.load()
#print(dir(im))
#print(dir(im.getdata().getpixel(0,0)))
#quit()
#width=6
#height=6

def trianglize(x1,y1,x2,y2,xs,ys):
  x1%=width
  y1%=height
  x2%=width
  y2%=height
  xs%=width
  ys%=height

  yscale= 32 / height

  tx1=math.sin(x1 / (width / (2 * math.pi)) ) * (pixel[x1][y1] / (127 / math.pi))
  tz1=math.cos(x1 / (width / (2 * math.pi)) ) * (pixel[x1][y1] / (127 / math.pi))
  ty1=-((y1 * yscale) / 5) + 15

  tx2=math.sin(x2 / (width / (2 * math.pi)) ) * (pixel[x2][y2] / (127 / math.pi))
  tz2=math.cos(x2 / (width / (2 * math.pi)) ) * (pixel[x2][y2] / (127 / math.pi))
  ty2=-((y2 * yscale) / 5) + 15

#  xs=(xs * 2) + 1
  tx3=math.sin(xs / (width / (2 * math.pi)) ) * (subpixel[xs][ys] / (127 / math.pi))
  tz3=math.cos(xs / (width / (2 * math.pi)) ) * (subpixel[xs][ys] / (127 / math.pi))
  ty3=-(((ys * yscale) + 1) / 5) + 15
  print("test addshape 3 3 {} {} {}   {} {} {}   {} {} {}".format(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3))


#width=16
#height=16
pixel=[[0]*height for i in range(width)]
subpixel=[[0]*height for i in range(width)]
## initialize for testing:
for y in range(0,height):
  for x in range(0,width):
#    pixel[x][y]=random.randint(1,255)
#    pixel[x][y]=im.tobytes()[y * width + x]
    pixel[x][y]=pixel_derp[x,y]
#    pixel[x][y]=y * 20
#subpixel=pixel
#im.show()

for y in range(0,height-1):
  for x in range(0,width):
    subpixel[x][y] = ( pixel[x][y] + pixel[(x+1)%width][y] + pixel[x][(y+1)%height] + pixel[(x+1)%width][(y+1)%height] ) / 4
    trianglize(x,y,     x+1,y,   x,y)
    trianglize(x+1,y,   x+1,y+1, x,y)
    trianglize(x+1,y+1, x,y+1,   x,y)
    trianglize(x,y+1,   x,y,     x,y)
