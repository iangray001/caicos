#!/bin/bash

vivado_hls autobuild.tcl
cp juniperHLS/solution1/impl/ip/hls.dcp ./
unzip hls.dcp -d hls_unpack
sed -i s/create_clock/\#create_clock/ hls_unpack/hls.xdc
zip -j hls_noclock.dcp hls_unpack/*
rm -rf hls_unpack
