open_project "juniperHLS"
open_solution "solution1"

# toplevel is called 'hls'
set_top hls

foreach f [exec ls src/ | grep -F .c] {
    add_files src/$f -cflags "-Iinclude"
}

foreach f [exec ls src/ | grep -F .cpp] {
    add_files src/$f -cflags "-Iinclude"
}

# Use VC707
set_part {xc7vx485tffg1761-2}

# I'm not sure the clock actually matters...
create_clock -period 10 -name default

# Create some hardware
csynth_design

# And dump to ISE
export_design -format syn_dcp -description "A module for the JUNIPER project" -vendor "york.ac.uk" -version "VERSIONNUM"

exit
