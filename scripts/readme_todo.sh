#!/bin/bash

./../src/a.out -h > ../README

echo TODO: > ../TODO

while read line
do 

l=(`echo $line | tr ' ' ' '`)
if [ "${l[0]}" == '//TODO:' ]
	then
	echo "-" ${l[@]:1} >> ../TODO
fi
done < "../src/terrain_generator.cpp"
