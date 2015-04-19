"""
The main caicos module.

Contains the main entry point to caicos, with routines to build the hardware and software projects, 
and to read and parse configuration files.

Call build_all() to begin.
"""

import os
import shutil

import prepare_hls_project
import prepare_src_project
from utils import mkdir, CaicosError


"""
Describes the required options in the configuration of caicos.
All valid options must be enumerated in this dictionary. Each option name maps
to a tuple of (required, arrayname) where required is a boolean that is True
if the option is mandatory. If arrayname is not "", then the option will be
added as a member of an array with that name.

Multiple definitions of the same variable are valid, and the last definition
takes precedence.

Summary of the config options:
	Required:
		signatures - list of Java method signatures to be compiled to hardware
		jamaicabuilderoutputdir - path to compiled C files from jamaicabuilder
		outputdir - path to directory to output to
		fpgapart - the FPGA part to target, in Xilinx format
		jamaicatarget - path to the Jamaica target directory to use for the host software  
	
	Optional:
		additionalhardwarefiles - list of paths to additional files to add to the hardware project 
"""
config_specification = {
	'signatures': (True, ""),
	'jamaicabuilderoutputdir': (True, ""),
	'additionalhardwarefiles': (False, ""),
	'outputdir': (True, ""),
	'fpgapart': (True, ""),
	'jamaicatarget': (True, ""),
	
	'signature': (False, "signatures"),
	'additionalhardwarefile': (False, "additionalhardwarefiles"),
}




def build_all(config):
	"""
	Build a JUNIPER project. Format of the config dictionary is described in the docstring for config_specification.
	"""
	cwd = os.path.dirname(os.path.realpath(__file__))
	mkdir(config['outputdir'])
	
	bindings = prepare_hls_project.build_from_functions(
		config['signatures'], 
		config['jamaicabuilderoutputdir'],
		os.path.join(config['outputdir'], "hardware"), 
		config.get('additionalhardwarefiles'), 
		config['fpgapart'])

	shutil.copyfile(os.path.join(cwd, "projectfiles", "scripts", "push.sh"), os.path.join(config['outputdir'], "hardware", "push.sh"))
	
	
	prepare_src_project.build_src_project(
		bindings,
		config['jamaicabuilderoutputdir'],
		os.path.join(config['outputdir'], "software"),
		config['jamaicatarget'])

	shutil.copyfile(os.path.join(cwd, "projectfiles", "scripts", "juniperbuild.sh"), os.path.join(config['outputdir'], "software", "juniperbuild.sh"))
	



	
	
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

	config = {}

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
		
	return config

