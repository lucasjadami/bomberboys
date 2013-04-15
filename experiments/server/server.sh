#!/bin/bash
#Params: server executable (a bin folder must be created on the same directory)

if [ ! -d "output" ]; then
	mkdir output
fi

runTime=10

echo "Running experiment for $1"

for executionId in `seq 1 10`; do

	echo "Current execution is $executionId, press RETURN (any key) to start"
	read -n 1
	echo "Started execution, ending in $runTime seconds"
	
	./bin/$1 > output/$1-$executionId.server &
	pId=$!
	
	sleep $runTime
	# Send SIGINT (2, CTRL + C) to the server to terminate properly.
	kill -2 $pId
	
	cat /proc/loadavg > output/$1-$executionId.cpu
done 

