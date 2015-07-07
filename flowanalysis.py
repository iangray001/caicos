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
from utils import log, deglob_file, CaicosError, project_path


excluded_functions = ['jamaicaGC_PlainAllocHeadBlock', 'jamaicaInterpreter_allocJavaObject', 'jamaicaInterpreter_allocSimpleArray', 
					'jamaicaInterpreter_allocMultiArray', 'jamaicaInterpreter_initialize_class_helper', 
					'jamaicaInterpreter_enterMonitor', 'jamaicaInterpreter_exitMonitor', 'jamaicaInterpreter_getInterfaceMethod']

calls_to_ignore = ["printf", "sprintf",

#Many Math.h functions are provided by HLS
	"sin", "cos", "tan", "asin", "acos", "atan", "atan2", 
	"sinh", "cosh", "tanh", 
	"sinf", "cosf", "tanf", "asinf", "acosf", "atanf", "atan2f", 
	"fabsf", "floorf", "ceilf", "sqrtf", "powf", "logf", "expf", "rintf", 
	"fabs", "floor", "ceil", "sqrt", "pow", "log", "exp", "rint",
	
#No need to resolve in the porting layer as we will be including that anyway
	"jamaicaThreads_checkCStackOverflow",
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
	"jamaica_throwIllAssgmtErr", "jamaica_throwStackOvrflwErr"
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


def functions_declared_in_ast(ast):
	"""
	Returns a list of all the function declarations in a given AST
	"""
	class FnVis(c_ast.NodeVisitor):
		def __init__(self):
			self.rv = []
		def visit_FuncDecl(self, node):
			self.rv.append(node)
	f = FnVis()
	f.visit(ast)
	return f.rv

	
class ReachableFunctions(object):
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
		self.reachable_non_translated = set()
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
				if callname in excluded_functions:
					self.reachable_non_translated.add(callname)
				else:
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
	

	def __str__(self):
		s = ""
		for f in self.reachable_functions:
			s = s + str(f.decl.name) + ": " + str(f.coord.file) + "\n"
		return s
	
	

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
	filestosearch.append(project_path("projectfiles", "src", "fpgaporting.c"))
	
	#Then the java.lang package
	filestosearch.append(deglob_file(os.path.join(jamaicaoutputdir, "PKG_java_lang_V*__.c")))
	
	#Then the other output C files
	for f in os.listdir(jamaicaoutputdir):
		ffullpath = os.path.join(jamaicaoutputdir, f)
		if ffullpath.endswith(".c") and (not ffullpath.endswith("Main__.c")) and not ffullpath in filestosearch: 
			filestosearch.append(ffullpath)
			
	return filestosearch



def get_funcdecl_of_system_funccall(call):
	"""
	Given a FuncCall to a Jamaica system call (or just the string of its name), return the corresponding FuncDecl node.
	"""
	ast = astcache.get(project_path("projectfiles", "include", "jamaica.h"))
	fdecs = functions_declared_in_ast(ast)
	
	if isinstance(call, c_ast.FuncCall):
		call = call.name.name
	
	for dec in fdecs:
		if dec.parent.name == call:
			return dec
	raise CaicosError("Cannot find the definition of system function: " + str(call))
