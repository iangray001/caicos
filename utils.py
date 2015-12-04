"""
Utility functions that are not related specifically to parsing or code generation.

Includes error handling and info logging, and file system manipulation.
"""

import glob
import logging
import os, stat
import shutil
import sys

import pycparser


theLogger = logging.getLogger("Caicos")
ch = logging.StreamHandler(sys.stdout)
formatter = logging.Formatter('%(levelname)s:\t%(message)s') #%(asctime)s can be used if time is required
ch.setFormatter(formatter)
theLogger.addHandler(ch)
theLogger.setLevel(logging.WARN) #This is the default log level. Can be changed in scripts if necessary.


class CaicosError(Exception):
	"""
	Indicates that an unrecoverable error was observed during processing.
	"""
	pass


def log():
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
		raise CaicosError("The filename \"" + str(globbedname) + "\" does not match any files.")
	if len(matches) > 1:
		logging.warning("Multiple matches for filename " + str(globbedname) + "\"")
		logging.warning("Using: " + matches[0])
		return matches[0]
	return matches[0]


	
def mkdir(d):
	"""
	If the directory d does not exist, create it
	"""
	if not os.path.exists(d): 
		os.makedirs(d)
	
	

def copy_files(srcdir, targetdir, suffixes = None):
	"""
	Copy the directory of files from srcdir to targetdir.
	Only copies files that share a suffix with a string from suffixes
	"""
	mkdir(targetdir)
	
	for f in os.listdir(srcdir):
		abspath = os.path.join(srcdir, f)
		if os.path.isfile(abspath):
			if suffixes != None:
				for suff in suffixes:
					if abspath.endswith(suff):
						shutil.copyfile(abspath, os.path.join(targetdir, f))
						break
			else:
				shutil.copyfile(abspath, os.path.join(targetdir, f))
		elif os.path.isdir(abspath):
			mkdir(os.path.join(targetdir, f))
			copy_files(abspath, os.path.join(targetdir, f), suffixes) 
		
	
def project_path(*path_parts):
	"""
	Returns the project directory as an absolute path with path_parts appended.
	Used to more easily reference files in the project directory.
	"""
	parts = [os.path.dirname(os.path.realpath(__file__))]
	parts.extend(path_parts)
	return os.path.join(*parts)
	
	
def make_executable(targets):
	"""
	Stat an array of file absolute paths to make them executable
	"""
	for f in targets:
		try:
			os.chmod(f, os.stat(f).st_mode | stat.S_IEXEC)
		except OSError as e:
			log().warning("Error whilst making scripts executable: " + str(e)) 
	

def remove_slots_from_classes(module):
	"""
	When passed a module, iterate through the classes in that module and replace any classes which use 
	__slots__ with a copy that is otherwise identical but doesn't define __slots__.
	
	This is because pycparser recently updated to use __slots__ and this prevents the use of pickle and
	prevents caicos from adding in parent links (astcache.add_parent_links).
	
	This is clearly a large monkey-patch, but the nature of __slots__ means that it is actually
	very difficult to make optional (classes need to be wrapped in a factory which selectively returns
	the __slots__ version or not). As this a significant change to pycparser, this is the path of least
	resistance.
	"""
	for name, cls in module.__dict__.iteritems():
		if isinstance(cls, type):
			clonedict = dict(cls.__dict__) #Copy the target class' dictionary
			if '__slots__' in clonedict:
				#Delete the member_descriptors that __slots__ adds
				if isinstance(clonedict['__slots__'], "".__class__):
					del clonedict[clonedict['__slots__']]
				else:
					for m in clonedict['__slots__']:
						del clonedict[m]
				del clonedict['__slots__'] #And delete the __slots__ for good measure
				replacement = type(cls.__name__, cls.__bases__, clonedict) #Return a new class with the amended interface
				setattr(module, name, replacement)


def codegen(node):
	"""
	Utility to make it easier to codegen an AST node
	"""
	return pycparser.c_generator.CGenerator().visit(node)
