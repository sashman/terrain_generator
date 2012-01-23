#!/bin/bash

./../src/a.out -h > ../README

echo TODO: > ../TODO
echo "" >> ../README
echo "" >> ../README
echo "TODO:" >> ../README

while read line
do 

l=(`echo $line | tr ' ' ' '`)
if [ "${l[0]}" == '//TODO:' ]
	then
	echo "-" ${l[@]:1} >> ../TODO
	echo "-" ${l[@]:1} >> ../README
fi
done < "../src/terrain_generator.cpp"
