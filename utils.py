import re
import sys, glob

from pycparser import c_ast
import pycparser

import utils


class CaicosException(Exception):
	pass


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
		print("The filename \"" + str(globbedname) + "\" does not match any files.")
		sys.exit(-1)
	if len(matches) > 1:
		print("Warning: Multiple matches for filename " + str(globbedname) + "\"")
		print("Using: " + matches[0])
		return matches[0]
	return matches[0]


def get_original_name_of_fn(ast, funcname):
	"""
	Search for a function definition of name funcname. If found, extract the original 
	Java version of the function (which will be inserted as a block comment by Jamaica)
	"""
	class FuncDefVisitor(c_ast.NodeVisitor):
		def __init__(self):
			self.found = None
		
		def visit_FuncDef(self, node):
			pattern = re.compile(
				r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
				re.DOTALL | re.MULTILINE
			)        
			line = utils.getlineoffile(node.decl.coord.file, node.decl.coord.line)
			comments = re.findall(pattern, line)
			if len(comments) == 1:
				c = comments[0]
				if c[:2] == "/*" and c[-2:] == "*/":
					origname = c[2:-2].strip()
					if funcname == None:
						print "Function " + node.decl.name    + " originally: " + origname
					else:
						if node.decl.name == funcname:
							self.found = origname
				
	v = FuncDefVisitor()
	v.visit(ast)
	return v.found


def parse_jamaica_output(filename):
	"""
	Use pycparser to parse a Jamaica output file.
	Because Jamaica's output is pretty complex, cpp is required, and even still 
	some features have to be removed because it uses GCC extensions unsupported by pycparser.
	Returns a pycparser.c_ast or throws ParseError
	"""
	if '*' in filename:
		filename = deglob_file(filename)
	
	cppargs = ['-E', 
			   '-DNDEBUG', #Disable Jamaica debug support 
			   '-U__GNUC__', #Prevents use of __attribute__, will cause an "unsupported compiler" #warning
			   '-W#warnings', #And this hides that warning
			   '-DJAMAICA_NATIVE_TIME_GET_HIGH_RESOLUTION_TIMESTAMP', #These are inline asm which seems not supported by pycparser
			   '-DJAMAICA_NATIVE_THREAD_COMPARE_AND_SWAP',
			   r'-I/Users/ian/Documents/pycparser/utils/fake_libc_include', #Override standard libc with pycparser's minimal version
			   #r'-I/Users/ian/Documents/jamaica/current/target/linux-x86_64/include'
			   r'-I/Users/ian/Dropbox/Work/VivadoHLS/jamaica/include'
			   ]    
	
	return pycparser.parse_file(filename, use_cpp=True, cpp_path="gcc", cpp_args=cppargs)


def write_ast_to_file(ast, filename):
	"""
	Run the provided AST through pycparser's C generator and write to filename. Will
	overwrite if filename exists. Will fail if filename points into a directory which 
	does not exist.
	"""
	generator = pycparser.c_generator.CGenerator()
	code = generator.visit(ast)
	outf = open(filename, "w+")
	try:
		outf.writelines(code)
	finally:
		outf.close()
	
	
