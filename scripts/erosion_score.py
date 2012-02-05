#!/usr/bin/python
import sys, math

print "Reading " + sys.argv[1]
map_vals = open(sys.argv[1], 'r').read().split()
w = int(map_vals[0])
h = int(map_vals[1])

print "Map " + sys.argv[1] + " " + str(w) + " x " + str(h)
total_v = 0
max_v = int(map_vals[2])
min_v = int(map_vals[2])
for i in range(0,h):
        for j in range(2,w):
                v = int(map_vals[i*h + j])
		if(max_v<v): max_v = v
		if(min_v>v): min_v = v
		total_v += v

avg_v = total_v / (w*h)

for i in range(0,h):
        for j in range(2,w):
                v = int(map_vals[i*h + j])
		
		total_v += pow((v-avg_v),2)

dv_v = int(math.sqrt(total_v / (w*h)))

print "min value " + str(min_v)
print "max value " + str(max_v)
print "avg value " + str(avg_v)
print "dev value " + str(dv_v)
print "dev/avg   " + str(float(dv_v)/float(avg_v))

		
