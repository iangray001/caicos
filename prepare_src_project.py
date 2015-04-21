'''
Contains functions to prepare the source project that will execute on the host OS and interact 
with the hardware which is generated by prepare_hls_project

build_src_project is the main entry point to this module
@author: ian
'''
import os
import shutil

from pycparser import c_ast
import pycparser

import astcache
from juniperrewrites import c_filename_of_java_method_sig, \
	c_decl_node_of_java_sig
import juniperrewrites
from prepare_hls_project import get_paramlist_of_sig
from utils import CaicosError, mkdir, copy_files
import utils


def build_src_project(bindings, jamaicaoutput, targetdir, jamaica_targetincludedir):
	"""
	Construct the software portion of the project. Copy the C source code for the Jamaica project, 
	refactoring the functions that are implemented on the FPGA.
	Also copies the FPGA interface and build scripts. 
	"""
	cwd = os.path.dirname(os.path.realpath(__file__))	
	mkdir(targetdir)
	copy_files(os.path.join(cwd, "projectfiles", "juniper_fpga_interface"), os.path.join(targetdir, "juniper_fpga_interface"))
	copy_files(os.path.join(cwd, "projectfiles", "malloc_preload"), os.path.join(targetdir, "malloc_preload"))
	refactor_src(bindings, jamaicaoutput, os.path.join(targetdir, "src"), jamaica_targetincludedir)
	shutil.copy(os.path.join(cwd, "projectfiles", "include", "juniperoperations.h"), os.path.join(targetdir, "src"))


def refactor_src(bindings, jamaicaoutput, targetdir, jamaica_targetincludedir):
	"""
	Copy the C source code for the Jamaica project. For files that contain functions that have
	been implemented on the FPGA, replace their contents with code to invoke the FPGA instead.
	
	Outputs files to targetdir
	"""
	
	def fpga_set_arg(code, devNo, partNo, argNo, val):
		return code + r"	juniper_fpga_partition_set_arg(" + str(devNo) + ", " + str(partNo) + ", " + str(argNo) + ", " + str(val) + ");\n"
	
	def fpga_set_mem_base(code, devNo, partNo, val):
		return code + r"	juniper_fpga_partition_set_mem_base(" + str(devNo) + ", " + str(partNo) + ", " + str(val) + ");\n"
	
	def fpga_start(code, devNo, partNo):
		return code + r"	juniper_fpga_partition_start(" + str(devNo) + ", " + str(partNo) + ");\n"
		
	def fpga_wait_for_idle(code, devNo, partNo):
		return code + r"	while(!juniper_fpga_partition_idle(" + str(devNo) + ", " + str(partNo) + "));\n"
		
	def fpga_retval(code, devNo, partNo, rv):
		return code + r"	juniper_fpga_partition_get_retval(" + str(devNo) + ", " + str(rv) + ");\n"
		
	def fpga_run(code, devNo, partNo):
		return code + fpga_start("", devNo, partNo) + fpga_wait_for_idle("", devNo, partNo)

	
	#Which are the C files we will need to rewrite?
	filestoedit = set()
	for _, sig in bindings.iteritems():
		filestoedit.add(c_filename_of_java_method_sig(sig, jamaicaoutput))
		
	#Copy the C files over, rewriting those that contain functions we have passed to the hardware
	mkdir(targetdir)
	for item in os.listdir(jamaicaoutput):
		filepath = os.path.join(jamaicaoutput, item)
		if os.path.isfile(filepath) and (filepath.endswith(".c") or filepath.endswith(".h")):
			if not filepath in filestoedit:
				#Just copy the file, no edits required
				shutil.copy(filepath, targetdir)
			else:
				#Any previous edits to the AST need to be disregarded
				astcache.invalidate_ast_for(filepath)
				ast = astcache.get(filepath, jamaica_targetincludedir)
				
				for callid, sig in bindings.iteritems():
					if c_filename_of_java_method_sig(sig, jamaicaoutput) == filepath:
						decl = c_decl_node_of_java_sig(sig, jamaicaoutput)
						
						devNo = 0
						partNo = 0
						
						code = """int *base = malloc(0);\n\tint rv;\n\n"""
						code = fpga_set_mem_base(code, devNo, partNo, "-((int) base)") + "\n"

						#Set up the arguments for the call
						params = get_paramlist_of_sig(sig, jamaicaoutput)
						firstarg = True
						for pnum in xrange(len(params.params)):
							paramname = params.params[pnum].name
							typename = ""
							current = params.params[pnum].type
							while isinstance(current, c_ast.PtrDecl):
								typename = typename + "*"
								current = current.type

							if not isinstance(current, c_ast.TypeDecl):
								raise CaicosError("Unhandled parameter type in function " + str(sig) + " for parameter " + str(paramname))
								
							typename = typename + current.type.names[0]
							
							if typename == "*jamaica_thread" and paramname == "ct":
								#All functions start with a reference to the current thread and can be ignored by the FPGA
								pass
							else:
								if firstarg: 
									code = fpga_set_arg(code, devNo, partNo, 0, "OP_WRITE_ARG")
									firstarg = False
								code = fpga_set_arg(code, devNo, partNo, 1, pnum)
								
								if typename in ["jamaica_int8", "jamaica_int16", "jamaica_int32", "jamaica_uint8", "jamaica_uint16", "jamaica_uint32", "jamaica_bool"]:
									code = fpga_set_arg(code, devNo, partNo, 2, paramname)
								elif typename == "jamaica_ref":
									code = fpga_set_arg(code, devNo, partNo, 2, paramname + " / 4")
								elif typename == "jamaica_float":
									code = fpga_set_arg(code, devNo, partNo, 2, "*((int *) &" + paramname + ")")
								elif typename == "jamaica_address":
									code = fpga_set_arg(code, devNo, partNo, 2, paramname)
								elif typename in ["jamaica_int64", "jamaica_uint64"]:
									#TODO
									pass
									#code = fpga_set_arg(code, devNo, partNo, 2, paramname)
								elif typename == "jamaica_double":
									#TODO
									pass
									#code = fpga_set_arg(code, devNo, partNo, 2, paramname)
								else:
									raise CaicosError("Unknown type " + str(typename) + " in function " + str(sig))
	
								code = fpga_run(code, devNo, partNo) + "\n"
								
						code = fpga_set_arg(code, devNo, partNo, 0, "OP_CALL")
						code = fpga_set_arg(code, devNo, partNo, 1, callid)
						code = fpga_run(code, devNo, partNo) + "\n"
						
						#Do we need a return value?
						rettype = decl.type.type.type.names[0] 
						if rettype == "void":
							code = code + "	return;\n";
						elif rettype in ["jamaica_int8", "jamaica_int16", "jamaica_int32", 
										"jamaica_uint8", "jamaica_uint16", "jamaica_uint32", 
										"jamaica_bool"
										"jamaica_ref", "jamaica_address"
										]:
							code = fpga_retval(code, devNo, partNo, "&rv")
							code = code + "	return rv;\n"
						elif rettype == "jamaica_float":
							code = fpga_retval(code, devNo, partNo, "&rv")
							code = code + "	return *((float *)(&rv));\n"
						elif rettype in ["jamaica_int64", "jamaica_uint64"]:
							#TODO
							code = code + "	return 0;\n"
						elif rettype == "jamaica_double":
							#TODO
							code = code + "	return 0;\n"
						else:
							raise CaicosError("Unknown return type " + str(typename) + " in function " + str(sig))
						
						#Wrap up the text in an ID node, then replace the function's body 
						textnode = c_ast.ID("name")
						setattr(textnode, 'name', code)
						compnode = c_ast.Compound([textnode])
						juniperrewrites.replace_node(decl.parent.body, compnode)
				
				utils.write_ast_to_file(ast, os.path.join(targetdir, item), "#include <juniper_fpga_interface.h>\n#include <juniperoperations.h>\n\n")
