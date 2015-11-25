source "../../config.tcl"


if { ![dict exists $project_config "project_part"] } {
    error "Project part not set in config, aborting."
}
set project_part [dict get $project_config "project_part"]

if { ![dict exists $project_config "hls_clock_period"] } {
    error "HLS clock period not set in config, aborting"
}
set hls_period [dict get $project_config "hls_clock_period"]

open_project "prj"
open_solution "solution1"
set_top hls

foreach f [exec ls | grep -F .c] {
    add_files $f -cflags "-I."
}

foreach f [exec ls | grep -F .h] {
    add_files $f -cflags "-I."
}

# Use VC707
set_part $project_part

create_clock -period $hls_period -name default
csynth_design
export_design -format syn_dcp -description "caicos dynamic hardware project" -vendor "york.ac.uk" -version "1.0"
