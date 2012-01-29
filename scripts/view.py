#!/usr/bin/python

import sys
from PIL import Image, ImageDraw

map_vals = open(sys.argv[1], 'r').read().split()
w = int(map_vals[0])
h = int(map_vals[1])

img = Image.new("RGB", (w,h), (256,256,256))
xsize,ysize = w,h
draw = ImageDraw.Draw(img)

for i in range(0,h):
	for j in range(0,w):
		v = int(map_vals[i*h + j])
		draw.point((i,j), fill=(v,v,v))

del draw
img.save("out.png")
img.show()
