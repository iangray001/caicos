#!/bin/bash

BUILDSERVER=pegasus
TESTSERVER=lunix

BUILD_TARGETDIR=/home/iang/caicosvc707
XILINXSCRIPT=/home/iang/xilinx.sh

BUILD_TARGETDIRBASE=${BUILD_TARGETDIR##*/}
HLSPROJECT=caicos
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

echo -e "\033[31m===========================================================================\033[0m"

clean_dir () {
	echo $1
	if [ -n "$1" ]; then
		ssh -q $BUILDSERVER "rm -rf $1"
		ssh -q $BUILDSERVER "mkdir -p $1"
	else
		echo "clean_dir called with empty argument"; exit 1
	fi
}

copyhls () {
	REMOTEPROJDIR=$BUILD_TARGETDIR/hardware/reconfig/$HLSPROJECT
	clean_dir $REMOTEPROJDIR
	scp -q -r $DIR/hardware/reconfig/$HLSPROJECT $BUILDSERVER:$BUILD_TARGETDIR/hardware/reconfig/
}


case "$1" in 
	'clean' )
		echo "Cleaning remote directory $BUILD_TARGETDIR..."
		clean_dir $BUILD_TARGETDIR 
	;;

	'copy' )
		echo "Copying project..."
		ssh -q $BUILDSERVER "mkdir -p $BUILD_TARGETDIR"
		scp -q -r $DIR/* $BUILDSERVER:$BUILD_TARGETDIR/
		echo "done."
	;;

	'testhls' )
		#Build the hls project and bring back the synthesis report
		copyhls
		ssh -q $BUILDSERVER "cd $REMOTEPROJDIR; . $XILINXSCRIPT; vivado_hls autobuild.tcl"
		scp -q $BUILDSERVER:$REMOTEPROJDIR/prj/solution1/syn/report/hls_csynth.rpt $DIR/
	;;
	
	'hls' )
		#Rebuild the bitfile for the project $HLSPROJECT, and copy the bitfile to the testserver
		copyhls
		ssh -q $BUILDSERVER "$BUILD_TARGETDIR/hardware/make_reconfig single $HLSPROJECT"
		ssh -q $BUILDSERVER "scp $BUILD_TARGETDIR/hardware/assemble/bitstream/$HLSPROJECT.bit $TESTSERVER:"
	;;
	
	'software' )
		ssh -q $TESTSERVER "mkdir -p $BUILD_TARGETDIRBASE/software"
		scp -q -r $DIR/software/* $TESTSERVER:$BUILD_TARGETDIRBASE/software
		ssh -q $TESTSERVER "cd $BUILD_TARGETDIRBASE/software ; make"
	;;
	
	'testbit' )
		ssh -q $BUILDSERVER "scp $BUILD_TARGETDIR/hardware/assemble/bitstream/$2 $TESTSERVER:$BUILD_TARGETDIRBASE/"
	;;

	'' ) 
		echo "Usage: $0 [ clean | copy | software | testhls | testbit ]"
	;;
esac
