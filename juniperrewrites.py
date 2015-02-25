from pycparser import c_ast, c_generator

import utils


RAM_NAME = '__juniper_ram_master'



# class BetterNodeVisitor(object):
#     """
#     A better Visitor superclass. This visitor does the same as c_ast.NodeVisitor 
#     except it also provides the visit method with the parent of the visited node, 
#     which is required to perform edits on the AST.
#     """
#     def visit(self, node, parent):
#         method = 'visit_' + node.__class__.__name__
#         visitor = getattr(self, method, self.generic_visit)
#         return visitor(node, parent)
# 
#     def generic_visit(self, node, parent):
#         for _, c in node.children():
#             self.visit(c, node)



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
                                    call = "juniper_ram_fetch_" + structmember + "(" + functionargs + ", " + str(arrayrefoffset) + ")"
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
