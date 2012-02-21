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

class Settlement():
	def __init__(self, x, y, name):
		self.x = x
		self.y = y
		self.name = name

#terrain data
map_vals = open(sys.argv[1], 'r').read().split()
w = int(map_vals[0])
h = int(map_vals[1])
max_val = max([int(x) for i,x in enumerate(map_vals) if i > 1])


#settelement data
settlements_string = open("settlements.txt", 'r').read()

root = etree.fromstring(settlements_string)
set_elems = root.findall("settlement")
settlements = []
for se in set_elems:
	s = Settlement(se.get("y"), se.get("x"), se.findtext("name"))
	settlements.append(s)

#river data
river_string = open("rivers.txt", 'r').read()

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

# use a truetype font
font = ImageFont.truetype("VeniceClassic.ttf", 18)

for s in settlements:
	v = int(map_vals[int(s.x)*h + int(s.y)])
	v = int(float(v)/float(max_val) * 255)

	draw.rectangle((int(s.x)-2, int(s.y)-2, int(s.x)+2, int(s.y)+2), fill=(0))

	x_pos = int(s.x) - 10
	y_pos = int(s.y) + 5

	if(int(s.y) > h-20):
		x_pos = int(s.x)+6
		y_pos = int(s.y)-10
	if(int(s.x) > w-100):
		x_pos = int(s.x)-80	

	draw.text((x_pos, y_pos), s.name, fill=(0), font=font)
		
del draw
img.save("out.png")
img.show()


