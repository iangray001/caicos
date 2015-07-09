from utils import CaicosError

INTERFACE_START = "/* All included implementations of "

def parse_getInterfaceMethod_call(node):
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
		found.add(i + methodname)
	return found

	