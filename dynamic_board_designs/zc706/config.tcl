set project_config [dict create \
	project_name {zc706_hw} \
	project_part {xc7z045ffg900-2} \
	project_board {xilinx.com:zc706:part0:1.1} \
	mem_offset {0x0} \
	mem_size {0x20000000} \
	mem_slave {mig_7series_0/memmap/memaddr} \
	accel_offset {0x60000000} \
	accel_master {processing_system7_0/Data} \
	base_checkpoint {base/hls_noclock.dcp} \
	partition_layout {reconfig_1partition.xdc} \
	hls_clock_period {10} \
]