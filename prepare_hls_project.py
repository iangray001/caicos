"""
Contains functions for building a Vivado HLS project that implements a set of Java methods.
build_from_functions is the main function that should be called to perform the entire process.
"""

import os, shutil
from os.path import join

from pycparser import c_ast

from flowanalysis import ReachableFunctions, get_files_to_search
from interfaces import InterfaceResolver
from juniperrewrites import c_prototype_of_java_sig, c_decl_node_of_java_sig, rewrite_source_file
import juniperrewrites
from utils import log, mkdir, copy_files, project_path


def build_from_functions(funcs, jamaicaoutputdir, outputdir, additionalsourcefiles, part, notranslatesigs):
	"""
	Build a Vivado HLS project that contains the hardware for a set of Java functions
	
	Args:
		funcs: Iterable of strings of Java signatures that are the methods to include
		jamaicaoutputdir: Absolute path of the Jamaica builder output directory (that contains the generated C code)
		outputdir: Absolute path of the target directory in which to build the project. Will be created if does not exist.
		additionalsourcefiles: Iterable of abs paths for other source files that are required.
		part: The FPGA part to target. Passed directly to the Xilinx tools and not checked.
		notranslatesigs: Signatures of methods that should not be translated
	Returns:
		A tuple of (bindings, syscalls, interfaceResolver)
			bindings = A dictionary of {int -> Java sig} of the hardware methods and their associated call ids.
			syscalls = A dictionary of {callname -> callid} of the sys calls that the hardware may make
			interfaceResolver = Instance of interfaces.InterfaceResolver which contains information about encountered interface calls
	"""
	filestobuild = set()
	filestobuild.add(project_path("projectfiles", "src", "fpgaporting.c"))

	#All functions that should be translated
	all_reachable_functions = set() 
	#Reachable functions that have been excluded by flowanalysis.excluded_functions so need to be handled as a PCIe interrupt
	reachable_non_translated = set() 
	
	interfaceResolver = trace_from_functions(funcs, jamaicaoutputdir, additionalsourcefiles, filestobuild, all_reachable_functions, reachable_non_translated)

	log().info("All reachable functions:")
	for f in all_reachable_functions:
		log().info("\t" + str(f.decl.name) + ": " + str(f.coord.file))
		
	#Build the syscall structure
	#TODO: notranslatedecls is not currently used
	notranslatedecls = determine_no_translate_fns(all_reachable_functions, notranslatesigs)
	callid = 1
	syscalls = {}
	for sysname in reachable_non_translated:
		syscalls[sysname] = callid
		callid = callid + 1
	if len(syscalls) > 0:
		log().info("Generating syscalls for:")
		for callname, sysid in syscalls.iteritems():
			log().info("\t" + str(sysid) + ": " + str(callname))
	
	copy_project_files(outputdir, jamaicaoutputdir, part, filestobuild, all_reachable_functions, syscalls, interfaceResolver)
	
	write_toplevel_header(funcs, jamaicaoutputdir, os.path.join(outputdir, "include", "toplevel.h"))
	bindings = write_functions_c(funcs, jamaicaoutputdir, os.path.join(outputdir, "src", "functions.c"))
	write_hls_script(os.path.join(outputdir, "src"), part, os.path.join(outputdir, "autobuild.tcl"))
	return (bindings, syscalls, interfaceResolver)


def trace_from_functions(funcs, jamaicaoutputdir, additionalsourcefiles, filestobuild, all_reachable_functions, reachable_non_translated):
	"""
	Given a list of java signatures, search for its funcdecl, then trace from each function 
	flowing funccall nodes to determine all reachable functions.
	
	Returns the interfaceResolver which contains information about encountered interface calls
	"""
	interfaceResolver = InterfaceResolver(jamaicaoutputdir)
	
	for sig in funcs:
		filestosearch = get_files_to_search(sig, jamaicaoutputdir)
		
		if additionalsourcefiles != None:
			for f in additionalsourcefiles: 
				if not f in filestosearch: 
					filestosearch.append(f)
				filestobuild.add(f)

		funcdecl = c_decl_node_of_java_sig(sig, jamaicaoutputdir)
		rf = ReachableFunctions(funcdecl.parent, filestosearch, jamaicaoutputdir, interfaceResolver)
		
		#Don't forget to include the starting point
		all_reachable_functions.add(funcdecl.parent)
		filestobuild.add(funcdecl.parent.coord.file)

		for f in rf.files: 
			filestobuild.add(f)
		for f in rf.reachable_functions: 
			all_reachable_functions.add(f)
		for r in rf.reachable_non_translated: 
			reachable_non_translated.add(r)
			
	return interfaceResolver
	

def copy_project_files(targetdir, jamaicaoutputdir, fpgapartname, filestobuild, reachable_functions, syscalls, interfaceResolver):
	"""
	Prepare an HLS project. Copies all required files from the local 'projectfiles' dir into targetdir
	along with any extra required files.
	Args:
		targetdir: directory to output to
		jamaicaoutputdir: absolute path that contains the output of Jamaica builder
		fgpapartname: string of the fpga part name
		filestobuild: array of absolute file paths and will be added to the HLS tcl script as source files
		reachable_functions: array of FuncDecl nodes that are reachable and require translation
		syscalls: map{string->int} names of function calls that should be translated to PCIe system calls -> ID of call
	"""
	mkdir(targetdir)
	copy_files(project_path("projectfiles", "include"), join(targetdir, "include"), [".h"])
	copy_files(project_path("projectfiles", "src"), join(targetdir, "src"), [".h", ".c"])
	shutil.copy(join(jamaicaoutputdir, "Main__.h"), join(targetdir, "include"))

	for f in filestobuild:
		if not os.path.basename(f) == "fpgaporting.c": #We needed fpgaporting to perform reachability analysis, but don't rewrite it
			log().info("Adding source file: " + f)
			if f.endswith(".c"): #We only parse C files
				targetfile = os.path.join(targetdir, "src", os.path.basename(f))
				rewrite_source_file(f, targetfile, reachable_functions, syscalls, interfaceResolver)
		

	
def write_hls_script(targetsrcdir, fpgapartname, outputfilename):
	"""
	Prepare the TCL script which will control HLS.
	All .cpp, .c or .h files in targetsrcdir will be added as sources in the script, so 
	this should be called after rest of the project has been set up.
	"""
	s = "open_project prj\n"
	s += "set_top hls\n"
	
	for f in os.listdir(targetsrcdir):
		abspath = os.path.join(targetsrcdir, f)
		if os.path.isfile(abspath):
			#Only bother adding HLS-supported file types
			if abspath.endswith(".cpp") or abspath.endswith(".c") or abspath.endswith(".h"):
				s = s + 'add_files src/' + f + ' -cflags "-Iinclude/."\n'

	s += 'open_solution "solution1"\n'
	s += 'set_part {' + fpgapartname + '}\n'
	s += 'create_clock -period 8 -name default\n'
	s += 'csynth_design\n'
	s += 'export_design -format syn_dcp -description "caicos project" -vendor "york.ac.uk" -version "1.0"\n'
	#s = s + 'export_design -format ip_catalog -description "caicos project" -vendor "york.ac.uk" -version "1.0"\n'

	script = open(outputfilename, "w")
	script.write(s)
	script.close()


def get_paramlist_of_sig(sig, jamaicaoutputdir):
	"""
	Given a Java signature, find the corresponding C function declaration in the AST
	and then parse to the parameter list. Returns a c_ast.ParamList or None in the
	case of an error.
	"""
	declnode = c_decl_node_of_java_sig(sig, jamaicaoutputdir)
	funcdecl = declnode.children()[0][1]
	if not isinstance(funcdecl, c_ast.FuncDecl):
		raise juniperrewrites.CaicosError("Unexpected function declaration format for signature " + str(sig) + ". Expected FuncDecl, got " + type(funcdecl).__name__)
	paramlist = funcdecl.children()[0][1]
	if not isinstance(paramlist, c_ast.ParamList):
		raise juniperrewrites.CaicosError("Unexpected function declaration format for signature " + str(sig) + ". Expected ParamList, got " + type(funcdecl).__name__)
	return paramlist
	

def get_args_max(functions, jamaicaoutputdir):
	"""
	Look through the functions in the generated code to determine the largest number of arguments any one has
	"""
	maxseen = 0
	for sig in functions:
		paramlist = get_paramlist_of_sig(sig, jamaicaoutputdir)
		maxseen = max([maxseen, len(paramlist.params)])
	return maxseen


def write_toplevel_header(functions, jamaicaoutputdir, outputfile):
	"""
	Prepare the header for the toplevel .cpp file.
	This is a simple header guard, then the prototypes of the functions passed in
	functions = [java_signatures_of_functions_to_prototype]
	"""
	s = 	"#ifndef TOPLEVEL_H_\n"
	s += "#define TOPLEVEL_H_\n"
	s += "\n"
	s += "#include <jamaica.h>\n"
	s += "\n"
	s = s + "#define ARGS_MAX " + str(get_args_max(functions, jamaicaoutputdir))
	s += "\n"
	for f in functions:
		s = s + c_prototype_of_java_sig(f, jamaicaoutputdir) + "\n"
	s += "\n"
	s += "#endif /* TOPLEVEL_H_ */\n"
	hfile = open(outputfile, "w")
	hfile.write(s)
	hfile.close()



def call_code_for_sig(sig, jamaicaoutputdir):
	"""
	Given a Java signature, return the code to call the translated C version of it, to be 
	placed in functions.cpp.
	Note: This function doesn't check the AST structure. This IS checked in get_paramlist_of_sig 
	so something will error if the AST is wrong, but this in general assumes a well-formed AST.
	"""
	declnode = c_decl_node_of_java_sig(sig, jamaicaoutputdir)
	funcdecl = declnode.children()[0][1]
	funcname = declnode.name
	paramlist = funcdecl.children()[0][1]
	rv = funcname + "("
	paramnum = 1 #Param 0 is always used to pass the current thread context
	for pid in xrange(len(paramlist.params)):
		#Insert an explicit cast to the target type
		#Handles single-stage pointers and base types only, no arrays, because
		#it is believed that this is all that Jamaica builder will output.
		prm = paramlist.params[pid]
		pointer = False
		if isinstance(prm.type, c_ast.PtrDecl):
			pointer = True
			pdec = prm.type.type
		else:
			pdec = prm.type
			
		if pointer and pdec.type.names[0] == "jamaica_thread":
			rv += "&__juniper_thread"
		else:
			rv += juniperrewrites.c_cast_for_type(prm.type)	+ " "
			#rv += "(" + str(pdec.type.names[0])
			#if pointer: 
			#	rv += "*"
			#rv += ") "
			rv += "__juniper_args[" + str(paramnum) + "]"
			paramnum = paramnum + 1
			
		if not pid == len(paramlist.params) - 1:
			rv += ", "
			
	rv += ");" 
	return rv


def write_functions_c(functions, jamaicaoutputdir, outputfile):
	"""
	Prepare functions.c, which contains the dispatch functions that calls the translated methods.
	"""
	bindings = {}
	callid = 0
	
	s =     "#include <jamaica.h>\n"
	s += "#include <toplevel.h>\n"
	s += "#include <Main__.h>\n"
	s += "\n"
	s += "int __juniper_call(int call_id) {\n"
	s += "\tswitch(call_id) {\n"
	
	for f in functions:
		bindings[callid] = f #Note the binding of index to function
		s += "\t\tcase " + str(callid) + ":\n"
		
		s += "\t\t\t__juniper_thread = (jamaica_thread) __juniper_args[0];\n"
		
		#The return type affects how we call it
		#TODO: Currently 64-bit return types are not supported
		declnode = c_decl_node_of_java_sig(f, jamaicaoutputdir)
		returntype = str(declnode.type.type.type.names[0])
		if returntype == "void":
			s += "\t\t\t" + str(call_code_for_sig(f, jamaicaoutputdir)) + "\n"
			s += "\t\t\treturn 0;\n"
		elif returntype in ['float', 'jamaica_float']:
			s += "\t\t\t" + returntype + " rv;\n"
			s += "\t\t\trv = " + str(call_code_for_sig(f, jamaicaoutputdir)) + "\n"
			s += "\t\t\treturn *(int *)&rv;"
		else:
			s += "\t\t\treturn (int) " + str(call_code_for_sig(f, jamaicaoutputdir)) + "\n"
		s += "\n"
		callid = callid + 1
		
	s += "\t\tdefault:\n"
	s += "\t\t\treturn 0;\n"
	s += "\t}\n"
	s += "}\n"
	hfile = open(outputfile, "w")
	hfile.write(s)
	hfile.close()
	return bindings



def determine_no_translate_fns(all_reachable_functions, notranslatesigs):
	"""
	Return an array which is a subset of all_reachable_functions for the functions that 
	the user has configured to not be translated and therefore implemented as a PCIe 
	interrupt to the host.
	"""
	rv = set()	
	for funcdef in all_reachable_functions:
		details = juniperrewrites.get_java_names_of_funcdef(funcdef)
		if details != None and details[0] in notranslatesigs:
			rv.add(funcdef)
			
	return rv
	
