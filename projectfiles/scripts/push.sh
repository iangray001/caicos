#!/bin/bash
BUILDSERVER=pegasus
TESTSERVER=zynqhome
BITFILELOCATION=/boot/base.bit

BUILD_TARGETDIR=/home/iang/caicosvc707
XILINXSCRIPT=/home/iang/xilinx.sh

BUILD_TARGETDIRBASE=${BUILD_TARGETDIR##*/}
HLSPROJECT=caicos
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
COL="\033[32m"
RESET="\033[0m"

echo -e "\033[31m===========================================================================$RESET"

clean_dir () {
	if [ -n "$1" ]; then
		echo -e "${COL}Cleaning remote directory$RESET $BUILDSERVER:$1"
		ssh -q $BUILDSERVER "rm -rf $1"
		ssh -q $BUILDSERVER "mkdir -p $1"
	else
		echo "clean_dir called with empty argument"; exit 1
	fi
}

copyhls () {
	REMOTEPROJDIR=$BUILD_TARGETDIR/hardware/reconfig/$HLSPROJECT
	clean_dir $REMOTEPROJDIR
	echo -e "${COL}Copying$RESET $DIR/hardware/reconfig/$HLSPROJECT ${COL}to$RESET $BUILDSERVER:$BUILD_TARGETDIR/hardware/reconfig/"
	scp -q -r $DIR/hardware/reconfig/$HLSPROJECT $BUILDSERVER:$BUILD_TARGETDIR/hardware/reconfig/
}

case "$1" in 
	'clean' )
		clean_dir $BUILD_TARGETDIR 
	;;

	'copy' )
		echo -e "Copying ${COL}$DIR/* $RESET to ${COL} $BUILDSERVER:$BUILD_TARGETDIR/ $RESET"
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
		#Rebuild the bitfile for the project $HLSPROJECT
		copyhls
		ssh -q $BUILDSERVER ". $XILINXSCRIPT; cd $BUILD_TARGETDIR/hardware; make_reconfig.sh single $HLSPROJECT"
	;;
		
	'bit' )
		if [ -n "$2" ]; then
			echo -e "Copying ${COL}$2 $RESET to ${COL} $TESTSERVER:$BITFILELOCATION $RESET"
			scp $BUILDSERVER:$BUILD_TARGETDIR/hardware/assemble/bitstream/$2 __testbit
			scp __testbit $TESTSERVER:$BITFILELOCATION
			rm __testbit
		else
			echo "Bitfiles on build server:"
			ssh -q $BUILDSERVER "cd $BUILD_TARGETDIR/hardware/assemble/bitstream/; ls *.bit"
		fi
	;;

	'' ) 
		echo -e "Usage: $0 [ clean | copy | hls | bit ]"
	;;
esac
