#!/bin/bash

PROJECT=${PWD##*/}
HOSTNAME=pegasus
HOSTNAMETEMP=/home/iang/vivado

echo -e "\033[31m===========================================================================\033[0m"
echo -e "\033[31m===========================================================================\033[0m"

#Make the target directory
ssh -q $HOSTNAME "mkdir $HOSTNAMETEMP/$PROJECT"

echo -n "Copying project $PROJECT..."
scp -q -r * $HOSTNAME:$HOSTNAMETEMP/$PROJECT/
echo "done."

#Build the thing
ssh -q $HOSTNAME "cd $HOSTNAMETEMP/$PROJECT/; . ~/xilinx14.3.sh; vivado_hls script.tcl"

#Bring back the report
scp -q $HOSTNAME:$HOSTNAMETEMP/$PROJECT/prj/solution1/syn/report/hls_csynth.rpt .
