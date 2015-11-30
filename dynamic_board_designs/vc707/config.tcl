set project_config [dict create \
	project_name {vc707_hw} \
	project_part {xc7vx485tffg1761-2} \
	project_board {xilinx.com:vc707:part0:1.0} \
	mem_offset {0x0} \
	mem_size {0x20000000} \
	mem_slave {mig_7series_0/memmap/memaddr} \
	accel_offset {0x20000000} \
	accel_master {axi_pcie_0/M_AXI} \
	base_checkpoint {base/hls_noclock.dcp} \
	partition_layout {reconfig_4partition.xdc} \
	hls_clock_period {8} \
]