#!/bin/sh

for i in `seq 1 10`; do
	./$1 > output/$ALIAS($i).server
	cat /proc/loadavg > output/$1($i).cpu
done 
