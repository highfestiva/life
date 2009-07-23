# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-07-21 for Righteous Engine tool chain.


import sys


class Shape:
        @staticmethod
        def inrange(vals, deviation):
                firstval = None
                for val in vals:
                        if firstval == None:
                                firstval = val
                        else:
                                if val < firstval*(1-deviation) or val > firstval*(1+deviation):
                                        return False
                return firstval != None

        def __init__(self, scalenode, shapenode):
                d = []
                self.data = d
                self._attrnode = scalenode
                scale = scalenode.get_world_scale()
                if shapenode.nodetype == "polyCube":
                        self.type = "box"
                        d.append(shapenode.getAttrValue("w", "w", None, default=1.0)*scale[0])
                        d.append(shapenode.getAttrValue("d", "d", None, default=1.0)*scale[1])
                        d.append(shapenode.getAttrValue("h", "h", None, default=1.0)*scale[2])
                elif shapenode.nodetype == "polySphere":
                        self.type = "sphere"
                        if not Shape.inrange(scale, 0.05):
                                print("Error: scale for sphere node '%s' must be orthogonal." % scalenode.getFullName())
                                sys.exit(21)
                        d.append(shapenode.getAttrValue("r", "r", None, default=1.0)*scale[0])
                else:
                        print("Error: shape type '%s' on node '%s' is unknown." % (shapenode.nodetype, shapenode.getFullName()))
                        sys.exit(22)

        def __str__(self):
                return "<Shape %s %s>" % (self.type, " ".join(map(lambda x: str(x), self.data)))

        def getnode(self):
                return self._attrnode
