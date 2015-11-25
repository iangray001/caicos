#!/bin/bash

# Sadly, we need to do some admin before we attempt to call into the Tcl scripts.
# This involves ensuring that the base IP core has been built.
cd base
./build_hls.sh
cd ../

vivado -mode batch -source build_base.tcl
