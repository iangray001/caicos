'''
Created on 29 Mar 2015
@author: ian
'''
import os

from pycparser import c_ast

import astcache
from utils import log, CaicosError


def functions_defined_in_ast(ast):
	"""
	Returns a list of all the function definitions in a given AST
	"""
	class FnVis(c_ast.NodeVisitor):
		def __init__(self):
			self.rv = []
		def visit_FuncDef(self, node):
			self.rv.append(node)
	f = FnVis()
	f.visit(ast)
	return f.rv


	
class ReachableFunctions():
	"""
	Starting at a given FuncDef node, recursively walks down the AST finding FuncCall nodes. For each node, attempts to
	resolve that call into its corresponding FuncDef node. It does this by parsing all the C files in the provided
	source folder, meaning that this could be very slow.
	
	This relies on the simple binding rules of C that all functions share the same namespace (no overloading). 
	Whilst this is unlikely to work on all real code due to GCC extensions and the new C std supporting limited overloading, 
	this will work on the output of Jamaica Builder which does not use any of this. 
	"""
	def __init__(self, startfndef, srcdir):
		self.srcdir = srcdir
		self.resolutioncache = {}
		self.reachable_functions = set()
		self.func_defs_seen = []
		
		self.find_reachable_functions(startfndef)
		
	
	def resolve_call(self, call):
		"""
		Given a FuncCall node, return the corresponding FuncDef node.
		"""
		def parse_files(calltofind):
			"""
			Parse all the .c files in the output directory looking for a suitable function definition.
			Relies on ASTCache to avoid reparsing the same files again and again. 
			"""
			for srcfile in os.listdir(self.srcdir):
				if srcfile.endswith(".c") and not srcfile.endswith("Main__.c"):
					ast = astcache.get(os.path.join(self.srcdir, srcfile))
					fns = functions_defined_in_ast(ast)
					for fn in fns:
						if fn.decl.name == calltofind.name.name:
							return fn
			return None

		#TODO: Check the current file first as an optimisation?
		f = parse_files(call)
		if f == None:
			raise CaicosError("Cannot find the definition of function: " + str(call.name.name))
		return f
	
	def find_reachable_functions(self, fndef):
		#Prevent loops
		if fndef in self.func_defs_seen:
			return
		self.func_defs_seen.append(fndef)
			
		#Find all calls in the function definition
		callsfound = []
		def search_for_fcall(node):
			if isinstance(node, c_ast.FuncCall):
				callsfound.append(node)
			for c in node.children():
				search_for_fcall(c[1])
		search_for_fcall(fndef)

		#Resolve the calls into function definitions
		for call in callsfound:
			if not call in self.resolutioncache: 
				self.resolutioncache[call] = self.resolve_call(call)
			self.reachable_functions.add(self.resolutioncache[call])
			self.find_reachable_functions(self.resolutioncache[call])

	
	
def test_reachable_functions(ast):
	class FnVis(c_ast.NodeVisitor):
		def visit_FuncDef(self, node):
			if node.decl.name == "main":
				self.found = node
	fv = FnVis()
	fv.visit(ast)
	
	rv = ReachableFunctions(fv.found, "/Users/ian/Dropbox/Java/juniperworkspace2/caicos/tests")

	print "Reachable functions:"
	files = set()
	for fn in rv.reachable_functions: 
		print "\t" + str(fn.decl.name) + " - " + str(fn.decl.coord.file)
		files.add(fn.decl.coord.file)
	print "Files: " + str(files)
