'''
Created on 29 Mar 2015
@author: ian
'''
"""
Some function calls can be ignored when attempting to resolve FuncCalls to FuncDefs because
we will not ever find their implementation. i.e. They are in the standard library.
"""

import os

from pycparser import c_ast

import astcache
from juniperrewrites import c_filename_of_java_method_sig
from utils import log, deglob_file


calls_to_ignore = ["printf", "sprintf",

#Many Math.h functions are provided by HLS
	"sin", "cos", "tan", "asin", "acos", "atan", "atan2", 
	"sinh", "cosh", "tanh", 
	"sinf", "cosf", "tanf", "asinf", "acosf", "atanf", "atan2f", 
	"fabsf", "floorf", "ceilf", "sqrtf", "powf", "logf", "expf", "rintf", 
	"fabs", "floor", "ceil", "sqrt", "pow", "log", "exp", "rint",
	
#No need to resolve in the porting layer as we will be including that anyway
	"jamaicaThreads_checkCStackOverflow",
	"jamaicaGC_PlainAllocHeadBlock",
	"jamaicaInterpreter_allocSimpleArray",
	"jamaicaInterpreter_allocJavaObject",
	"jamaicaGC_GetArray32", "jamaicaGC_SetArray32", 
	"jamaicaGC_GetArray16", "jamaicaGC_SetArray16", 
	"jamaicaGC_GetArray8", "jamaicaGC_SetArray8", 
	"jamaicaGC_GetRefArray", "jamaicaGC_SetRefArray",
	"resolve_tree_array",
	"jamaicaExceptions_throw",
	"jamaica_throwNullPtrExc", "jamaica_throwIncompClassChangeErr", "jamaica_throwAbstrMethodErr",
	"jamaica_throwClassCastExc", "jamaica_throwArrIdxOutOfBndsExc", "jamaica_throwArrIdxOutOfBndsExcMsg",
	"jamaica_throwOutOfMemErr", "jamaica_throwNegArrSzExc", "jamaica_throwArithmExc",
	"jamaica_throwIllMonitorStateExc", "jamaica_throwArrStoreExc", "jamaica_throwNoClassDefFndErr"
	"jamaica_throwInterruptedExc", "jamaica_throwInternalErr", "jamaica_throwInternalErrMsg"
	"jamaica_throwInternalErrcmsg", "jamaica_throwIllArgExccmsg", "jamaica_throwStringIdxOutOfBndsExc"
	"jamaica_throwIllAssgmtErr", "jamaica_throwStackOvrflwErr",
	"jamaicaInterpreter_enterMonitor", "jamaicaInterpreter_exitMonitor",
]


resolutioncache = {}


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
	
	Note that this class uses a global resolution cache, so that resolutions can be reused through the project preparation 
	process. 
	"""
	def __init__(self, startfndef, filestosearch):
		self.filestosearch = filestosearch
		self.reachable_functions = set()
		self.func_defs_seen = []
		self.files = set()
		
		self.find_reachable_functions(startfndef)
		self.get_files()
		
	
	def resolve_call(self, call):
		"""
		Given a FuncCall node, return the corresponding FuncDef node.
		"""
		def parse_files(calltofind):
			"""
			Parse all the .c files provided looking for a suitable function definition.
			Relies on ASTCache to avoid reparsing the same files again and again. 
			"""
			log().info("Resolving call to: " + call.name.name)
			
			for srcfile in self.filestosearch:
				ast = astcache.get(srcfile)
				fns = functions_defined_in_ast(ast)
				for fn in fns:
					if fn.decl.name == calltofind.name.name:
						return fn
			return None

		f = parse_files(call)
		if f == None:
			log().warning("Cannot find the definition of function: " + str(call.name.name))
			return None
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
			if isinstance(call.name, c_ast.Cast):
				log().warning("A cast to a function type detected at " + os.path.basename(call.coord.file) + ":" + str(call.coord.line) + ". Flow analysis may not be complete.")
			else:
				callname = str(call.name.name)
				if not callname in calls_to_ignore:			
					if not callname in resolutioncache:
						resolutioncache[callname] = self.resolve_call(call)
					if resolutioncache[callname] != None:
						self.reachable_functions.add(resolutioncache[callname])
						self.find_reachable_functions(resolutioncache[callname])


	def get_files(self):
		"""
		Work out the set of files that contains all the reachable source code.
		"""
		for fn in self.reachable_functions: 
			self.files.add(fn.decl.coord.file)
	
	

def get_files_to_search(sig, jamaicaoutputdir):
	"""
	When performing flow analysis, the search order of files will hugely affect the execution time
	because almost all referenced functions will be in the current file (i.e. Java package), the 
	FPGA porting layer, or the java.lang package or similar.
	
	This function returns a list containing all of the files in the Jamaica output directory and the
	porting layer. However it orders it based on heuristics to try to ensure that resolutions are faster.
	"""
	filestosearch = []
	
	#First, put the originating file as this is the most likely source
	filestosearch.append(c_filename_of_java_method_sig(sig, jamaicaoutputdir))
	
	#Then the FPGA porting layer
	cwd = os.path.dirname(os.path.realpath(__file__))	
	filestosearch.append(os.path.join(cwd, "projectfiles", "src", "fpgaporting.cpp"))
	
	#Then the java.lang package
	filestosearch.append(deglob_file(os.path.join(jamaicaoutputdir, "PKG_java_lang_V*__.c")))
	
	#Then the other output C files
	for f in os.listdir(jamaicaoutputdir):
		ffullpath = os.path.join(jamaicaoutputdir, f)
		if ffullpath.endswith(".c") and (not ffullpath.endswith("Main__.c")) and not ffullpath in filestosearch: 
			filestosearch.append(ffullpath)
			
	return filestosearch


	
def test_reachable_functions(ast):
	class FnVis(c_ast.NodeVisitor):
		def visit_FuncDef(self, node):
			if node.decl.name == "main":
				self.found = node
	fv = FnVis()
	fv.visit(ast)
	
	rv = ReachableFunctions(fv.found, "/Users/ian/Dropbox/Java/juniperworkspace2/caicos/tests")

	print "Reachable functions:"
	for fn in rv.reachable_functions: 
		print "\t" + str(fn.decl.name) + " - " + str(fn.decl.coord.file)
	print "Files: " + str(rv.files)
