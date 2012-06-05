#!/bin/bash
for (( c=256; c<=2000; c+=100 ))
do

	echo ${c}
	./Default/terrain_generator --height ${c} --width ${c} >> perfromance_trace
done
