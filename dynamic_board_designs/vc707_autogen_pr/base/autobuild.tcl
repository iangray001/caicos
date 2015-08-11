open_project "juniperHLS"
open_solution "solution1"

set_top hls

foreach f [exec ls | grep -F .c] {
    add_files $f
}

foreach f [exec ls | grep -F .h] {
    add_files $f
}

# Use VC707
set_part {xc7vx485tffg1761-2}

create_clock -period 10 -name default

# Create some hardware
csynth_design

# And dump to ISE
export_design -format syn_dcp -description "caicos dynamic hardware project" -vendor "york.ac.uk" -version "1.0"
