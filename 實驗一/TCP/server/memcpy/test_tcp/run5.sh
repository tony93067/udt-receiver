#!/bin/bash
# Program:
#	5 BK TCP Server
Method=("cubic" "bbr")

BK=5
export PATH
for me in ${Method[@]}
do
	
	./downlink_run5.sh
	./server $BK $me
	killall -9 background_server_downlink
	sleep 10
	ps
done

