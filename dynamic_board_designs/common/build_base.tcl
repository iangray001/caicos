# This will pull in project_building.tcl and config.tcl
# I realise this is bad design, but it's better than it was.
source "project_management.tcl"

# Make the new project
create_juniper_project $project_config

# Add a single reconfigurable core
add_accel $project_config

# Now build!
synth_base_design $project_config
implement_base_design $project_config
