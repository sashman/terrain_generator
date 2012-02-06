#!/bin/bash

for (( i=0;i<25;i++ )) 
do
step=0.01
p=$(echo "scale=2; $i*$step+0.1" | bc)
e=$(echo "scale=2; $i/25 * 10" | bc)
e=$( printf "%.0f" $e )

#p=$(($i * $step))

l=$( printf "%02d" $i) 
echo "plate ${p} erosion ${e}"
./../Default/terrain_generator --height 512 --width 512 --plate ${p}  --erosion ${e} > terrain.txt && ./view_colour.py terrain.txt
cp out.png ../examples/out"${l}".png
done
