#!/bin/bash
# Params: 
#	$1: server executable
#	$2: extension type

for executionId in `seq 1 10`; do

	cat ../output/$1-$executionId.$2 >> $1.$2.concat

done
