proc synth_base_design { project_config } {
	# Ensure the project is open
	set autoclose 0
	set root_dir [dict get $project_config "root_dir"]

	if { [current_project] == "" } {
		set project_name [dict get $project_config "project_name"]
		set root_dir [dict get $project_config "root_dir"]
		open_project "$root_dir/$project_name/$project_name.xpr"
		set autoclose 1
	}

    reset_run synth_1
    launch_run synth_1
    wait_on_run synth_1
    open_run synth_1

    # Write out the auto-gen'd constraints
    if { ![file exists "$root_dir/assemble"] } {
        file mkdir "$root_dir/assemble"
        file mkdir "$root_dir/assemble/bitstream"
    }

    write_xdc "$root_dir/assemble/generated.xdc"
    write_checkpoint "$root_dir/assemble/base_synth.dcp"

    if { $autoclose } {
    	close_project
    }
}

proc implement_base_design { project_config } {
	set root_dir [dict get $project_config "root_dir"]
	set partition_layout ""

	# Do we have a project open? If so, close it; we're currently working on checkpoints...
	if { [current_project] != "" } {
		close_project
	}

	if { ![dict exists $project_config "base_checkpoint"] } {
		error "Base checkpoint path not specified"
	}
	set base_checkpoint [dict get $project_config "base_checkpoint"]

	if { [dict exists $project_config "partition_layout" ] } {
		set partition_layout [dict get $project_config "partition_layout"]
	}

	# Open the checkpoint to work on
	open_checkpoint "$root_dir/assemble/base_synth.dcp"
	read_xdc "$root_dir/assemble/generated.xdc"

	if { $partition_layout != "" } {
		read_xdc "$root_dir/$partition_layout"

		set_property HD.RECONFIGURABLE 1 [get_cells */top_*/U0/brg]
	}

	# Read the base checkpoint into the reconfigurable lumps
	set reconfig_sites [get_cells */top_*/U0/brg]

	foreach cell $reconfig_sites {
		read_checkpoint -cell $cell "$root_dir/$base_checkpoint"
	}

	opt_design
	place_design
	route_design
	write_checkpoint "$root_dir/assemble/base_routed.dcp"
	write_bitstream -bin_file -file "$root_dir/assemble/bitstream/base.bit"

	# Are we making a reconfigurable design?
	if { $partition_layout != "" } {
		foreach cell $reconfig_sites {
			update_cell -cell $cell -black_box
		}

		lock_design -level routing
		write_checkpoint "$root_dir/assemble/base_static.dcp"
	}
}

proc build_reconfig_design { project_config reconfig_dcp } {
	set root_dir [dict get $project_config "root_dir"]
	set filename [file rootname [file tail $reconfig_dcp]]

	if { ![file exists "$root_dir/assemble/base_static.dcp"] } {
		error "Could not find static design - have you built the base project in reconfigurable mode?"
	}

	# Again, we're just working on checkpoints, so make sure we don't
	# actually have anything open
	if { [current_project] != "" } {
		close_project
	}

	open_checkpoint "$root_dir/assemble/base_static.dcp"

	# Sub the new checkpoint in and go from there
	set reconfig_sites [get_cells "*/top_*/U0/brg"]

	foreach cell $reconfig_sites {
		read_checkpoint -cell $cell "$root_dir/$reconfig_dcp"
	}

	# Checkpoint is pre-synthesized, just opt, place and route!
	opt_design
	place_design
	route_design
	write_checkpoint "$root_dir/assemble/[set filename]_routed.dcp"
	write_bitstream -bin_file -file "$root_dir/assemble/bitstream/[set filename].bit"
}