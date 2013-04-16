#!/bin/bash
# Params: 
#	$1: server executable (a bin folder must be created on the same directory)
#	$2: clients script

if [ ! -d "output" ]; then
	mkdir output
fi

startupTime=5
runTime=70
waitTime=30

echo "Running experiment for $1"

for executionId in `seq 1 10`; do

	echo "Current execution is $executionId, press RETURN (any key) to start"
	read -n 1
	echo "Started execution, ending in $runTime seconds"
	
	./bin/$1 > output/$1-$executionId.server &
	pId=$!
	
	sleep $startupTime
	../client/$2 &
	
	sleep $runTime - $startupTime
	# Server sent shutdown, save CPU usage.
	cat /proc/loadavg > output/$1-$executionId.cpu
	
	# Wait for clients to finalize.
	sleep $waitTime
	
	# Send SIGINT (2, CTRL + C) to the server to terminate properly.
	kill -2 $pId
	
done 

