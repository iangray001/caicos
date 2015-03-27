"""
The ASTCache module helps to prevent unnecessary re-parsing, which can be 
very slow when using pycparser.

Maintains a dictionary of {filenames -> ASTs}

Call get(filename) to get an AST. The file will be parsed if this is the
first request, otherwise the previously generated AST will be returned.   

If code is being modified invalidate_ast_for(filename) can be used to 
remove a cached AST and force a reparse.
"""

from utils import log
import utils


cache = {}

def get(filename):
	if filename in cache:
		return cache[filename]
	else:
		log().info("Parsing " + filename)
		ast = utils.parse_jamaica_output(filename)
		cache[filename] = ast
		return ast
		
def invalidate_ast_for(filename):
	if filename in cache:
		del cache[filename]

def clear():
	cache.clear()

	


	