#!/bin/bash
# Program:
#	3 BK TCP Server
Method=("cubic" "bbr")

BK=3
export PATH
for me in ${Method[@]}
do
	./downlink_run3.sh
	./server $BK $me
	killall -9 background_server_downlink
	sleep 10
	ps
done

