#!/bin/bash

HOSTNAME=pegasus
TARGETDIR=/home/iang/caicosvc707
XILINXSCRIPT=/home/iang/xilinx.sh

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
HLSPROJECT=caicos

echo -e "\033[31m===========================================================================\033[0m"

clean_dir () {
	echo $1
	if [ -n "$1" ]; then
		ssh -q $HOSTNAME "rm -rf $1"
		ssh -q $HOSTNAME "mkdir -p $1"
	else
		echo "clean_dir called with empty argument"
		exit 1
	fi
}

copyall () {
	echo -n "Copying project..."
	clean_dir $TARGETDIR
	scp -q -r $DIR $HOSTNAME:$TARGETDIR/
	echo "done."
}

case "$1" in 
	'copy' )
		copyall
	;;

	'testhls' )
		REMOTEPROJDIR=$TARGETDIR/hardware/reconfig/$HLSPROJECT
		clean_dir $REMOTEPROJDIR
		scp -q -r $DIR/hardware/reconfig/$HLSPROJECT $HOSTNAME:$TARGETDIR/hardware/reconfig/
		ssh -q $HOSTNAME "cd $REMOTEPROJDIR; . $XILINXSCRIPT; vivado_hls autobuild.tcl"
		#Bring back the synthesis report
		scp -q $HOSTNAME:$REMOTEPROJDIR/prj/solution1/syn/report/hls_csynth.rpt $DIR/
	;;

	'' ) 
		echo "Usage: $0 [ copy | testhls ]"
	;;
esac
