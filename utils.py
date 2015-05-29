import glob
import logging
import os
import shutil
import sys

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
	if not os.path.exists(d): os.makedirs(d)
	
	

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
	
	
