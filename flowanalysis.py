'''
Created on 29 Mar 2015
@author: ian
'''
import copy
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


class BuildCallgraph():
	"""
	Starts at a given AST node and walks the AST finding all instances where a
	FuncCall node is inside a FuncDef node. Creates a graph showing all the functions called from 
	all functions that are defined inside the start node. 
	
	After construction, the scan is executed. This will relate FuncDef objects to FuncCall objects, 
	not functions to functions. To resolve this, call resolve_calls.
	"""
	def __init__(self, startnode):
		self.graph = {}
		self.fns_in_current_node = []
		self.startnode = startnode
		self.scan_ast(('', self.startnode), None)
		
	def scan_ast(self, currentnode, caller):
		currentcaller = caller
		if isinstance(currentnode[1], c_ast.FuncDef):
			currentcaller = currentnode
			self.fns_in_current_node.append(currentnode[1])
		elif isinstance(currentnode[1], c_ast.FuncCall):
			if not currentcaller[1] in self.graph:
				self.graph[currentcaller[1]] = []
			self.graph[currentcaller[1]].append(currentnode[1])

		for c in currentnode[1].children():
			self.scan_ast(c, currentcaller)

	def printgraph(self):
		"""
		Pretty print the output of the call graph
		"""
		for f in self.graph:
			print f.decl.name + " (Defined: " + str(f.coord) + ")"
			for c in self.graph[f]:
				if isinstance(c, c_ast.FuncCall):
					print "\t" + str(c.name.name) + " (Call point: " + str(c.coord) + ")"
				elif isinstance(c, c_ast.FuncDef):
					print "\t" + str(c.decl.name) + " (Defined: " + str(c.coord) + ")"
				else:
					print "\t" + str(c) + " (Location: " + str(c.coord) + ")"
		
	def resolve_calls(self, jamaicaoutputdir):
		"""
		When the call graph is built, it links FuncDef to FuncCall. This turns FuncCall into FuncDef
		by leveraging the simplistic rules of C, that all functions share the same namespace and types
		are not required for disambiguation. This is not sufficient for general purpose code, but will
		be fine with the output of Jamaica builder, which is the intended use.
		
		This can cause a lot of parsing and will be slow, as it is essentially a dumb, slow, linker 
		that is given no clue about where to look.
		"""
		resolved = {}
		
		def parse_files(callee):
			"""
			Parse all the .c files in the output directory looking for a suitable function definition.
			Relies on ASTCache to avoid reparsing the same files again and again. 
			"""
			for srcfile in os.listdir(jamaicaoutputdir):
				if srcfile.endswith(".c") and not srcfile.endswith("Main__.c"):
					ast = astcache.get(os.path.join(jamaicaoutputdir, srcfile))
					fns = functions_defined_in_ast(ast)
					for fn in fns:
						if fn.decl.name == callee.name.name:
							return fn
			return None

		for caller in self.graph:
			listtoresolve = copy.copy(self.graph[caller]) #We will modify this list as we iterate so shallow copy it first
			for callee in listtoresolve:
				if isinstance(callee, c_ast.FuncCall): 
					if not callee in resolved:
						#Check the current file first
						found = None
						for fn in self.fns_in_current_node:
							if fn.decl.name == callee.name.name:
								resolved[callee] = fn
								found = fn
								break			
						if found == None:
							f = parse_files(callee)
							if f == None:
								raise CaicosError("Cannot find the definition of function: " + str(callee.name.name) + ". Called by " + str(caller.decl.name))
							resolved[callee] = f
					self.graph[caller].remove(callee)
					self.graph[caller].append(resolved[callee])
	
	def reachable_functions(self, startfunc):
		"""
		Given a starting function (a FuncDef AST node), find all the functions reachable from it.
		Returns a set containing, at least, the starting function.
		"""
		def reachable_functions_recurse(fn, rv):
			if fn in rv: return rv
			rv.add(fn)
			if fn in self.graph:
				for callee in self.graph[fn]:
					rv.union(reachable_functions_recurse(callee, rv))
			return rv
		
		if not isinstance(startfunc, c_ast.FuncDef): return None
		if not startfunc in self.graph: return set([startfunc])
		return reachable_functions_recurse(startfunc, set())
	
	

	
def test_reachable_functions(ast):
	bcg = BuildCallgraph(ast)
	bcg.printgraph()
	print
	bcg.resolve_calls("/Users/ian/Dropbox/Java/juniperworkspace2/caicos/tests")
	bcg.printgraph()
	print
	
	class FnVis(c_ast.NodeVisitor):
		def visit_FuncDef(self, node):
			if node.decl.name == "main":
				self.found = node
	fv = FnVis()
	fv.visit(ast)
	
	
	rfs = bcg.reachable_functions(fv.found)
	print "Reachable functions:"
	files = set()
	for fn in rfs: 
		print "\t" + str(fn.decl.name) + " - " + str(fn.decl.coord.file)
		files.add(fn.decl.coord.file)
	print "Files: " + str(files)
