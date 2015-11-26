#!/bin/bash

build_hls() {
	if [ -d ./reconfig/$1 ]; then
		cd ./reconfig/$1
		vivado_hls autobuild.tcl || exit 1;

		cp prj/solution1/impl/ip/hls.dcp ./$1_synth.dcp
		unzip ${1}_synth.dcp -d ${1}_unpack
		sed -i s/create_clock/\#create_clock/ ${1}_unpack/hls.xdc
		zip -j ${1}_synth_noclock.dcp ${1}_unpack/*

		cd ../../

		vivado -mode batch -source make_reconfig.tcl -tclargs reconfig ./reconfig/${1}/${1}_synth_noclock.dcp
	fi
}

case "$1" in
	'all' )
		for d in ./reconfig/*; do
			build_hls $d
		done
	;;

	'single' )
		build_hls $2
	;;

	* )
		echo "Usage: $0 [ all | single <reconfig_name> ]"
	;;
