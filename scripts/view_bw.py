#!/usr/bin/python

import sys
from PIL import Image, ImageDraw

map_vals = open(sys.argv[1], 'r').read().split()
w = int(map_vals[0])
h = int(map_vals[1])
max_val = max([int(x) for i,x in enumerate(map_vals) if i > 1])

img = Image.new("RGB", (w,h), (256,256,256))
xsize,ysize = w,h
draw = ImageDraw.Draw(img)

for i in range(0,h):
	for j in range(2,w):
		v = int(map_vals[i*h + j])
		v = int(float(v)/float(max_val) * 255)

		if(v>150):
			draw.point((i,j), fill=(v,v,v))
		else:
			draw.point((i,j), fill=(0))
		

del draw
img.save("out.png")
img.show()
