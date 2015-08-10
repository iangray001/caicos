#!/bin/bash

HOSTNAME=pegasus
HOSTNAMETEMP=/home/iang/vc707_autogen_pr/reconfig

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
PROJECT=caicosout

echo -e "\033[31m===========================================================================\033[0m"

#Delete and remake the target directory
echo "Cleaning remote directory $HOSTNAMETEMP/$PROJECT..."
ssh -q $HOSTNAME "rm -rf $HOSTNAMETEMP/$PROJECT"
ssh -q $HOSTNAME "mkdir $HOSTNAMETEMP/$PROJECT"

echo -n "Copying project $PROJECT..."
scp -q -r $DIR/* $HOSTNAME:$HOSTNAMETEMP/$PROJECT/
echo "done."

#Maybe build
if [ "build" = "$1" ]; then
	ssh -q $HOSTNAME "cd $HOSTNAMETEMP/$PROJECT/; . ~/xilinx14.3.sh; vivado_hls script.tcl"
	
	#Bring back the report
	scp -q $HOSTNAME:$HOSTNAMETEMP/$PROJECT/prj/solution1/syn/report/hls_csynth.rpt .
fi
