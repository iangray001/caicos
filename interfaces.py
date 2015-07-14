from pycparser import c_ast

from juniperrewrites import c_name_of_java_method
from utils import CaicosError, codegen


INTERFACE_START = "/* All included implementations of "


class InterfaceResolver(object):
	"""
	Searches for the annotations inserted by JamaicaBuilder to say which methods may be returned by a given interface call
	This information is stored in a mapping dictionary to ensure a unique ID for each interface call.
	"""
	
	def __init__(self, jamaicaoutputdir):
		self.interfacemapping = {} #Maps C function names to (ID, cname)
		self.next_id = 1
		self.jamaicaoutputdir = jamaicaoutputdir


	def parse_getInterfaceMethod_call(self, node):
		"""
		Given a FuncCall node to jamaicaInterpreter_getInterfaceMethod, determine which methods might be returned.
		"""
		with open(node.coord.file) as ofile:
			lines = ofile.readlines()
	
		errorstr = "When handling interface calls, file " + node.coord.file + " does not appear to be in the correct format. Was a version of JamaicaBuilder that supports interface enumeration used?"
		
		if lines[node.coord.line - 2].strip() != "*/":
			raise CaicosError(errorstr)
		
		offset = 3
		interfaces = []
		while True:
			if node.coord.line - offset < 0:
				raise CaicosError(errorstr)
			line = lines[node.coord.line - offset].strip()	
			if line.startswith(INTERFACE_START):
				break
			else:
				interfaces.append(line)
			offset += 1
			
		interfacename = line[len(INTERFACE_START):]
		if not interfacename[-1] == ':':
			raise CaicosError(errorstr)
		interfacename = interfacename[:-1]
		
		methodname = interfacename[interfacename.rfind("."):]
	
		found = set()
		for i in interfaces:
			name = i + methodname
			found.add(name)
			
			if not name in self.interfacemapping: 
				self.interfacemapping[name] = (self.next_id, c_name_of_java_method(name, self.jamaicaoutputdir))
				self.next_id += 1
			
		return found


	def get_host_code(self):
		"""
		Generate the switch statement in the host code (caicos_interrupts.c) which checks the return value
		of jamaicaInterpreter_getInterfaceMethod to see which method was returned.
		"""
		code = "\t\t\tunsigned long addr = (unsigned long) jamaicaInterpreter_getInterfaceMethod(ct, (jamaica_ref) args.arg1, (jamaica_ref) args.arg2);\n"
		code += "\t\t\tswitch(addr) {\n"
		
		for iid, cname in self.interfacemapping.values():
			code += "\t\t\t\tcase (unsigned long) " + str(cname[0]) + ":\n"
			code += "\t\t\t\t\trv = " + str(iid) + ";\n"
			code += "\t\t\t\t\tbreak;\n"
		
		code += "\t\t\t}\n"
		
		return code
	
	
	def rewrite_interface_calls(self, funcdef):
		intreslv = self
		
		class FuncCallVisitor(c_ast.NodeVisitor):
			def __init__(self): 
				self.fns = {}
			
			def visit_FuncCall(self, node):
				if not isinstance(node.name, c_ast.Cast) and node.name.name == "jamaicaInterpreter_getInterfaceMethod":
					#Verify the format of the getInterfaceMethod Compound. It is believed this is constant.
					decl = node.parent
					compound = decl.parent
					if (not isinstance(decl, c_ast.Decl) or (not isinstance(compound, c_ast.Compound))):
						raise CaicosError("Call to jamaicaInterpreter_getInterfaceMethod appears to be in a different format. Are you using a compatible version of Jamaica?") 
					expected = [c_ast.Decl, c_ast.Assignment, c_ast.If, c_ast.If, c_ast.Assignment]
					for i in xrange(len(compound.block_items)):
						if not isinstance(compound.block_items[i], expected[i]):
							raise CaicosError("Call to jamaicaInterpreter_getInterfaceMethod is a different format. Are you using a compatible version of Jamaica?") 
					
					compound.block_items[0].type.type.names[0] = "jamaica_int32"
					
					del compound.block_items[1] #ct->calledMethod = m;
					del compound.block_items[1] #If jamaica_throwIncompClassChangeErr
					del compound.block_items[1] #If jamaica_throwAbstrMethodErr
					
					block_items = []
					
					for m in intreslv.parse_getInterfaceMethod_call(node):
						cname, _ = c_name_of_java_method(m, intreslv.jamaicaoutputdir)

						funccall = c_ast.FuncCall(c_ast.ID(cname), compound.block_items[-1].rvalue.args)
						assignment = c_ast.Assignment(compound.block_items[-1].op, compound.block_items[-1].lvalue, funccall)
						case = c_ast.Case(c_ast.ID("(unsigned long) " + str(cname)), [assignment, c_ast.Break()])
						block_items.append(case)
						
					swi = c_ast.Switch(c_ast.ID(compound.block_items[0].name), c_ast.Compound(block_items))
					compound.block_items[-1] = swi

		
		v = FuncCallVisitor()
		v.visit(funcdef)
	
	