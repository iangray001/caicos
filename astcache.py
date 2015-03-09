from utils import log
import utils


class ASTCache():
	"""
	To prevent unnecessary re-parsing, we use an AST cache which will parse files on 
	first request and reply with the cached version on subsequent requests. 
	"""
	cache = {}

	def get(self, filename):
		if filename in self.cache:
			return self.cache[filename]
		else:
			log().info("ASTCache parsing " + filename)
			ast = utils.parse_jamaica_output(filename)
			self.cache[filename] = ast
			return ast
			
	def invalidate_ast_for(self, filename):
		if filename in self.cache:
			del self.cache[filename]

	def clear(self):
		self.cache.clear()
	