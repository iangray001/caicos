import logging
import os

from pycparser import c_ast, c_generator

from utils import CaicosException
import utils


RAM_NAME = '__juniper_ram_master'
RAM_ACCESS_NAME = 'juniper_ram_fetch_'


def add_parent_links(ast):
	"""
	Iterate over an AST annotating every node with a link to its parent node.
	"""
	def recurse(node, parent):
		node.parent = parent
		for _, c in node.children():
			recurse(c, node)
	recurse(ast, None)


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
					print str(arrayidx) + " is not a valid array index"
				
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
							print str(node.coord) + ":\n\tOriginal: " + utils.getlineoffile(node.coord.file, node.coord.line).strip()
							functionargs = c_generator.CGenerator().visit(refch[1][1])
							
							#Is there a higher-level ArrayRef we are interested in?
							if isinstance(node.parent, c_ast.ArrayRef):
								if len(node.parent.children()) >= 2 and isinstance(node.parent.children()[1][1], c_ast.Constant):
									arrayrefoffset = c_generator.CGenerator().visit(node.parent.children()[1][1])
									call = RAM_ACCESS_NAME + structmember + "(" + functionargs + ", " + str(arrayrefoffset) + ")"
									print "\tReplacement: " + call
									replacementnode = c_ast.ID("name")
									setattr(replacementnode, 'name', call)
									replace_node(node.parent, replacementnode)
								else:
									print "Unsupported ArrayRef format detected"
							else:
								call = "juniper_ram_fetch_" + structmember + "(" + functionargs + ", 0)"
								print "\tReplacement: " + call
								replacementnode = c_ast.ID("name")
								setattr(replacementnode, 'name', call)
								replace_node(node, replacementnode)
						
	v = Visitor()
	v.visit(ast)



def c_filename_of_javaclass(classname, c_output_base):
	"""
	The C file containing a given Java class is only semi-predictable, we need to glob for it.
	Java class format x.y.classname is translated to an absolute file pathname.
	Jamaica translates on a per-package basis, so many classes may be in the same C file.
	"""
	parts = classname.strip().split('.')
	if not os.path.exists(c_output_base): return None

	if len(parts) < 2:
		raise CaicosException("Class name " + str(classname) + " appears to reference an unpackaged class, which is not supported by JamaicaBuilder or modern Java.")
	
	name = "PKG_"
	for part in parts[:-1]:
		name = name + part + "_"
	name = name + "*.c"

	return utils.deglob_file(os.path.join(c_output_base, name))

	
	