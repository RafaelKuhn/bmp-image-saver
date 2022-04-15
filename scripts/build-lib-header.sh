#!/bin/bash

# this will create a file at arg $1, combining all files
# from args $2 forwards, striping them of anything between
# lines that have #ifdef DEBUG and lines that have #endif

LIB_PATH=$1

cleanup() {
	echo "got arg:: \n "
	echo "$1"

}

for header in "${@:2}"; do {
	printf "[bash] cleaning up header \"$header\"\n"

	# if line has '#ifdef DEBUG', stop reading until finds line with '#endif'
	IS_READING_IFDEF=0
	
	header_filename=$(basename $header)
	header_filename="$LIB_PATH/$header_filename"
	
	# printf "#pragma once\n\n" > "$header_filename"
	printf "// original location: $header:\n\n" > "$header_filename"
	
	# we need to do this crap becaus bash is retarded and skips last line of read file
	while IFS= read -r line || [ -n "$line" ]; do {
		# check for '#ifdef DEBUG' in current line
		if [[ "$line" == *\#ifdef\ DEBUG* ]]; then
			IS_READING_IFDEF=1
		fi

		# check if not reading ifdef debug
		if [ $IS_READING_IFDEF -eq 0 ]; then
			printf "$line" >> "$header_filename"
		fi

		# check if line does not contain '#pragma once'
		## if ! [[ "$line" == *\#pragma\ once* ]]; then
		
		# check if line does not contain '#include "'
		## if ! [[ "$line" == *\#include\ \"* ]]; then

		# check for #endif in current line
		if [[ "$line" == *\#endif* ]]; then
			IS_READING_IFDEF=0
		fi

	} done < "$header"

} done

printf "[bash] library headers created in ${1}\n"