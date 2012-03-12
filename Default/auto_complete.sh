#!/bin/bash
# Source this file   
#     . auto_complete.sh
# to enable autocompletion for terrain_generator
# Bilal Hussain

function _list_terrain_generator(){
	echo "--help
		  --verbose
		--height
		--width
		--rough
		--seed
		--offset
		--plate
		--erosion
		--negative
		--standard
		--graphical
		--xml
		"
}

function _comp_terrain_generator(){
	local curw
	COMPREPLY=()
	curw=${COMP_WORDS[COMP_CWORD]}
	COMPREPLY=($(compgen -W '`_list_terrain_generator`' -- $curw))
	return 0
}
shopt -s progcomp
complete -F _comp_terrain_generator terrain_generator 
