import os, shutil

def copy_project_files(targetdir, part):
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
	script = open(os.path.join(targetdir, "script.tcl"), "w")
	script.write(hls_script(part))
	script.close()
	
	
def hls_script(part):
	s = "open_project prj\n"
	s = s + "set_top hls\n"
	
	src = os.path.join(os.path.dirname(os.path.realpath(__file__)), "projectfiles", "src")
	for f in os.listdir(src):
		abspath = os.path.join(src, f)
		if os.path.isfile(abspath):
			if abspath.endswith(".cpp") or abspath.endswith(".h"):
				s = s + 'add_files src/' + f + ' -cflags "-Iinclude/."\n'
				
	s = s + 'open_solution "solution1"\n'
	s = s + 'set_part {' + part + '}\n'
	s = s + 'create_clock -period 10 -name default\n'
	s = s + 'csynth_design\n'
	s = s + 'export_design -format ip_catalog\n'
	return s
