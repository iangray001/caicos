#!/bin/bash

# I give up doing this in Make.
# This doesn't do dependency management or proper error checking. Deal with it.
rm -rf ./iprepo
mkdir ./iprepo

NUMPRJ=4
for i in {0..9}; do
    if [ -d "./base/$i" ]; then
        #sed "s/VERSIONNUM/1.${i}/" < autobuild.tcl.tpl > ./base/$i/autobuild.tcl;
        #cd ./base/$i;
        #vivado_hls -i -f autobuild.tcl || exit 1;
        #cd ../../;

        #cp -R ./base/${i}/juniperHLS/solution1/impl/ip ./iprepo/${i}_core
        #cp logo.png ./iprepo/${i}_core/misc
        #touch ./iprepo/${i}_core/component.xml
        #((NUMPRJ++))
        echo "LOL"
    fi
done

for d in ./reconfig/*; do
    if [ -d $d ]; then
        sed "s/VERSIONNUM/1.0/" < autobuild_dcp.tcl.tpl > ./$d/autobuild.tcl;
        cd $d;
        vivado_hls -i -f autobuild.tcl || exit 1;
        cd ../../;
    fi
done

# Now create the Vivado projects.
rm -rf zc706_hw
rm generated.xdc
vivado -mode batch -source vivado_project.tcl
vivado -mode batch -source vivado_bd_base.tcl zc706_hw/zc706_hw.xpr

# Now create the board TCL
cat > ./vivado_bd_cores.tcl <<EOF
    open_bd_design [get_property directory [current_project]]/zc706_hw.srcs/sources_1/bd/juniper_board/juniper_board.bd

    # Resize the AXI buses
    set_property -dict [ list CONFIG.NUM_SI {$((NUMPRJ+1))} ] [get_bd_cells axi_interconnect_0]
    set_property -dict [ list CONFIG.NUM_MI {$((NUMPRJ+2))} ] [get_bd_cells axi_interconnect_2]
    set_property -dict [ list CONFIG.C_GPIO_WIDTH {$((NUMPRJ))}  ] [get_bd_cells axi_gpio_0]
EOF

# And sub into the MHS file
# I'd call this bit completely and utterly fragile, but that's giving the impression that it might not break...
for i in {0..3}; do
#    if [ -d "./base/$i" ]; then
        # Create the cores...
        cat >> ./vivado_bd_cores.tcl <<EOF
set top_${i} [ create_bd_cell -type ip -vlnv york.ac.uk:hls:juniper_hls_toplevel:1.0 top_${i} ]
connect_bd_intf_net -intf_net axi_interconnect_2_M0$((i+2))_AXI [get_bd_intf_pins axi_interconnect_2/M0$((i+2))_AXI] [get_bd_intf_pins top_${i}/s_axi_AXILiteS]
connect_bd_intf_net -intf_net top_${i}_m_axi_mem [get_bd_intf_pins axi_interconnect_0/S0$((i+1))_AXI] [get_bd_intf_pins top_${i}/m_axi_mem]
connect_bd_net -net mig_7series_0_ui_addn_clk_0 [get_bd_pins axi_interconnect_0/S0$((i+1))_ACLK] [get_bd_pins axi_interconnect_2/M0$((i+2))_ACLK] [get_bd_pins top_${i}/ap_clk]
connect_bd_net -net proc_sys_reset_0_interconnect_aresetn [get_bd_pins axi_interconnect_0/S0$((i+1))_aresetn] [get_bd_pins axi_interconnect_2/M0$((i+2))_aresetn]
connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins top_${i}/ap_rst_n]
create_bd_addr_seg -range 0x20000000 -offset 0x0 [get_bd_addr_spaces top_${i}/m_axi_mem] [get_bd_addr_segs mig_7series_0/memmap/memaddr] SEG_mig_7series_${i}_memaddr
create_bd_addr_seg -range 0x10000 -offset 0x60${i}00000 [get_bd_addr_spaces axi_pcie_0/M_AXI] [get_bd_addr_segs top_${i}/s_axi_AXILiteS/reg0] SEG_top_${i}_Reg
create_bd_addr_seg -range 0x10000 -offset 0x60${i}00000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs top_${i}/s_axi_AXILiteS/reg0] SEG_top_${i}_Reg_2

# Create instance: xlslice_${i}, and set properties
set xlslice_${i} [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_${i} ]
set_property -dict [ list CONFIG.DIN_WIDTH {${NUMPRJ}} CONFIG.DIN_FROM ${i} CONFIG.DIN_TO ${i} ] \$xlslice_${i}
connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins axi_gpio_0/gpio_io_o] [get_bd_pins xlslice_${i}/Din]
connect_bd_net -net xlslice_${i}_Dout [get_bd_pins top_${i}/hold_outputs] [get_bd_pins xlslice_${i}/Dout]
EOF
#    fi
done

cat >> ./vivado_bd_cores.tcl <<EOF
save_bd_design

make_wrapper -files [get_files [get_property directory [current_project]]/zc706_hw.srcs/sources_1/bd/juniper_board/juniper_board.bd] -top
add_files -norecurse [get_property directory [current_project]]/zc706_hw.srcs/sources_1/bd/juniper_board/hdl/juniper_board_wrapper.v
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1
set_property top juniper_board_wrapper [current_fileset]

reset_run synth_1
launch_runs synth_1
wait_on_run synth_1
open_run synth_1
write_xdc generated.xdc

EOF
vivado -mode batch -source vivado_bd_cores.tcl zc706_hw/zc706_hw.xpr
./make_reconfig
