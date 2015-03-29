import os
import re
import sys

from pycparser import c_ast, c_generator
import pycparser

import astcache
from utils import CaicosError, log
import utils


RAM_NAME = '__juniper_ram_master'
RAM_GET_NAME = 'juniper_ram_get_'
RAM_SET_NAME = 'juniper_ram_set_'


def replace_node(orignode, newnode):
	"""
	Replace the node orignode with the node newnode
	"""
	for (child_name, child) in orignode.parent.children():
		if child == orignode:
			if '[' in child_name:
				arrayname = child_name[:child_name.find('[')]
				arrayidx = child_name[child_name.find('[')+1:child_name.find(']')]
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
	"""
	class Visitor(c_ast.NodeVisitor):
		"""
		We are looking for structures like the following:
		  ArrayRef:
			StructRef: .
			  ArrayRef: 
				ID: __juniper_ram_master
				<Anything else>
			  ID: f
			Constant: int, 2
		""" 
		def visit_StructRef(self, node):
			c = node.children()
			if len(c) == 2:
				if isinstance(c[0][1], c_ast.ArrayRef) and isinstance(c[1][1], c_ast.ID):
					refch = c[0][1].children()
					if len(refch) > 0 and isinstance(refch[0][1], c_ast.ID):
						if getattr(refch[0][1], 'name') == RAM_NAME:
							
							#Appropriate node found.
							structmember = c[1][1].name #What was dereferenced ('i', 's', 'c', 'r', etc.)
							log().debug(str(node.coord) + ":")
							log().debug("\tOriginal: " + utils.getlineoffile(node.coord.file, node.coord.line).strip())
							rewrite_RAM_structure_dereferences(refch[1][1]) #Deal with ram accesses in the argument
							functionargs = c_generator.CGenerator().visit(refch[1][1])
							
							#If there a higher-level ArrayRef we are interested in we need to get the index
							if isinstance(node.parent, c_ast.ArrayRef):
								if len(node.parent.children()) >= 2 and isinstance(node.parent.children()[1][1], c_ast.Constant):
									rewrite_RAM_structure_dereferences(node.parent.children()[1][1]) #Deal with ram accesses in the argument
									arrayrefoffset = c_generator.CGenerator().visit(node.parent.children()[1][1])
									nodetoreplace = node.parent
								else:
									raise CaicosError("Unsupported ArrayRef format detected")
							else:
								arrayrefoffset = 0
								nodetoreplace = node
							
							#We also need to decide whether to use a GET or SET function
							operation = "get"
							if isinstance(nodetoreplace.parent, c_ast.Assignment):
								if nodetoreplace.parent.op == "=":
									if nodetoreplace.parent.lvalue == nodetoreplace:
										operation = "set"
										rvalue = nodetoreplace.parent.rvalue
										
							if operation == "get": 
								call = RAM_GET_NAME + structmember + "(" + functionargs + ", " + str(arrayrefoffset) + ")"
							else:
								rewrite_RAM_structure_dereferences(rvalue) #Deal with ram accesses in the argument
								call = RAM_SET_NAME + structmember + "(" + \
										functionargs + ", " + str(arrayrefoffset) + \
										", " + pycparser.c_generator.CGenerator().visit(rvalue) + ")"
								nodetoreplace = nodetoreplace.parent
							
							log().debug("\tReplacement: " + call)
							replacementnode = c_ast.ID("name")
							setattr(replacementnode, 'name', call)
							replace_node(nodetoreplace, replacementnode)
							
	v = Visitor()
	v.visit(ast)



def c_filename_of_javaclass(classname, c_output_base, classdelimiter='.'):
	"""
	The C file containing a given Java class is only semi-predictable, we need to glob for it.
	Java class format x.y.classname is translated to an absolute file pathname.
	Jamaica translates on a per-package basis, so many classes may be in the same C file.
	"""
	parts = classname.strip().split(classdelimiter)
	if not os.path.exists(c_output_base): return None

	if len(parts) < 2:
		raise CaicosError("Class name " + str(classname) + " appears to reference an unpackaged class, which is not supported by JamaicaBuilder or modern Java.")
	
	name = "PKG_"
	for part in parts[:-1]:
		name = name + part + "_"
	name = name + "V*__.c"

	return utils.deglob_file(os.path.join(c_output_base, name))



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
			pattern = re.compile(
				r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
				re.DOTALL | re.MULTILINE
			)        
			line = utils.getlineoffile(node.decl.coord.file, node.decl.coord.line)
			comments = re.findall(pattern, line)
			if len(comments) == 1:
				c = comments[0]
				if c[:2] == "/*" and c[-2:] == "*/":
					javaname = c[2:-2].strip()
					self.fns[javaname] = (node.decl.name, node)
			else:
				log().error("Unexpected comment format in Jamaica output: " + str(node.decl.coord.file) + " " + str(node.decl.coord.line))
				
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
		raise CaicosError("There is no method with the signature " + str(sig) + " + in file " + str(filename))
	node = names[sig][1];
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
	

def rewrite_source_file(inputfile, outputfile):
	"""
	Parse inputfile, rewrite the AST, save the result to outputfile. All paths should be absolute.
	"""
	ast = astcache.get(inputfile)
	rewrite_RAM_structure_dereferences(ast)
	utils.write_ast_to_file(ast, outputfile)

	