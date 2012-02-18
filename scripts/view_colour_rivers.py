#!/usr/bin/python

import sys


libs_needed = []
try:
	import PIL
except Exception:
	libs_needed.append("python-imaging")

try:
	import lxml
except Exception:
	libs_needed.append("python-lxml")

if(len(libs_needed)>0):
	print "Required libs: "
	print ",".join(libs_needed)
	exit()

from PIL import Image, ImageDraw, ImageFont
from lxml import etree


#terrain data
map_vals = open(sys.argv[1], 'r').read().split()
w = int(map_vals[0])
h = int(map_vals[1])
max_val = max([int(x) for i,x in enumerate(map_vals) if i > 1])


#river data
river_string = open(sys.argv[2], 'r').read()

root = etree.fromstring(river_string)
river_elems = root.findall("river/river_point")
rivers = []


#draw image
img = Image.new("RGB", (w,h), (256,256,256))
xsize,ysize = w,h
draw = ImageDraw.Draw(img)

for i in range(0,h):
	for j in range(2,w):
		v = int(map_vals[i*h + j])
		v = int(float(v)/float(max_val) * 255)
		
		if(v<150):
			draw.point((i,j), fill=(0,0,100+v))
		elif(v<160):
			draw.point((i,j), fill=(v,v,0))
		elif(v<210):
			draw.point((i,j), fill=(0,200-(v/2),0))
		else:
			draw.point((i,j), fill=(v-60,v-10,v-60))


i = 0
for re in river_elems:
	x_pos = re.get("y")
	y_pos = re.get("x")
	

	blue = float(i)/float(len(river_elems)) * 240
	blue = 240
	draw.rectangle((int(x_pos), int(y_pos), int(x_pos), int(y_pos)), fill=(0,0,int(blue)))
	i+=1
		
del draw
img.save("out.png")
img.show()


