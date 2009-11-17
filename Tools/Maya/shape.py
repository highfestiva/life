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

                wt = scalenode.get_world_transform()
                v0 = wt*vec4(1,0,0,0)
                v1 = wt*vec4(0,1,0,0)
                v2 = wt*vec4(0,0,1,0)
                nv0 = v0.normalize()
                nv1 = v1.normalize()
                nv2 = v2.normalize()
                if math.fabs(nv0*nv1) > 0.05 or math.fabs(nv0*nv2) > 0.05 or math.fabs(nv1*nv2) > 0.05:
                        print("Error: scale for physical shape '%s' is not orthogonal!" % scalenode.getFullName())
                        print(nv0, nv1, nv2)
                        print(wt)
                        sys.exit(21)
                check_orthonormal = True

                if shapenode.nodetype == "polyCube":
                        self.type = "box"
                        check_orthonormal = False
                        scale = scalenode.get_world_scale()
                        #d.append(shapenode.getAttrValue("w", "w", None, default=1.0)*scale[0])
                        #d.append(shapenode.getAttrValue("d", "d", None, default=1.0)*scale[1])
                        #d.append(shapenode.getAttrValue("h", "h", None, default=1.0)*scale[2])
                        x = shapenode.getAttrValue("w", "w", None, default=1.0)
                        y = shapenode.getAttrValue("h", "h", None, default=1.0)
                        z = shapenode.getAttrValue("d", "d", None, default=1.0)
                        s = list(map(lambda x,y: x*y, [x,y,z], scale))
                        #print(scalenode.getName(), "has scale", scale, "and size", [x,y,z])
                        d += s
                        s = wt * vec4(x,y,z,0)
                        #d += s[:3]
                        #s = map(lambda x,y: x*y, [x,y,z], scale)
                        #d += map(lambda x, y: x*y, s[:3], scale)
                        #_w = shapenode.getAttrValue("w", "w", None, default=1.0)
                        #_d = shapenode.getAttrValue("d", "d", None, default=1.0)
                        #_h = shapenode.getAttrValue("h", "h", None, default=1.0)
                        #size = (wt*vec4(_w,_d,_h,0))[:3]
                        #d += size
                elif shapenode.nodetype == "polySphere":
                        self.type = "sphere"
                        d.append(shapenode.getAttrValue("r", "r", None, default=1.0)*v0.length())
                else:
                        print("Error: shape type '%s' on node '%s' is unknown." % (shapenode.nodetype, shapenode.getFullName()))
                        sys.exit(22)

                if check_orthonormal:
                        if math.fabs(v0.length()-v1.length()) > 0.05 or math.fabs(v0.length()-v2.length()) > 0.05 or math.fabs(v1.length()-v2.length()) > 0.05:
                                print("Error: scale for physical shape '%s' is not orthonormal!" % scalenode.getFullName())
                                sys.exit(21)


        def __str__(self):
                return "<Shape %s %s>" % (self.type, " ".join(map(lambda x: str(x), self.data)))

        def getnode(self):
                return self._attrnode
