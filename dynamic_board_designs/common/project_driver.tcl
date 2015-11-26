require "project_management.tcl"
require "project_building.tcl"

# Startup and initial argument parsing
if { [catch {source "config.tcl"} errMsg] } {
    puts "Could not find project config, aborting."
    puts "Detailed error: $errMsg"
    exit 1
}

# Finally, add the working directory to the project config
# This is just incase we change the PWD and want to go back again
# This also allows us to change the behaviour in future (i.e. to always cd to the folder with
# the config file in it, rather than always working out of the current directory).
dict append project_config "root_dir" [file normalize "."]

if { $argc == 0 } {
    error "Usage: $argv0 [base | reconfig <reconfig_dcp>]"
}

switch [lindex $argv 0] {
    base {
        create_juniper_project $project_config
        add_accel $project_config
        synth_base_design $project_config
        implement_base_design $project_config
    }
    reconfig {
        if { $argc < 2 } {
            error "Usage: $argv0 [base | reconfig <reconfig_dcp>]"
        }
        set reconfig_checkpoint [lindex $argv 1]
        build_reconfig_design $project_config $reconfig_checkpoint
    }
    default {
        error "Usage: $argv0 [base | reconfig <reconfig_dcp>]"
    }
}
