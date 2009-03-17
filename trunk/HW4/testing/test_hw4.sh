#!/bin/bash

echo "Making this script run with FIFO (prio=99)"
chrt -r -p 50 $$

## Changing bash prio, so that we we can kill jobs if we need to

echo "Switching current bash to FIFO (prio=99)"
bash_pid=`ps -opid,comm | grep bash | nawk '{ print $1 }'`
chrt -f -p 99 ${bash_pid}

## This is the kernel thread that services interrupts for the console
## (and possibly network and io).  If this isn't prioritized, we can't see
## anything on the console

echo "Switching event handling kernel thread to FIFO (prio=99)"
event_pid=`ps -e -opid,comm | grep events | nawk '{ print $1 }'`
chrt -f -p 99 ${event_pid}

for u in alice bob carol
do
	
	echo "Adding user ${u}..."
	useradd ${u}
	
	sleep 1
	
	echo "Submitting test_hw4 to the background as user ${u}..."
	sudo -u ${u} -b ./inf-loop &
	
	test_pid=`ps -u ${u} -opid,comm | grep inf-loop | nawk '{ print $1 }'`
	
	## TODO: We can't use chrt, we should have our own C program that takes
	## a PID as argument and changes the policy to UWRR
	echo "Switching ${test_pid} to UWRR (prio=50)"
	chrt -r -p 50 ${test_pid}
	
	sleep 1

done

echo "Launching top..."
chrt -f 99 top

exit
