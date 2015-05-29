"""
The ASTCache module helps to prevent unnecessary re-parsing, which can be 
very slow when using pycparser.

Maintains a dictionary of {filenames -> ASTs}

Call get(filename) to get an AST. The file will be parsed if this is the
first request, otherwise the previously generated AST will be returned.   

If code is being modified invalidate_ast_for(filename) can be used to 
remove a cached AST and force a reparse.
"""

import errno
import os

import pycparser
from pycparser.plyparser import ParseError

import astcache
import cPickle as pickle
from utils import log, CaicosError, deglob_file, project_path, mkdir


filecache = None
memcache = {}

def get(filename, alternateincludepath = None):
	"""
	Get the AST for the file pointed to by the absolute path filename
	If alternateincludepath is provided then a different include path is passed to the
	preprocessor before parsing
	"""
	try:
		if filename in memcache:
			return memcache[filename]
		else:
			cachedast = fetch_from_filecache(filename)
			if cachedast != None:
				memcache[filename] = cachedast
				return cachedast
			
			log().info("Parsing " + str(filename))
			ast = parse_jamaica_output(filename, alternateincludepath)
			add_parent_links(ast)
			memcache[filename] = ast
			save_to_filecache(filename, ast)
			return ast
	except ParseError, pe:
		raise CaicosError("Parse error in file '" + str(filename) + "'.\n\t" + str(pe.message))
		
		
def invalidate_ast_for(filename):
	"""
	If the filename has been parsed and is in memcache, this cached AST is deleted.
	"""
	if filename in memcache:
		del memcache[filename]


def clear():
	"""
	Clear the memcache.
	"""
	memcache.clear()


def fetch_from_filecache(filename):
	"""
	Checks to see if we have a cached version of the AST in the file cache
	If filecache is None then immediately returns None
	If no suitable file exists, returns None
	Else returns the requested AST.
	"""
	if filecache != None:
		if os.path.isfile(cache_filename(filename)):
			sourcefile_mtime = os.stat(filename).st_mtime
			cache_mtime = os.stat(cache_filename(filename))
			if cache_mtime > sourcefile_mtime:
				log().info("Using cached AST for " + str(filename))
				return pickle.load(open(cache_filename(filename), 'rb'))
	else:
		return None
	
def save_to_filecache(filename, ast):
	"""
	Save the provided AST (which was generated from the provided filename) to the file cache
	"""
	if filecache != None:
		cdir = os.path.dirname(cache_filename(filename))
		try:
			os.makedirs(cdir)
		except OSError as exc:
			if exc.errno == errno.EEXIST and os.path.isdir(cdir):
				pass
			else: raise
			
		f = open(cache_filename(filename), 'wb')
		pickler = pickle.Pickler(f, protocol=pickle.HIGHEST_PROTOCOL)
		pickler.dump(ast)
		f.close()	
	
	
def cache_filename(sourcefile):
	"""
	Return the filename that should be used in the file cache for the given source file 
	"""
	if sourcefile.startswith(os.sep):
		sourcefile = sourcefile[len(os.sep):]
	return os.path.join(filecache, sourcefile)
	

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
	
	cppargs = ['-E', 
			   '-DNDEBUG', #Disable Jamaica debug support 
			   '-U__GNUC__', #Prevents use of __attribute__, will cause an "unsupported compiler" #warning
			   '-W#warnings', #And this hides that warning
			   '-DJAMAICA_NATIVE_TIME_GET_HIGH_RESOLUTION_TIMESTAMP', #These use volatile asm() which is not supported by pycparser
			   '-DJAMAICA_NATIVE_THREAD_COMPARE_AND_SWAP',
			   '-D__CAICOS__', #So we can tell if we are being parsed by caicos
			   r'-I' + project_path("stdlibheaders"), #Override stdlib headers with blank versions (Jamaica builder doesn't use them, but includes them)
			   ]
	
	if includepath == None: 
		cppargs.append(r'-I' + project_path("projectfiles", "include"))
	else: 
		cppargs.append(r'-I' + str(includepath))
	
	return pycparser.parse_file(filename, use_cpp=True, cpp_path="gcc", cpp_args=cppargs)

	
def add_parent_links(ast):
	"""
	Iterate over an AST annotating every node with a link to its parent node.
	"""
	def recurse(node, parent):
		node.parent = parent
		for _, c in node.children():
			recurse(c, node)
	recurse(ast, None)


def activate_cache(path):
	"""
	Turn on the file cache by providing an absolute path to a directory.
	ASTs are pickled here once created and used to speed subsequent runs of caicos.
	"""
	mkdir(path)
	astcache.filecache = path

	