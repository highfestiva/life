# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-07-21 for Righteous Engine tool chain.


import math
import sys
from mat4 import mat4
from vec3 import vec3
from vec4 import vec4


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
                #if not scalenode.isortho():
                #        print("Error: node '%s' is not orthogonal." % scalenode.getFullName())
                #        sys.exit(21)
                #scale = scalenode.get_local_scale()
                #m = mat4(scalenode.get_world_transform()).getMat3()
                scale = (scalenode.get_world_transform()*vec4(1,1,1,0))[:3]
                #scale = m * scalenode.get_world_scale()
                #scale = m * vec3(1,1,1)
                #scale = scalenode.get_world_scale()
                if shapenode.nodetype == "polyCube":
                        self.type = "box"
                        #d.append(shapenode.getAttrValue("w", "w", None, default=1.0)*scale[0])
                        #d.append(shapenode.getAttrValue("d", "d", None, default=1.0)*scale[1])
                        #d.append(shapenode.getAttrValue("h", "h", None, default=1.0)*scale[2])
                        x = shapenode.getAttrValue("w", "w", None, default=1.0)
                        y = shapenode.getAttrValue("h", "h", None, default=1.0)
                        z = shapenode.getAttrValue("d", "d", None, default=1.0)
                        d += map(lambda x,y: x*y, [x,y,z], scale)
                        s = scalenode.get_world_transform() * vec4(x,y,z,0)
                        #d += s[:3]
                        #s = map(lambda x,y: x*y, [x,y,z], scale)
                        #d += map(lambda x, y: x*y, s[:3], scale)
                        #_w = shapenode.getAttrValue("w", "w", None, default=1.0)
                        #_d = shapenode.getAttrValue("d", "d", None, default=1.0)
                        #_h = shapenode.getAttrValue("h", "h", None, default=1.0)
                        #size = (scalenode.get_world_transform()*vec4(_w,_d,_h,0))[:3]
                        #d += size
                elif shapenode.nodetype == "polySphere":
                        self.type = "sphere"
                        absscale = map(lambda x: math.fabs(x), scale)
                        if not Shape.inrange(absscale, 0.05):
                                print("Error: scale for sphere node '%s' must be symmetric (is %s)." % (scalenode.getFullName(), str(scale)))
                                sys.exit(21)
                        d.append(shapenode.getAttrValue("r", "r", None, default=1.0)*scale[0])
                else:
                        print("Error: shape type '%s' on node '%s' is unknown." % (shapenode.nodetype, shapenode.getFullName()))
                        sys.exit(22)

        def __str__(self):
                return "<Shape %s %s>" % (self.type, " ".join(map(lambda x: str(x), self.data)))

        def getnode(self):
                return self._attrnode
