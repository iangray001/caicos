import glob
import logging
import os
import sys

import pycparser


theLogger = None


class CaicosError(Exception):
	"""
	Indicates that an unrecoverable error was observed during processing.
	"""
	pass


def log():
	global theLogger
	if theLogger == None:
		theLogger = logging.getLogger("Caicos")
		ch = logging.StreamHandler(sys.stdout)
		formatter = logging.Formatter('%(levelname)s:\t%(message)s') #%(asctime)s can be used if time is required
		ch.setFormatter(formatter)
		theLogger.addHandler(ch)
		theLogger.setLevel(logging.WARN) #This is the default log level. Can be changed in scripts if necessary.
	return theLogger


def getlineoffile(filename, lineno):
	"""
	Return a string of line number lineno of file filename. This is a slow operation
	and should be called sparingly.
	"""
	try:
		fp = open(filename)
		for i, line in enumerate(fp):
			if i == (lineno - 1):
				return line
	finally:
		fp.close()


def deglob_file(globbedname):
	"""
	Jamaicabuilder outputs C files of the form PKG_<Java package name>_V<Some hex>.c
	As we don't know what the hex is, we can glob it and warn if more than one file matches.
	"""
	matches = glob.glob(str(globbedname))
	if len(matches) == 0:
		logging.warning("The filename \"" + str(globbedname) + "\" does not match any files.")
		return None
	if len(matches) > 1:
		logging.warning("Multiple matches for filename " + str(globbedname) + "\"")
		logging.warning("Using: " + matches[0])
		return matches[0]
	return matches[0]



def parse_jamaica_output(filename, includepath = None):
	"""
	Use pycparser to parse a Jamaica output file.
	Because Jamaica's output is pretty complex, cpp is required, and even still 
	some features have to be removed because it uses GCC extensions unsupported by pycparser.
	Returns a pycparser.c_ast or throws ParseError
	
	If includepath is None then the project include files are used for parsing. Else, an absolute
	path to alternate includes can be provided.
	"""
	if '*' in filename:
		filename = deglob_file(filename)
	
	cwd = os.path.dirname(os.path.realpath(__file__))

	cppargs = ['-E', 
			   '-DNDEBUG', #Disable Jamaica debug support 
			   '-U__GNUC__', #Prevents use of __attribute__, will cause an "unsupported compiler" #warning
			   '-W#warnings', #And this hides that warning
			   '-DJAMAICA_NATIVE_TIME_GET_HIGH_RESOLUTION_TIMESTAMP', #These are inline asm which seems not supported by pycparser
			   '-DJAMAICA_NATIVE_THREAD_COMPARE_AND_SWAP',
			   r'-I' + os.path.join(cwd, "stdlibheaders"), #Override stdlib headers with blank versions (Jamaica builder doesn't use them, but includes them)
			   ]
	
	if includepath == None: cppargs.append(r'-I' + os.path.join(cwd, "projectfiles", "include"))
	else: cppargs.append(r'-I' + str(includepath))
	
	return pycparser.parse_file(filename, use_cpp=True, cpp_path="gcc", cpp_args=cppargs)


def write_ast_to_file(ast, filename, prefix = None):
	"""
	Run the provided AST through pycparser's C generator and write to filename. Will
	overwrite if filename exists. Will fail if filename points into a directory which 
	does not exist.
	
	If prefix is not None then it is prepended onto the code before being written.
	"""
	generator = pycparser.c_generator.CGenerator()
	code = generator.visit(ast)
	if prefix != None: code = prefix + code
	outf = open(filename, "w+")
	try:
		outf.writelines(code)
	finally:
		outf.close()
	
	
