#!/bin/bash
# Program:
#	execute 5 server at the time
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH
i=1
p=8888
while [ "$i" -le "5" ]
do
echo "Server" $i "generate"
./background_server_uplink $p "cubic" &
sleep 1
i=$(($i+1))
p=$(($p+1))
done
echo "Background Server Create Finish"

