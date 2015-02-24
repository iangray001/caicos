import re

from pycparser import c_ast
import pycparser

import juniperrewrites
import utils



    

print "Parsing..."
ast = utils.parse_jamaica_output("/Users/ian/Dropbox/Java/juniperworkspace2/JamaicaOutputAnalysis/generatedCode/PKG_unitTests_V*.c")
print "Parsed."

#juniperrewrites.rewrite_RAM_structure_dereferences(ast)

print utils.get_original_name_of_fn(ast, "jam_comp_unitTests_Types_0_types1")

#print "Code gen..."
#utils.write_ast_to_file(ast, "output.c")
#print "Code gen complete."
