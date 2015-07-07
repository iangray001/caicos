import os
import re

from pycparser import c_ast
import pycparser
from pycparser.c_generator import CGenerator 

from astcache import add_parent_links
import astcache
from utils import CaicosError, log
import utils

RAM_NAME = '__juniper_ram_master'
RAM_GET_NAME = 'juniper_ram_get_'
RAM_SET_NAME = 'juniper_ram_set_'
MAX_SYSCALL_ARGS = 5
SYSCALL_NAME = 'pcie_syscall'

logreplacements = False

def replace_node(orignode, newnode):
	"""
	Replace the node orignode with the node newnode
	"""
	for (child_name, child) in orignode.parent.children():
		if child == orignode:
			if '[' in child_name:
				arrayname = child_name[:child_name.find('[')]
				arrayidx = child_name[child_name.find('[') + 1:child_name.find(']')]
				try:
					arrayidx = int(arrayidx)
				except:
					raise CaicosError(str(arrayidx) + " is not a valid array index in replace_node")
				
				getattr(orignode.parent, arrayname)[arrayidx] = newnode
			else:
				setattr(orignode.parent, child_name, newnode)

				

def rewrite_RAM_structure_dereferences(ast):
	"""
	The replacement Jamaica headers use RAM_NAME as a global pointer to the external bus interface. 
	Unfortunately, JamaicaBuilder assumes that the type of this pointer is a complex 
	union-of-structs-of-arrays. This rewrites such accesses to use a set of memory access functions
	declared in fpgaporting.h that allow a more HLS-friendly representation of RAM.
	We are looking for structures like the following:
	  ArrayRef:
		StructRef: .
		  ArrayRef: 
			ID: __juniper_ram_master
			<Anything else>
		  ID: f
		Constant: int, 2
	"""
	def rewrite_structref(node):
		"""
		Called by recurse to check a StructRef node for if rewrites are necessary, and if so, make them. 
		"""
		deref_type = node.field.name  # What was dereferenced ('i', 's', 'c', 'r', etc.)

		# When a subword type, there may be a higher level arrayref :- __juniper_ram_master[x].s[4]
		# Save the '4' ast in top_array_deref
		top_array_deref = None
		if isinstance(node.parent, c_ast.ArrayRef) and not node == node.parent.subscript:
			top_array_deref = node.parent.subscript
			nodetoreplace = node.parent
		else:
			nodetoreplace = node	
		
		arf = node.name

		if isinstance(nodetoreplace.parent, c_ast.Assignment) and nodetoreplace.parent.op == "=" and nodetoreplace.parent.lvalue == nodetoreplace:
			callname = RAM_SET_NAME
			callargs = [arf.subscript, top_array_deref, nodetoreplace.parent.rvalue]
			nodetoreplace = nodetoreplace.parent  # The top level Assignment 
		else: 
			callname = RAM_GET_NAME
			callargs = [arf.subscript, top_array_deref]
		callname = callname + str(deref_type)
		
		replacementnode = make_func_call_node(callname, callargs)
		replace_node(nodetoreplace, replacementnode)

	def recurse(node):
		"""
		Recurse depth-first through the provided AST down to the leaf nodes.
		Then, whilst popping off the stack, any StructRef nodes are passed to rewrite_structref
		If changes are made, returns True
		"""
		changemade = False
		for c in node.children(): 
			if recurse(c[1]):
				changemade = True
		if isinstance(node, c_ast.StructRef):
			if node.type == "." and isinstance(node.name, c_ast.ArrayRef) and node.name.name.name == RAM_NAME:
				rewrite_structref(node)
				changemade = True
		return changemade
				
	while recurse(ast): 
		# After moving nodes we need to correct the parent links. 
		# TODO: Potential optimisation to fix the links only as they are moved, but this isn't too slow
		add_parent_links(ast)



def c_filename_of_javaclass(classname, c_output_base, classdelimiter='.'):
	"""
	The C file containing a given Java class is only semi-predictable, we need to glob for it.
	Java class format x.y.classname is translated to an absolute file pathname.
	Jamaica translates on a per-package basis, so many classes may be in the same C file.
	"""
	parts = classname.strip().split(classdelimiter)
	if not os.path.exists(c_output_base): 
		return None

	if len(parts) < 2:
		raise CaicosError("Class name " + str(classname) + " appears to reference an unpackaged class, which is not supported by JamaicaBuilder or modern Java.")
	
	name = "PKG_"
	for part in parts[:-1]:
		name = name + part + "_"
	name = name + "V*__.c"

	try:
		matches = utils.deglob_file(os.path.join(c_output_base, name))
	except CaicosError, _:
		raise CaicosError("No source file was found for class '" + str(classname) + "'. Are the bindings in the config file correct?")

	return matches



def c_filename_of_java_method_sig(sig, c_output_base):
	"""
	Return the C filename that contains the translation of the provided Java method signature.
	Does not check that the signature is valid.
	This is easy, because the entire package is translated into a single C file so we only need 
	to strip out the type name (class name) and convert that.
	"""
	typename = sig[:sig.find('.')]
	return c_filename_of_javaclass(typename, c_output_base, '/')



def c_name_of_java_method(sig, c_output_base):
	"""
	Find the name of the C method into which the given method signature was translated.
	This is only partly deterministic so we need to search the AST.
	"""
	sig = sig.strip()
	filetoparse = c_filename_of_java_method_sig(sig, c_output_base)
	ast = astcache.get(filetoparse)
	names = get_java_names_of_C_fns(ast)
	return names[sig]
	
	
	
def get_java_names_of_funcdef(funcdef):
	"""
	Given a c_ast.FuncDef node, determine the Java signature from which it was generated
	Returns (java_method_sig, (c_fn_name, funcdef))
	"""
	pattern = re.compile(
		r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
		re.DOTALL | re.MULTILINE
	)        
	line = utils.getlineoffile(funcdef.decl.coord.file, funcdef.decl.coord.line)
	comments = re.findall(pattern, line)
	if len(comments) == 1:
		c = comments[0]
		if c[:2] == "/*" and c[-2:] == "*/":
			javaname = c[2:-2].strip()
			return (javaname, (funcdef.decl.name, funcdef))
	else:
		return None
	
	
	
def get_java_names_of_C_fns(ast):
	"""
	Search for all function definitions, extract the original 
	Java version of the function (which will be inserted as a block comment by Jamaica)
	and return as a dictionary of {java_method_sig->(c_fn_name, astnode)}.
	"""
	class FuncDefVisitor(c_ast.NodeVisitor):
		def __init__(self):
			self.fns = {}
		
		def visit_FuncDef(self, node):
			details = get_java_names_of_funcdef(node)
			if details == None:
				# Lots of functions are declared in jamaica.h and we do not want to report warnings about those
				if not str(node.decl.coord.file).endswith("jamaica.h"):
					log().error("Unexpected comment format in Jamaica output: " + str(node.decl.coord.file) + " " + str(node.decl.coord.line))
			else:	
				javaname = details[0]
				rv = details[1]
				self.fns[javaname] = rv
				
	v = FuncDefVisitor()
	v.visit(ast)
	return v.fns
	

javanames_cache = {}

def c_decl_node_of_java_sig(sig, c_output_base):
	"""
	Given a Java signature, get the decl node in the AST of the C function which implements it.
	Note that this function performs a complete AST traversal of the target file. It is wasteful to 
	call this to resolve a large number of signatures in the same package so the results are cached.
	If code is being morphed, javanames_cache should be cleared to force a re-parse.
	"""
	filename = c_filename_of_java_method_sig(sig, c_output_base)
	
	if not astcache.get(filename) in javanames_cache:
		javanames_cache[astcache.get(filename)] = get_java_names_of_C_fns(astcache.get(filename))
	
	names = javanames_cache[astcache.get(filename)]
	if not sig in names:
		raise CaicosError("There is no method with the signature " + str(sig) + " in file " + str(filename))
	node = names[sig][1]
	declnode = node.children()[0][1]
	if not isinstance(declnode, c_ast.Decl):
		raise CaicosError("Unexpected function declaration format in file " + str(filename) + " for signature " + str(sig))
	return declnode


def c_prototype_of_java_sig(sig, c_output_base):
	"""
	Given a Java signature, get the C function which implements it, as a C-style function prototype 
	suitable for a header file.
	"""
	return pycparser.c_generator.CGenerator().visit(c_decl_node_of_java_sig(sig, c_output_base)) + ";"
	

def rewrite_source_file(inputfile, outputfile, reachable_functions, syscalls):
	"""
	Parse inputfile, rewrite the AST, save the result to outputfile. All paths should be absolute.
	"""
	def is_funcdef_in_reachable_list(funcdef):
		for r in reachable_functions:
			if r.coord.file == inputfile and r.decl.name == funcdef.decl.name:
				return True
		return False
	
	ast = astcache.get(inputfile)
	rewrite_RAM_structure_dereferences(ast)
	
	outf = open(outputfile, "w")
	outf.write('#include "jamaica.h"\n')
	outf.write('#include "jni.h"\n')
	outf.write('#include "jbi.h"\n')
	outf.write('#include "Main__.h"\n')
	outf.write('\n')
	
	generator = pycparser.c_generator.CGenerator()
	for c in ast.children():
		if isinstance(c[1], c_ast.FuncDef):
			if reachable_functions == None or is_funcdef_in_reachable_list(c[1]):
				rewrite_ct_refs(c[1])
				rewrite_syscall_calls(c[1], syscalls)
				
				outf.write(generator.visit(c[1]))
				outf.write("\n")

	outf.close()


def rewrite_syscall_calls(funcdef, syscalls):
	"""
	Search the current function definition for any calls to functions, the name of which are in syscalls.
	When found, rewrite to be an invocation of pcie_syscall, with arguments appropriately cast.
	"""
	class FuncCallVisitor(c_ast.NodeVisitor):
		def __init__(self): 
			self.fns = {}
		
		def visit_FuncCall(self, node):
			if node.name.name in syscalls:
				if len(node.args.exprs) > MAX_SYSCALL_ARGS:
					raise CaicosError("Call to " + str(node.name.name) + " in " + str(funcdef.decl.name) + " has " + str(len(node.args.exprs)) + " arguments. Maximum is " + str(MAX_SYSCALL_ARGS))

				first = node.args.exprs[0]
				if not first.name == "ct":
					raise CaicosError("Call to " + str(node.name.name) + " in " + str(funcdef.decl.name) + " does not use the current thread as the first argument.")
				
				#Set the first parameter to the ID of the call (This should be 'ct', which we don't need as the host code knows it anyway).
				node.args.exprs[0] = c_ast.ID(str(syscalls[node.name.name]))
				
				#Set the name of the syscall function
				node.name.name = SYSCALL_NAME
				
				while len(node.args.exprs) != MAX_SYSCALL_ARGS:
					node.args.exprs.append(c_ast.ID("0"))
				
				#print "@@@ " + str(pycparser.c_generator.CGenerator().visit(node))
				#TODO: Do we need to cast the arguments or return type? Currently jamaica_ref is only used so this /should/ coerce OK
				#Resolve the system call into a FuncDecl so we can check its types
				#decl = flowanalysis.get_funcdecl_of_system_funccall(node.name.name)
				
	v = FuncCallVisitor()
	v.visit(funcdef)
	
	


def make_func_call_node(name, args):
	"""
	Build a function call node from a function name and a list of arguments.
	Arguments may be AST nodes, or strings. Strings are inserted as instances of c_ast.ID
	None arguments are turned into ID("0")
	"""
	exprs = []
	for a in args:
		if a != None:
			if isinstance(a, ''.__class__):
				exprs.append(c_ast.ID(str(a)))
			else:
				exprs.append(a)
		else:
			exprs.append(c_ast.ID('0'))
	explist = c_ast.ExprList(exprs)
	return c_ast.FuncCall(c_ast.ID(name), explist)



def rewrite_ct_refs(ast):
	"""
	Jamaica Builder uses references to the current thread in a way which is not synthesisable, so we rewrite them 
	to use functions in the porting layer.
	"""
	codegen = CGenerator()
	
	def rewrite_structref(node):
		"""
		Rewrite other structure dereferences of the ct reference 
		"""
		reftext = codegen.visit(node)
		rewritepairs = {
			"ct->m.cli": ("m_cli", 'ct'),
			"ct->plainAlloc": ("plainalloc", 'ct'),
			"ct->javaStackSize": ("javastacksize", 'ct'),
			"gc->white": ("gc_white", 'ct'),
			"gc->greyList": ("gc_greylist", 'ct')
		}
		if reftext in rewritepairs.keys():
			fname, var = rewritepairs[reftext]
			if isinstance(node.parent, c_ast.Assignment) and node.parent.lvalue == node:
				rewrite_ct_refs(node.parent.rvalue)
				replace_node(node.parent, make_func_call_node("juniper_set_" + str(fname), [var, node.parent.rvalue]))
			else: 
				replace_node(node, make_func_call_node("juniper_get_" + str(fname), [var]))

	def rewrite_decl(node):
		"""
		Rewrite declarations of the l and gc local pointers.
		"""
		if node.name == "l" and isinstance(node.type, c_ast.PtrDecl) and node.type.type.type.names[0] == "jamaica_ref":
			node.init = make_func_call_node("juniper_get_l_array_ref", ['ct'])
		elif node.name == "gc" and isinstance(node.type, c_ast.PtrDecl) and node.type.type.type.names[0] == "jamaica_GCEnv":
			node.init = make_func_call_node("juniper_get_gcenv_ref", ['ct'])

	def recurse(node):
		"""
		Recurse depth-first down the AST. Whilst popping off the stack, call rewrite_structref or rewrite_decl
		if a StructRef or Decl node respectively is seen
		"""
		for c in node.children(): 
			recurse(c[1])
		if isinstance(node, c_ast.StructRef): 
			rewrite_structref(node)
		if isinstance(node, c_ast.Decl): 
			rewrite_decl(node)

	recurse(ast)


	
def get_line_bounds_for_function(declnode):
	"""
	Given an instance of a c_ast.DeclNode, return a tuple of the start and end lines (inclusive)
	of the function definition. 
	
	This is not a robust function, and should only be used on the 
	predictable output format of Jamaica Builder. Whilst the start line is accurate because it comes
	from a proper parse, the end line is derived by finding the start line of the node's next sibling and
	subtracting 1. 
	
	This will be fooled by preprocessor macros, or unusual C layout, but Jamaica Builder does not use these.
	
	If the node has no next sibling, then the end line is returned as None to signify EOF.
	"""
	def get_next_sibling(node):
		children = node.parent.children()
		for i in xrange(len(children)):
			if children[i][1] == node:
				if i + 1 < len(children):
					return children[i + 1][1]
				else:
					return None	
				
	startline = declnode.parent.coord.line
	sib = get_next_sibling(declnode.parent)
	if sib == None:
		endline = None
	else:
		endline = sib.coord.line - 1		
	return (startline, endline)


def c_name_of_type(typenode):
	"""
	TypeNode:
		Either a PtrDecl or TypeDecl
	Returns:
		The name of the type, with the appropriate number of *s before it to indicate 
		the level of pointer indirection in the AST. 
	"""
	ptrs, name = c_code_from_typenode(typenode)
	return ptrs + name

		
def c_cast_for_type(typenode):
	"""
	TypeNode:
		Either a PtrDecl or TypeDecl
	Returns:
		The name of the type, with parentheses and the appropriate number of *s after it to create a cast to this type
	"""
	ptrs, name = c_code_from_typenode(typenode)
	return "(" + name + ptrs + ")"


def c_code_from_typenode(typenode):
	ptrs = ""
	current = typenode
	while isinstance(current, c_ast.PtrDecl):
		ptrs += "*"
		current = current.type
		
	if not isinstance(current, c_ast.TypeDecl):
		raise CaicosError("Unhandled type in c_code_from_typenode. Code: " + pycparser.c_generator.CGenerator().visit(typenode))
	return (ptrs, str(current.type.names[0]))
	
		
def call_code_for_funcdecl(funcdecl, funcname):
	"""
	Given a funcdecl node, return the C code required to call it.
	"""
	paramlist = funcdecl.children()[0][1]
	rv = funcname + "("
	
	paramnum = 0
	for pid in xrange(len(paramlist.params)):
		#Insert an explicit cast to the target type
		#Handles single-stage pointers and base types only, no arrays, because
		#it is believed that this is all that Jamaica builder will output.
		prm = paramlist.params[pid]
		pointer = False
		if isinstance(prm.type, c_ast.PtrDecl):
			pointer = True
			pdec = prm.type.type
		else:
			pdec = prm.type
			
		if pointer and pdec.type.names[0] == "jamaica_thread":
			rv += "&__juniper_thread"
		else:  		
			rv += c_cast_for_type(prm.type)	+ " "
			#rv += "(" + str(pdec.type.names[0])
			#if pointer: 
			#	rv += "*"
			#rv += ") "
			rv += "__juniper_args[" + str(paramnum) + "]"
			paramnum = paramnum + 1
			
		if not pid == len(paramlist.params) - 1:
			rv += ", "
			
	rv += ");" 
	return rv
