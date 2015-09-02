"""
The main caicos module.

Contains the main entry point to caicos, with routines to build the hardware and software projects, 
and to read and parse configuration files.

Call build_all() to begin.
"""
"""
Describes the required options in the configuration of caicos.
All valid options must be enumerated in this dictionary. Each option name maps
to a tuple of (required, arrayname) where required is a boolean that is True
if the option is mandatory. If arrayname is not "", then the option will be
added as a member of an array with that name.

Multiple definitions of the same variable are valid, and the last definition
takes precedence.
"""

import os, shutil, sys
from string import Template

import pycparser

import astcache
import prepare_hls_project
import prepare_src_project
from utils import mkdir, CaicosError, log, project_path, make_executable, \
	copy_files
import utils


config_specification = {
	#Standard options
	'signatures': (False, ""), # list of Java method signatures to be compiled to hardware
	'jamaicaoutputdir': (True, ""), # path to compiled C files from jamaicabuilder
	'additionalhardwarefiles': (False, ""), # list of paths to additional files to add to the hardware project
	'outputdir': (True, ""), # path to directory to output to
	'jamaicatarget': (True, ""), # path to the Jamaica target directory to use for the host software  
	'cleanoutput': (False, ""), # if 'true', the contents of outputdir will be cleaned first
	'hwoutputdir': (False, ""), #if set, then the hardware project will be put here instead of the default location
	'swoutputdir': (False, ""), #if set, then the software project will be put here instead of the default location
	'scriptsoutputdir': (False, ""), #if set, then the control scripts will be put here instead of the default location
	'jamaicaoutputdir_hw': (False, ""), #if set, then caicos will use this Jamaica Builder output directory for the hardware project instead of jamaicaoutputdir
	'notranslates': (False, ""), #A list of Java signatures which should not be translated into hardware
	'targetboard': (True, ""), #Which board should we build the base system for (currently "vc707", or "zc706")
	'fpgapart': (False, ""), # the FPGA part to target, in Xilinx format. Normally set from "targetboard", but can be overridden by specifying it manually
	'hlsprojectname': (False, ""), #The name of the HLS project caicos should generate
	
	#Developer options
	'dev_softwareonly': (False, ""),
	'astcache': (False, ""), #If set to a directory, caches parsed ASTs here 
	
	#Array singletons
	'signature': (False, "signatures"),
	'additionalhardwarefile': (False, "additionalhardwarefiles"),
	'notranslate': (False, 'notranslates'),
}

"""
An array of the fpga part names that are in each supported board.
"""
fpgapart = {
	'vc707': 'xc7vx485tffg1761-2',
	'zc706': 'xc7z045ffg900-2',
}


def build_all(config):
	"""
	Build a JUNIPER project. Format of the config dictionary is described in the docstring for config_specification.
	"""
	utils.remove_slots_from_classes(pycparser)
	
	try:
		if config.get('cleanoutput', "false").lower() == "true":
			shutil.rmtree(config['outputdir'], ignore_errors=True)
		
		mkdir(config['outputdir'])
		
		#Determine output paths
		swdir = config.get('swoutputdir', os.path.join(config['outputdir'], "software"))
		boarddir = config.get('hwoutputdir', os.path.join(config['outputdir'], "hardware"))
		scriptsdir = config.get('scriptsoutputdir', os.path.join(config['outputdir'], "scripts"))
		hwdir = os.path.join(boarddir, "reconfig", config.get('hlsprojectname', 'caicos'))
		
		if 'astcache' in config:
			astcache.activate_cache(config['astcache'])
		
		#Create board design
		log().info("Building board design for " + config['targetboard'] + " in " + str(boarddir) + "...")
		utils.copy_files(project_path("dynamic_board_designs", 'common'), boarddir)
		utils.copy_files(project_path("dynamic_board_designs", config['targetboard']), boarddir)
		
		#Build hardware project
		log().info("Building hardware project in " + str(hwdir) + "...")
		if config.get('dev_softwareonly', "false").lower() == "true":
			log().warning("dev_softwareonly is set. Generating software only.")
			bindings = __getfakebindings(config['signatures'])
		else:	
			bindings, syscalls, interfaceResolver = prepare_hls_project.build_from_functions(
				config['signatures'], 
				config.get('jamaicaoutputdir_hw', config['jamaicaoutputdir']),
				hwdir, 
				config.get('additionalhardwarefiles'), 
				config['fpgapart'],
				config.get('notranslates', [])
			)
		
			target = os.path.join(config['outputdir'], "push.sh")
			shutil.copyfile(project_path("projectfiles", "scripts", "push.sh"), target)
			
			make_executable([target,
							os.path.join(boarddir, 'build.sh'),
							os.path.join(boarddir, 'make_base'),
							os.path.join(boarddir, 'make_reconfig')
			])
			
		#Build software project
		log().info("Building software project in " + str(swdir) + "...")
		prepare_src_project.build_src_project(bindings, config['jamaicaoutputdir'], swdir, syscalls, interfaceResolver)
	
		#Output templated Makefile
		contents = open(project_path("projectfiles", "scripts", "Makefile")).read()
		subs = {'SUB_JAMAICATARGET': config['jamaicatarget']}
		template = Template(contents)
		fout = open(os.path.join(swdir, "Makefile"), "w")
		fout.write(template.safe_substitute(subs))
		fout.close()
		
		#Output scripts folder
		mkdir(scriptsdir)
		for fn in ['cmd_template.bat', 'program.sh', 'rescan.sh']:
			shutil.copyfile(project_path("projectfiles", "scripts", fn), os.path.join(scriptsdir, fn))
		
		#Output kernel module
		copy_files(project_path("system_software", "host_kernel_module"), os.path.join(swdir, "host_kernel_module"))
		
	except CaicosError, e:
		log().error("A critical error was encountered:\n\t" + str(e))


	
def load_config(filename):
	"""
	Open the provided filename as a caicos config file.
	Validates the file, and that all required options are set, using the config_specification structure.
	Raises CaicosError if a problem is found.
	"""
	
	def make_array_or_append(config, name, val):
		if name in config:
			config[name].append(val)
		else:
			config[name] = [val]
	
	if not os.path.exists(filename) and os.path.isfile(filename):
		raise CaicosError(str(filename) + " is not a valid file.")
	f = open(filename)

	config = {'signatures': []}

	lno = 1
	for line in f.readlines():
		line = line.strip()
		if not line.startswith('#') and not line == "":
			pos = line.find('=')
			if pos == -1:
				raise CaicosError(str(filename) + " is not a valid caicos config file. Line " + str(lno) + " does not contain '='.") 
			param = line[:pos].strip().lower()
			val = line[pos+1:].strip()
			
			if param in config_specification and config_specification[param][1] != "":
				make_array_or_append(config, config_specification[param][1], val)
			else:
				if param not in config_specification:
					raise CaicosError("File " + str(filename) + " specifies the invalid config parameter '" + str(param) + "' on line " + str(lno))
				config[param] = val
		lno = lno + 1

	#Now check all required parameters are defined
	for p, spec in config_specification.iteritems():
		if spec[0]:
			if not p in config:
				raise CaicosError("Required parameter '" + str(p) + "' was not defined by config file " + str(filename))
		
	#Now validate parameters
	if not os.path.isdir(project_path("dynamic_board_designs", config['targetboard'])):
		raise CaicosError("targetboard is set to an unsupported board: "+ config['targetboard'])
		
	if not 'fpgapart' in config:
		config['fpgapart'] = fpgapart[config['targetboard']]
		
	return config




def __getfakebindings(sigs):
	"""
	Generate a fake bindings dictionary. Only used when in developer mode with dev_softwareonly
	"""
	b = {}
	callid = 0
	for s in sigs:
		b[callid] = s
		callid = callid + 1
	return b



if __name__ == "__main__":
	if len(sys.argv) < 2:
		print "Usage: caicos.py <configfile>"
		sys.exit(1)

	if os.path.exists(sys.argv[1]) and os.path.isfile(sys.argv[1]):
		build_all(load_config(sys.argv[1]))
	else:
		print "File not found: " + str(sys.argv[1])
		sys.exit(1)

