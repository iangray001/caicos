#!/usr/bin/env python

import re

offsets = [
	("PCI_CORE_BASE", "Control signals"), 
	("PCI_CORE_ACCEL_BASE", "Control signals"), 
	("PCI_CORE_ACCEL_RETVAL", "Data signal of ap_return"),

	("PCI_CORE_ACCEL_MASTER0", "Data signal of p_juniper_ram_master"),
	("PCI_CORE_ACCEL_MASTER1", "Data signal of p_juniper_ram_master_char"), 
	("PCI_CORE_ACCEL_MASTER2", "Data signal of p_juniper_ram_master_short"),

	("PCI_CORE_ACCEL_ARG0", "Data signal of opid"),
	("PCI_CORE_ACCEL_ARG1", "Data signal of arg1"),
	("PCI_CORE_ACCEL_ARG2", "Data signal of arg2"),
	("PCI_CORE_ACCEL_ARG3", "Data signal of arg3"),

	("PCI_CORE_ACCEL_SYSCALL_ARG0", "Data signal of syscall_args_cmd_i"),
	("PCI_CORE_ACCEL_SYSCALL_ARG1", "Data signal of syscall_args_arg1_i"),
	("PCI_CORE_ACCEL_SYSCALL_ARG2", "Data signal of syscall_args_arg2_i"),
	("PCI_CORE_ACCEL_SYSCALL_ARG3", "Data signal of syscall_args_arg3_i"),
	("PCI_CORE_ACCEL_SYSCALL_ARG4", "Data signal of syscall_args_arg4_i"),
	("PCI_CORE_ACCEL_SYSCALL_ARG5", "Data signal of syscall_args_arg5_i"),

	("PCI_CORE_ACCEL_SYSCALL_OUT_ARG0", "Data signal of syscall_args_cmd_o"),
	("PCI_CORE_ACCEL_SYSCALL_OUT_ARG1", "Data signal of syscall_args_arg1_o"),
	("PCI_CORE_ACCEL_SYSCALL_OUT_ARG2", "Data signal of syscall_args_arg2_o"),
	("PCI_CORE_ACCEL_SYSCALL_OUT_ARG3", "Data signal of syscall_args_arg3_o"),
	("PCI_CORE_ACCEL_SYSCALL_OUT_ARG4", "Data signal of syscall_args_arg4_o"),
	("PCI_CORE_ACCEL_SYSCALL_OUT_ARG5", "Data signal of syscall_args_arg5_o"),
]

def find(f):
	with open('../hardware/reconfig/caicos/prj/solution1/syn/verilog/hls_AXILiteS_s_axi.v') as fin:
		for line in fin:
			m = re.match("// 0x([0-9A-Fa-f]+) : " + f, line)
			if m != None:
				return m.group(1)
		return None


for defname, searchval in offsets:
	print "#define " + defname + " 0x000000" + find(searchval)
