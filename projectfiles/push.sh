#!/bin/bash

PROJECT=${PWD##*/}
SOLUTION=solution1
HOSTNAME=pegasus
HOSTNAMETEMP=/home/iang/vivado

echo -e "\033[31m===========================================================================\033[0m"
echo -e "\033[31m===========================================================================\033[0m"

echo -n "Copying project $PROJECT..."
scp -q -r * $HOSTNAME:$HOSTNAMETEMP/$PROJECT/
echo "done."

#Build the thing
ssh -q $HOSTNAME "cd $HOSTNAMETEMP; . ~/xilinx14.3.sh; vivado_hls $PROJECT/$SOLUTION/script.tcl"

#Bring back the report
scp -q $HOSTNAME:$HOSTNAMETEMP/$PROJECT/$SOLUTION/syn/report/hls_csynth.rpt .
