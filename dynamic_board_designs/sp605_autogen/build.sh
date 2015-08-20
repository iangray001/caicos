#!/bin/bash

for i in {0..9}; do
    if [ -d "./$i" ]; then
        sed "s/VERSIONNUM/1.0${i}.a/" < autobuild.tcl.tpl > $i/autobuild.tcl;
        cd $i;
        vivado_hls -i -f autobuild.tcl || exit 1;
        cd ../;
    fi
done

# Now copy over the base project
rm -rf hw
cp -R xps_base hw

# Sadly, Xilinx kinda stopped supporting the pcore flow properly
# This means that the above bit generates a Vivado IP, then we
# need to manually convert it back to pcore format.
# Fun...
for i in {0..9}; do
    if [ -d "./$i" ]; then
        # Make the pcore folder structure
        mkdir ./hw/pcores/hls_v1_0${i}_a
        mkdir -p ./hw/pcores/hls_v1_0${i}_a/synhdl/verilog
        mkdir ./hw/pcores/hls_v1_0${i}_a/data

        # Copy out all of the "new" HDL
        cp ./$i/juniperHLS/solution1/impl/ip/hdl/verilog/* ./hw/pcores/hls_v1_0${i}_a/synhdl/verilog

        # Move into the pcore data folder for further processing
        cd ./hw/pcores/hls_v1_0${i}_a/data
        rm -f hls_v2_1_0.pao

        for f in ../synhdl/verilog/*; do
            echo "synlib hls_v1_0${i}_a `basename $f .v` verilog" >> hls_v2_1_0.pao
        done

        # Finally, copy in the MPD
        cp ../../../../pcore_mpd_template.mpd ./hls_v2_1_0.mpd
        cd ../../../../
    fi
done

# Now need to edit the new core into the MPD
for i in {0..9}; do
    if [ -d "./$i" ]; then
        cat >> hw/system.mhs <<EOF
BEGIN hls
 PARAMETER INSTANCE = hls_$i
 PARAMETER HW_VER = 1.0${i}.a
 PARAMETER C_S_AXI_AXILITES_BASEADDR = 0x2000${i}000
 PARAMETER C_S_AXI_AXILITES_HIGHADDR = 0x2000${i}fff
 PARAMETER C_INTERCONNECT_S_AXI_AXILITES_MASTERS = mp_to_periph.M_AXI
 BUS_INTERFACE M_AXI_GMEM32 = mem_interconnect
 BUS_INTERFACE S_AXI_AXILITES = periph_interconnect
 PORT jamaica_syscall_V = hls_${i}_jamaica_syscall_V
 PORT ap_clk = axi_pcie_0_axi_aclk_out
END
EOF
        
        sed -i "/PORT Intr.*/ s/$/ \& hls_${i}_jamaica_syscall_V/" hw/system.mhs
        sed -i "/PARAMETER C_INTERCONNECT_S0_AXI_MASTERS.*/ s/$/ \& hls_${i}.M_AXI_GMEM32/" hw/system.mhs
    fi
done

# Build it!
cd hw
xps -nw -scr ../xpsbuild.tcl system.xmp
cd ../
