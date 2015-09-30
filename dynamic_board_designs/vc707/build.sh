#!/bin/bash

# TODO: Add dependency management or proper error checking
rm -rf ./iprepo
mkdir ./iprepo

NUMPRJ=1

cd base
vivado_hls autobuild.tcl || exit 1
cd ../

# Now create the Vivado projects.
rm -rf vc707_hw
rm -f generated.xdc
vivado -mode batch -source vivado_project.tcl
vivado -mode batch -source vivado_bd_base.tcl vc707_hw/vc707_hw.xpr

# Now create the board TCL
cat > ./vivado_bd_cores.tcl <<EOF
    open_bd_design [get_property directory [current_project]]/vc707_hw.srcs/sources_1/bd/juniper_board/juniper_board.bd

    # Resize the AXI buses
    set_property -dict [ list CONFIG.NUM_SI {$((NUMPRJ+2))} ] [get_bd_cells mem_interconnect]
    set_property -dict [ list CONFIG.NUM_MI {$((NUMPRJ+6))} ] [get_bd_cells periph_interconnect]
    set_property -dict [ list CONFIG.C_GPIO_WIDTH {$((NUMPRJ+1))}  ] [get_bd_cells axi_gpio_0]
    set_property -dict [ list CONFIG.NUM_PORTS {$((NUMPRJ+1))} ] [get_bd_cells axi_intc_concat]
EOF

# Add in the buffers
# Seq is inclusive (start, step, end)
for i in $(seq 2 1 $((NUMPRJ+1))); do
    echo "set_property CONFIG.S0${i}_HAS_REGSLICE 3 [get_bd_cells mem_interconnect]"
done

for i in $(seq 6 1 $((NUMPRJ+5))); do
    echo "set_property CONFIG.M0${i}_HAS_REGSLICE 3 [get_bd_cells periph_interconnect]"
done

# And sub into the MHS file
# TODO: This may be fragile
for i in $(seq 0 1 $((NUMPRJ-1))); do
#    if [ -d "./base/$i" ]; then
        # Create the cores...
        cat >> ./vivado_bd_cores.tcl <<EOF
set top_${i} [ create_bd_cell -type ip -vlnv york.ac.uk:hls:juniper_hls_toplevel:1.0 top_${i} ]
connect_bd_intf_net -intf_net periph_interconnect_M0$((i+6))_AXI [get_bd_intf_pins periph_interconnect/M0$((i+6))_AXI] [get_bd_intf_pins top_${i}/s_axi_AXILiteS]
connect_bd_intf_net -intf_net top_${i}_m_axi_mem [get_bd_intf_pins mem_interconnect/S0$((i+2))_AXI] [get_bd_intf_pins top_${i}/m_axi_mem]
connect_bd_net -net axi_pcie_0_axi_aclk_out [get_bd_pins mem_interconnect/S0$((i+2))_ACLK] [get_bd_pins periph_interconnect/M0$((i+6))_ACLK] [get_bd_pins top_${i}/ap_clk]
connect_bd_net -net proc_sys_reset_1_interconnect_aresetn [get_bd_pins mem_interconnect/S0$((i+2))_aresetn] [get_bd_pins periph_interconnect/M0$((i+6))_aresetn]
connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins top_${i}/ap_rst_n]
create_bd_addr_seg -range 0x20000000 -offset 0x0 [get_bd_addr_spaces top_${i}/m_axi_mem] [get_bd_addr_segs mig_7series_0/memmap/memaddr] SEG_mig_7series_${i}_memaddr
create_bd_addr_seg -range 0x20000000 -offset 0x40000000 [get_bd_addr_spaces top_${i}/m_axi_mem] [get_bd_addr_segs axi_pcie_0/S_AXI/BAR0] SEG_axi_pcie_top_${i}_memaddr
create_bd_addr_seg -range 0x10000 -offset 0x20${i}00000 [get_bd_addr_spaces axi_pcie_0/M_AXI] [get_bd_addr_segs top_${i}/s_axi_AXILiteS/reg0] SEG_top_${i}_Reg

connect_bd_net -net axi_intc_concat_$((i+1)) [get_bd_pins top_${i}/jamaica_syscall] [get_bd_pins axi_intc_concat/In$((i+1))]

# Create instance: xlslice_${i}, and set properties
set xlslice_${i} [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_${i} ]
set_property -dict [ list CONFIG.DIN_WIDTH {$((NUMPRJ+1))} CONFIG.DIN_FROM ${i} CONFIG.DIN_TO ${i} ] \$xlslice_${i}
connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins axi_gpio_0/gpio_io_o] [get_bd_pins xlslice_${i}/Din]
connect_bd_net -net xlslice_${i}_Dout [get_bd_pins top_${i}/hold_outputs] [get_bd_pins xlslice_${i}/Dout]
EOF
#    fi
done

cat >> ./vivado_bd_cores.tcl <<EOF
save_bd_design

make_wrapper -files [get_files [get_property directory [current_project]]/vc707_hw.srcs/sources_1/bd/juniper_board/juniper_board.bd] -top
add_files -norecurse [get_property directory [current_project]]/vc707_hw.srcs/sources_1/bd/juniper_board/hdl/juniper_board_wrapper.v
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

vivado -mode batch -source vivado_bd_cores.tcl vc707_hw/vc707_hw.xpr

./make_base
./make_reconfig all
