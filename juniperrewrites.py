from pycparser import c_ast, c_generator

import utils


RAM_NAME = '__juniper_ram_master'



class BetterNodeVisitor(object):
    """
    A better Visitor superclass. This visitor does the same as c_ast.NodeVisitor 
    except it also provides the visit method with the parent of the visited node, 
    which is required to perform edits on the AST.
    """
    def visit(self, node, parent):
        method = 'visit_' + node.__class__.__name__
        visitor = getattr(self, method, self.generic_visit)
        return visitor(node, parent)

    def generic_visit(self, node, parent):
        for _, c in node.children():
            self.visit(c, node)



def replace_child(parent, orignode, newnode):
    """
    Replace the node orignode, which is a child of parent, with the node newnode
    """
    for (child_name, child) in parent.children():
        if child == orignode:
            parent.child_name = newnode
            setattr(parent, child_name, newnode)


class RAMVisitor(BetterNodeVisitor):
    """
    We are looking for structures like the following:
        StructRef: .
          ArrayRef: 
            ID: __juniper_ram_master
            BinaryOp: +
              ID: r0
              Constant: int, 3
          ID: f
    """
    
    def visit_StructRef(self, node, parent):
        c = node.children()
        if len(c) == 2:
            if isinstance(c[0][1], c_ast.ArrayRef) and isinstance(c[1][1], c_ast.ID):
                #Found a suitable StructRef. Now check the ID of the struct being refed
                refch = c[0][1].children()
                if len(refch) > 0 and isinstance(refch[0][1], c_ast.ID):
                    if getattr(refch[0][1], 'name') == RAM_NAME:
                        print str(node.coord) + ": " + utils.getlineoffile(node.coord.file, node.coord.line).strip()
                        print c_generator.CGenerator().visit(node)
                        print c[1][1].name

                        blahnode = c_ast.ID("name")
                        setattr(blahnode, 'name', "BLAH BLAH BLAH;")
                        
                        replace_child(parent, node, blahnode)
                        
                        

def rewrite_RAM_structure_dereferences(ast):
    v = RAMVisitor()
    v.visit(ast, None)
