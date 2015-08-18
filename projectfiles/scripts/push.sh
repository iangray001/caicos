#!/bin/bash

HOSTNAME=pegasus
TARGETDIR=/home/iang/caicosvc707
XILINXSCRIPT=/home/iang/xilinx.sh
TESTSERVER=lunix

HLSPROJECT=caicos
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

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

cleandir () {
	echo "Cleaning remote directory $TARGETDIR..."
	clean_dir $TARGETDIR 
}

copyall () {
	echo "Copying project..."
	ssh -q $HOSTNAME "mkdir -p $TARGETDIR"
	scp -q -r $DIR/* $HOSTNAME:$TARGETDIR/
	echo "done."
}

copyhls () {
	REMOTEPROJDIR=$TARGETDIR/hardware/reconfig/$HLSPROJECT
	clean_dir $REMOTEPROJDIR
	scp -q -r $DIR/hardware/reconfig/$HLSPROJECT $HOSTNAME:$TARGETDIR/hardware/reconfig/
}


case "$1" in 
	'clean' )
		cleandir
	;;

	'copy' )
		copyall
	;;

	'testhls' )
		copyhls
		ssh -q $HOSTNAME "cd $REMOTEPROJDIR; . $XILINXSCRIPT; vivado_hls autobuild.tcl"
		#Bring back the synthesis report
		scp -q $HOSTNAME:$REMOTEPROJDIR/prj/solution1/syn/report/hls_csynth.rpt $DIR/
	;;
	
	'hls' )
		copyhls
		ssh -q $HOSTNAME "$TARGETDIR/hardware/make_reconfig single $HLSPROJECT"
		ssh -q $HOSTNAME "scp $TARGETDIR/hardware/assemble/bitstream/$HLSPROJECT.bit $TESTSERVER:"
	;;
	
	'testbit' )
		ssh -q $HOSTNAME "scp $TARGETDIR/hardware/assemble/bitstream/$2 $TESTSERVER:"
	;;

	'' ) 
		echo "Usage: $0 [ clean | copy | testhls | testbit ]"
	;;
esac
