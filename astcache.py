"""
The ASTCache module helps to prevent unnecessary re-parsing, which can be 
very slow when using pycparser.

Maintains a dictionary of {filenames -> ASTs}

Call get(filename) to get an AST. The file will be parsed if this is the
first request, otherwise the previously generated AST will be returned.   

If code is being modified invalidate_ast_for(filename) can be used to 
remove a cached AST and force a reparse.
"""

import os

import pycparser
from pycparser.plyparser import ParseError

from utils import log, CaicosError, deglob_file


cache = {}

def get(filename, alternateincludepath = None):
	try:
		if filename in cache:
			return cache[filename]
		else:
			log().info("Parsing " + str(filename))
			ast = parse_jamaica_output(filename, alternateincludepath)
			add_parent_links(ast)
			cache[filename] = ast
			return ast
	except ParseError, pe:
		raise CaicosError("Parse error in file '" + str(filename) + "'.\n\t" + str(pe.message))
		
		
def invalidate_ast_for(filename):
	if filename in cache:
		del cache[filename]


def clear():
	cache.clear()


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
			   '-DJAMAICA_NATIVE_TIME_GET_HIGH_RESOLUTION_TIMESTAMP', #These use volatile asm() which is not supported by pycparser
			   '-DJAMAICA_NATIVE_THREAD_COMPARE_AND_SWAP',
			   '-D__CAICOS__', #So we can tell if we are being parsed by caicos
			   r'-I' + os.path.join(cwd, "stdlibheaders"), #Override stdlib headers with blank versions (Jamaica builder doesn't use them, but includes them)
			   ]
	
	if includepath == None: cppargs.append(r'-I' + os.path.join(cwd, "projectfiles", "include"))
	else: cppargs.append(r'-I' + str(includepath))
	
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



	