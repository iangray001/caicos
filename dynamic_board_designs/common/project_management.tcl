package require "cmdline"

proc create_project { project_config } {
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

    # Helpers...
    set pname [dict get $project_config "project_name"]
    set ppart [dict get $project_config "project_part"]
    
    set prj [create_project -part $ppart $pname "./$pname"]
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
    add_files -norecurse -fileset $constr_set [file normalize "./constrs.xdc"]

    # Add in the IP repo path for wrapper_hdl
    set_property IP_REPO_PATHS [file normalize "./wrapper_hdl"] $prj
    update_ip_catalog
    
    # And set the current runs correctly
    current_run -synthesis [get_runs "synth_1"]
    current_run -implementation [get_runs "impl_1"]

    # Create the base design
    source "./vivado_bd_base.tcl"
    
    return $prj
}

# Startup and initial argument parsing
if { [catch {source "config.tcl"} errMsg] } {
    puts "Could not find project config, aborting."
    puts "Detailed error: $errMsg"
    exit 1
}



