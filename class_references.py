from pycparser import c_ast
import pycparser

from astcache import add_parent_links
from juniperrewrites import make_func_call_node, replace_node


class ClassRefs():
	"""
	Used for tracking class references in the source, which are actually referenced, and their indices 
	Both dictionaries map from the name of the classref to a monotonically increasing integer ID 
	(starting at 1)
	"""
	def __init__(self):
		self.all = {}
		self.referenced = {}
		
	def __str__(self):
		return "ClassRefs:\n\t" + str(self.all) + "\n\t" + str(self.referenced) + "\n" 


def enumerate_static_refs(main):
	"""
	Get the class references that are defined in Main__.c
	This is not perfect as it should really be parsed. For now however, the structure of the file is
	so static that we can just use basic text scanning.

	Returns a map of classname -> index
	
	TODO: Replace with proper parsing
	"""
	classrefs = ClassRefs()
	idx = 1
	
	with open(main, "r") as f:
		for line in f:
			if line.startswith("jamaica_ref ") and line.endswith(";\n"):
				name = line[12:len(line)-2]
				if name != "Result":
					if not name in classrefs.all:
						classrefs.all[name] = idx
						idx += 1
			
	return classrefs



def replace_class_references(ast, classrefs):
	"""
	This function performs rewrites for the HLS hardware design.
	Replace references to static class information with juniper_get_class_reference, a hardcoded syscall 
	back to the host which fetches a juniper_ref of the class metadata.
	"""
	def recurse(node):
		for c in node.children(): 
			recurse(c[1])	
		if isinstance(node, c_ast.ID):
			if node.name in classrefs.all:
				if not node.name in classrefs.referenced: 
					classrefs.referenced[node.name] = len(classrefs.referenced) + 1
				fcall = make_func_call_node("juniper_get_class_reference", ["1", str(classrefs.referenced[node.name])])
				replace_node(node, fcall)
		
	recurse(ast)
	add_parent_links(ast)


