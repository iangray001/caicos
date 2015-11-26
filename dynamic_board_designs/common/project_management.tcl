package require "cmdline"

proc create_juniper_project { project_config } {
    # Check that the project config has the required keys set
    if { ![dict exists $project_config "project_name"] } {
        error "Could not find config key \"project_name\", exiting."
    }
    
    if { [dict get $project_config "project_name"] == "" } {
        error "Project name cannot be empty"
    }
    
    if { ![dict exists $project_config "project_part"] } {
        error "Could not find config key \"project_part\", exiting."
    }
    
    # Ensure that the part is valid
    if { [lsearch [get_parts] [dict get $project_config "project_part"]] == -1 } {
        error "Unrecognised part, valid Vivado parts are as follows: [get_parts]"
    }

    set root_dir [dict get $project_config "root_dir"]

    # Helpers...
    set pname [dict get $project_config "project_name"]
    set ppart [dict get $project_config "project_part"]
    
    set prj [create_project -part $ppart $pname "$root_dir/$pname"]
    set prj_dir [get_property "directory" $prj]

    # Is there a board part set?
    if { [dict exists $project_config "project_board"] } {
        set_property "board_part" [dict get $project_config "project_board"] $prj
    }
    
    # Check that the required filesets exists
    if {[get_filesets -quiet "sources_1"] == ""} {
        create_fileset -srcset "sources_1"
    }
    if {[get_filesets -quiet "constrs_1"] == ""} {
        create_fileset -constrset "constrs_1"
    }
    
    # Add in the constraints
    set constr_set [get_filesets "constrs_1"]
    add_files -norecurse -fileset $constr_set [file normalize "$root_dir/constrs.xdc"]

    # Add in the IP repo path for wrapper_hdl
    set_property IP_REPO_PATHS [file normalize "$root_dir/wrapper_hdl"] $prj
    update_ip_catalog
    
    # And set the current runs correctly
    current_run -synthesis [get_runs "synth_1"]
    current_run -implementation [get_runs "impl_1"]

    # Create the base design
    source "$root_dir/vivado_bd_base.tcl"

    # Create the wrapper for the board design
    set bd_file [get_property FILE_NAME [get_bd_designs]]
    make_wrapper -import -fileset [get_filesets "sources_1"] -top [get_files $bd_file]
    
    return $prj
}

proc add_accel { project_config } {
    # Ensure we have a board design open
    if { [current_bd_design] == "" } {
        set bd_file [get_property FILE_NAME [get_bd_designs]]
        open_bd_design $bd_file
    }

    # Sanity checking
    if { ![dict exists $project_config "mem_offset"] } {
        error "Mem offset not defined in current project settings"
    }

    if { ![dict exists $project_config "mem_size"] } {
        error "Mem size not defined in current project settings"
    }

    if { ![dict exists $project_config "mem_slave"] } {
        error "Mem slave not defined in current project settings"
    }

    if { ![dict exists $project_config "accel_offset"] } {
        error "Accel base offset not defined in current project settings"
    }

    if { ![dict exists $project_config "accel_master"] } {
        error "Accel master offset not defined in current project settings"
    }

    # First, instantiate the core
    set ip_vlnv [get_ipdefs -regexp "york.ac.uk:hls:juniper_hls_toplevel:.*"]
    if { $ip_vlnv == "" } {
        error "Could not find VLNV for toplevel component"
    }

    # How many top level components currently exist?
    set new_top_index [llength [get_bd_cells -quiet "top_*"]]

    # Create our core, named appropriately
    set new_cell [create_bd_cell -vlnv $ip_vlnv -type IP "top_$new_top_index"]
    set num_top_components [expr {$new_top_index + 1}]

    # Connect it up
    set mem_interconnect [get_bd_cells "mem_interconnect"]
    set periph_interconnect [get_bd_cells "periph_interconnect"]

    # Add a slave to the mem interconnect to allow the core to communicate with memory,
    # and a master to the periph interconnect so we can talk to the core
    set mem_interconnect_newidx [get_property CONFIG.NUM_SI $mem_interconnect]
    set periph_interconnect_newidx [get_property CONFIG.NUM_MI $periph_interconnect]
    set_property CONFIG.NUM_SI [expr {$mem_interconnect_newidx + 1}] $mem_interconnect
    set_property CONFIG.NUM_MI [expr {$periph_interconnect_newidx + 1}] $periph_interconnect

    set mem_interconnect_newidx_pad [format "%02d" $mem_interconnect_newidx]
    set periph_interconnect_newidx_pad [format "%02d" $periph_interconnect_newidx]

    # Connect up
    connect_bd_net -net accel_clock [get_bd_pins $new_cell/ap_clk] [get_bd_pins $mem_interconnect/S[set mem_interconnect_newidx_pad]_ACLK] [get_bd_pins $periph_interconnect/M[set periph_interconnect_newidx_pad]_ACLK]
    connect_bd_net -net accel_resetn [get_bd_pins $new_cell/ap_rst_n] [get_bd_pins $mem_interconnect/S[set mem_interconnect_newidx_pad]_ARESETN] [get_bd_pins $periph_interconnect/M[set periph_interconnect_newidx_pad]_ARESETN]
    connect_bd_intf_net [get_bd_intf_pins $new_cell/m_axi_mem] [get_bd_intf_pins $mem_interconnect/S[set mem_interconnect_newidx_pad]_AXI]
    connect_bd_intf_net [get_bd_intf_pins $periph_interconnect/M[set periph_interconnect_newidx_pad]_AXI] [get_bd_intf_pins $new_cell/s_axi_AXILiteS]

    # And to the hold output system
    # The GPIO must have at least two outputs so that the slicers can be constructed correctly
    set gpio [get_bd_cells "axi_gpio_0"]

    if { $num_top_components == 1 } {
        set_property CONFIG.C_GPIO_WIDTH 2 $gpio
    } else {
        set_property CONFIG.C_GPIO_WIDTH $num_top_components $gpio
    }

    set gpio_slice [create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 "slice_$new_top_index"]
    if { $num_top_components == 1 } {
        set_property CONFIG.DIN_WIDTH 2 [get_bd_cells "slice_*"]
    } else {
        set_property CONFIG.DIN_WIDTH $num_top_components [get_bd_cells "slice_*"]
    }

    set_property CONFIG.DIN_FROM $new_top_index $gpio_slice
    set_property CONFIG.DIN_TO   $new_top_index $gpio_slice

    connect_bd_net [get_bd_pins $gpio/gpio_io_o] [get_bd_pins $gpio_slice/Din]
    connect_bd_net [get_bd_pins $gpio_slice/Dout] [get_bd_pins $new_cell/hold_outputs]

    # Finally, wire up interrupts
    # We're using a concat here which may already have connections into it. Get the current number of pins, and act accordingly
    set concat [get_bd_cells "axi_intc_concat"]
    set new_concat_index [get_property CONFIG.NUM_PORTS $concat]
    set_property CONFIG.NUM_PORTS [expr {$new_concat_index + 1}] $concat
    connect_bd_net [get_bd_pins $new_cell/jamaica_syscall] [get_bd_pins $concat/In[set new_concat_index]]

    # Finally, do the address mapping
    set mem_offset [dict get $project_config "mem_offset"]
    set mem_size [dict get $project_config "mem_size"]
    set mem_slave [dict get $project_config "mem_slave"]

    set periph_offset [dict get $project_config "accel_offset"]
    set periph_master [dict get $project_config "accel_master"]

    create_bd_addr_seg -range $mem_size -offset $mem_offset [get_bd_addr_spaces $new_cell/m_axi_mem] [get_bd_addr_segs $mem_slave] "SEG_top_[set new_top_index]_mem"
    create_bd_addr_seg -range 0x10000 -offset [expr {$periph_offset + (0x100000 * $new_top_index)}] [get_bd_addr_spaces $periph_master] [get_bd_addr_segs $new_cell/s_axi_AXILiteS/reg0] "SEG_top_[set new_top_index]_periph"
}
