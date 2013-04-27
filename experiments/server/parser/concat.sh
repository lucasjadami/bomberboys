#!/bin/bash
# Params: 
#	$1: server executable

for executionId in `seq 1 10`; do

	cat ../output/$1-$executionId.server >> $1.server.concat
	cat ../output/$1-$executionId.cpu >> $1.cpu.concat
	cat ../output/$1-$executionId.net >> $1.net.concat

done
