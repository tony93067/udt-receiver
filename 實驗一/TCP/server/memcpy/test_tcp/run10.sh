#!/bin/bash
# Program:
#	10 BK TCP Server

Method=("cubic" "bbr")

BK=10
export PATH
for me in ${Method[@]}
do
	
	./downlink_run10.sh
	./server $BK $me
	killall -9 background_server_downlink
	sleep 10
	ps
done

