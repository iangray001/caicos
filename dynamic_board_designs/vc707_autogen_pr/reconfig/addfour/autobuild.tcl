open_project "prj"
open_solution "solution1"
set_top hls

foreach f [exec ls src/ | grep -F .c] {
    add_files src/$f -cflags "-Iinclude"
}

foreach f [exec ls | grep -F .h] {
    add_files $f
}

# Use VC707
set_part {xc7vx485tffg1761-2}

create_clock -period 10 -name default
csynth_design
export_design -format syn_dcp -description "caicos dynamic hardware project" -vendor "york.ac.uk" -version "1.0"
