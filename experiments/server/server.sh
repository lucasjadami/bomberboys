#!/bin/bash
# Params: 
#	$1: server executable (a bin folder must be created on the same directory)
#	$2: clients script

if [ ! -d "output" ]; then
	mkdir output
fi

startupTime=5
runTime=65
waitTime=30
finalizeTime=10

echo "Running experiment for $1"

for executionId in `seq 1 10`; do

	echo "Current execution is $executionId"
	
	echo "Starting server"
	./bin/$1 > output/$1-$executionId.server &
	pId=$!
	
	sleep $startupTime
	echo "Launching clients"
	../client/execute_ssh $2 &
	
	sleep $runTime
	# Server sent shutdown, save CPU usage.
	echo "Shutdown, saving CPU usage"
	cat /proc/loadavg > output/$1-$executionId.cpu
	
	# Wait for clients to finalize.
	sleep $waitTime
	
	# Send SIGINT (2, CTRL + C) to the server to terminate properly.
	echo "Terminating server"
	kill -2 $pId
	sleep $finalizeTime
	
done 

