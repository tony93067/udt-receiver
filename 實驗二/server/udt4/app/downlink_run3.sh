#!/bin/bash
# Program:
#	3 BK TCP Server
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
LB_PATH=/home/tony/論文code/論文code/實驗二/server/udt4/src
TCP_PATH=/home/tony/論文code/論文code/實驗一/TCP/server/memcpy/test_tcp
UDT_PATH=/home/tony/論文code/論文code/實驗二/server/udt4/app
MSS=("1500" "1250" "1000" "750" "500" "250" "100")
MSS1=("1500")
BK=3
export PATH
for (( c=1; c<=3; c++ ))
do
	for str in ${MSS1[@]}
	do
		cd $TCP_PATH
		./downlink_run3.sh
		export LD_LIBRARY_PATH=$LB_PATH
		cd $UDT_PATH
		./udtserver 5000 $str 1 $c $BK
		killall -9 background_server_downlink
		sleep 10
		date -R
	done
	
done

