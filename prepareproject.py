import os, shutil

from juniperrewrites import c_prototype_of_java_sig
import juniperrewrites
from utils import log


def copy_project_files(targetdir, fpgapartname, extrasourcefiles, astcache):
	"""
	Prepare an HLS project. Copies all required files from the local 'projectfiles' dir into targetdir
	along with any extra required files.
	extrasourcefiles is an array of absolute file paths and will be added to the HLS tcl script as source files
	"""
	def mkdir(d): 
		if not os.path.exists(d): os.makedirs(d);
		
	def copy_files(srcdir, targetdir, suffixes):
		mkdir(targetdir)
		for f in os.listdir(srcdir):
			abspath = os.path.join(srcdir, f)
			if os.path.isfile(abspath):
				for suff in suffixes:
					if abspath.endswith(suff):
						shutil.copyfile(abspath, os.path.join(targetdir, f))
						break

	cwd = os.path.dirname(os.path.realpath(__file__))	
	mkdir(targetdir)
	copy_files(os.path.join(cwd, "projectfiles", "include"), os.path.join(targetdir, "include"), [".h"])
	copy_files(os.path.join(cwd, "projectfiles", "src"), os.path.join(targetdir, "src"), [".h", ".cpp"])
	copy_files(os.path.join(cwd, "projectfiles"), targetdir, [".sh"])
	
	for f in extrasourcefiles:
		log().info("Adding source file: " + f)
		if f.endswith(".c"): #We only parse C files
			juniperrewrites.rewrite_source_file(astcache, f, os.path.join(targetdir, "src", os.path.basename(f)))
	
	#The TCL script
	script = open(os.path.join(targetdir, "script.tcl"), "w")
	script.write(hls_script(os.path.join(targetdir, "src"), fpgapartname))
	script.close()


	
def hls_script(targetsrcdir, fpgapartname):
	"""
	Prepare the TCL script which will control HLS.
	Returns the script as a string.
	"""
	s = "open_project prj\n"
	s = s + "set_top hls\n"
	
	for f in os.listdir(targetsrcdir):
		abspath = os.path.join(targetsrcdir, f)
		if os.path.isfile(abspath):
			#Only bother adding HLS-supported file types
			if abspath.endswith(".cpp") or abspath.endswith(".c") or abspath.endswith(".h"):
				s = s + 'add_files src/' + f + ' -cflags "-Iinclude/."\n'
				
	s = s + 'open_solution "solution1"\n'
	s = s + 'set_part {' + fpgapartname + '}\n'
	s = s + 'create_clock -period 10 -name default\n'
	s = s + 'csynth_design\n'
	s = s + 'export_design -format ip_catalog\n'
	return s


def write_toplevel_header(functions, jamaicaoutputdir, astcache, outputfile):
	"""
	Prepare the header for the toplevel .cpp file.
	This is a simple header guard, then the prototypes of the functions passed in
	functions = [java_signatures_of_functions_to_prototype]
	"""
	s = 	"#ifndef TOPLEVEL_H_\n"
	s = s + "#define TOPLEVEL_H_\n"
	s = s + "\n"
	s = s + "#include <jamaica.h>\n"
	s = s + "\n"
	for f in functions:
		s = s + c_prototype_of_java_sig(f, jamaicaoutputdir, astcache) + "\n"
	s = s + "\n"
	s = s + "#endif /* TOPLEVEL_H_ */\n"
	hfile = open(outputfile, "w")
	hfile.write(s)
	hfile.close()
