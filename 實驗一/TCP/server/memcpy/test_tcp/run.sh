#!/bin/bash
# Program:
#	0 BK TCP Server
Method=("cubic" "bbr")

BK=0
export PATH
for me in ${Method[@]}
do
	./server $BK $me
	sleep 10
	ps
done

