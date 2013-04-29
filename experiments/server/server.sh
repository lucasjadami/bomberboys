#!/bin/bash
# Params: 
#	$1: server executable (a bin folder must be created on the same directory)
#	$2: clients script

if [ ! -d "output" ]; then
	mkdir output
fi

startupTime=5
runTime=65
totalTime=70
waitTime=90
finalizeTime=5

echo "Running experiment for $1"

for executionId in `seq 1 10`; do

	echo "Current execution is $executionId"
	
	echo "Starting server"
	./bin/$1 > output/$1-$executionId.server &
	pId=$!
	
	echo "Launching vmstat and nstat"
	vmstat $totalTime 2 > output/$1-$executionId.cpu &
	nstat > /dev/null
	
	echo "Waiting $startupTime seconds (startup)"
	sleep $startupTime
	echo "Launching clients"
	../client/execute_ssh $2 &
	
	echo "Waiting $runTime seconds (run time)"
	sleep $runTime
	# Server sent shutdown.
	echo "Shutdown sent"
	
	echo "Waiting $waitTime seconds (clients finalizing)"
	# Wait for clients to finalize.
	sleep $waitTime
	
	# Send SIGINT (2, CTRL + C) to the server to terminate properly.
	echo "Terminating server"
	kill -2 $pId
	echo "Waiting $finalizeTime seconds (server finalizing)"
	sleep $finalizeTime
	
	# Save net statistics.
	nstat > output/$1-$executionId.net
	
done 

