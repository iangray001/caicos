source "project_management.tcl"

if { $argc != 1 } {
	error "Checkpoint name not supplied, aborting"
}

set reconfig_checkpoint [lindex $argv 0]

build_reconfig_design $project_config $reconfig_checkpoint
