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
                self._scalenode = scalenode
                self._shapenode = shapenode

                wt = scalenode.get_world_transform()
                v0 = wt*vec4(1,0,0,0)
                v1 = wt*vec4(0,1,0,0)
                v2 = wt*vec4(0,0,1,0)
                nv0 = v0.normalize()
                nv1 = v1.normalize()
                nv2 = v2.normalize()
                d0, d1, d2 = map(lambda x,y: math.fabs(x*y), [nv0, nv0, nv1], [nv1, nv2, nv2])
                if d0 > 0.1 or d1 > 0.1 or d2 > 0.1:
                        print("Error: scale for physical shape '%s' is not orthogonal!" % scalenode.getFullName())
                        print(nv0, nv1, nv2)
                        print(d0, d1, d2)
                        print(wt)
                        sys.exit(21)
                check_orthonormal = True

                if shapenode.nodetype == "polyCube":
                        self.type = "box"
                        check_orthonormal = False
                        scale = scalenode.get_world_scale()
                        x = shapenode.getAttrValue("w", "w", None, default=1.0)
                        y = shapenode.getAttrValue("h", "h", None, default=1.0)
                        z = shapenode.getAttrValue("d", "d", None, default=1.0)
                        d += list(map(lambda x,y: x*y, [x,y,z], scale))
                elif shapenode.nodetype == "polySphere":
                        self.type = "sphere"
                        d.append(shapenode.getAttrValue("r", "r", None, default=1.0)*v0.length())
                else:
                        print("Error: primitive physics shape type '%s' on node '%s' is unknown." % (shapenode.nodetype, shapenode.getFullName()))
                        sys.exit(22)

                if check_orthonormal:
                        if math.fabs(v0.length()-v1.length()) > 0.1 or math.fabs(v0.length()-v2.length()) > 0.1 or math.fabs(v1.length()-v2.length()) > 0.1:
                                print("Error: scale for physical shape '%s' is not orthonormal!" % scalenode.getFullName())
                                sys.exit(21)


        def __str__(self):
                return "<Shape %s %s>" % (self.type, " ".join(map(lambda x: str(x), self.data)))


        def get_lowest_world_point(self):
                p = self._scalenode.get_world_translation() - self._scalenode.getphysmaster().get_world_translation()
                lp = p[:3]
                if self._shapenode.nodetype == "polyCube":
                        lp[0] -= self.data[0]/2
                        lp[1] -= self.data[2]/2
                        lp[2] -= self.data[1]/2
                elif self._shapenode.nodetype == "polySphere":
                        lp[0] -= self.data[0]
                        lp[1] -= self.data[0]
                        lp[2] -= self.data[0]
                else:
                        print("Error: (2) primitive physics shape type '%s' on node '%s' is unknown." % (shapenode.nodetype, shapenode.getFullName()))
                        sys.exit(22)
                #print("Shape %s at %s says it has a lowest point of %s with size %s." %(self._scalenode.getName(), p, lp, self.data))
                return vec3(lp[:3])


        def getnode(self):
                return self._scalenode
