# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-07-17 for Righteous Engine tool chain.


from mat4 import mat4
from mayaascii import *
from quat import quat
from vec4 import vec4
import shape

import math
import pprint
import re
import struct
import sys


physics_type = {"static":1, "dynamic":2, "collision_detect_only":3}

CHUNK_CLASS                        = "CLAS"
CHUNK_CLASS_PHYSICS                = "CLPH"
CHUNK_CLASS_MESH_LIST              = "CLML"
CHUNK_CLASS_PHYS_MESH              = "CLPM"

CHUNK_PHYSICS                      = "PHYS"
CHUNK_PHYSICS_BONE_COUNT           = "PHBC"
CHUNK_PHYSICS_PHYSICS_TYPE         = "PHPT"
CHUNK_PHYSICS_ENGINE_COUNT         = "PHEC"
CHUNK_PHYSICS_BONE_CONTAINER       = "PHBO"
CHUNK_PHYSICS_BONE_CHILD_LIST      = "SBCL"
CHUNK_PHYSICS_BONE_TRANSFORM       = "PHBT"
CHUNK_PHYSICS_BONE_SHAPE           = "PHSH"
CHUNK_PHYSICS_ENGINE_CONTAINER     = "PHEO"
CHUNK_PHYSICS_ENGINE               = "PHEN"

CHUNK_MESH                         = "MESH"
CHUNK_MESH_VERTICES                = "MEVX"
CHUNK_MESH_NORMALS                 = "MENO"
CHUNK_MESH_UV                      = "MEUV"
CHUNK_MESH_COLOR                   = "MECO"
CHUNK_MESH_COLOR_FORMAT            = "MECF"
CHUNK_MESH_TRIANGLES               = "METR"
CHUNK_MESH_STRIPS                  = "MEST"
CHUNK_MESH_VOLATILITY              = "MEVO"



class PhysMeshPtr:
        def __init__(self, physidx, meshbasename, q, p):
                self.physidx = physidx
                self.meshbasename = meshbasename
                self.t = q[:]+p[:3]
                #print(self.t)



class ChunkyWriter:
        def __init__(self, basename, group, config):
                self.basename = basename
                self.group = group
                self.config = config
                self.feats = {}
                self.bodies = self._sortbodies(group)


        def printfeats(self):
                for k,v in self.feats.items():
                        singular, plural = k.split(":")
                        if v == 1:
                                name = singular
                        else:
                                name = plural
                        print("Wrote %6i %s." % (v, name))


        def _sortbodies(self, group):
                bodies = []
                for node in self.group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                bodies += [node]
                def childlevel(node):
                        c = 0;
                        while node.getParent():
                                node = node.getParent()
                                c += 1
                        return c
                bodies.sort(key=childlevel)
                return bodies


        def _addfeat(self, k, v):
                oldv = self.feats.get(k)
                if oldv != None:
                        v += oldv
                self.feats[k] = v


        def _writechunk(self, chunks, name=None):
                t = type(chunks)
                if t == tuple or t == list:
                        islist = False
                        if len(chunks) > 0:
                                firsttype = type(chunks[0])
                        for chunk in chunks:
                                if firsttype == int:
                                        if not type(chunk) == int:
                                                print("Error: mixed types in integer chunk '%s'." % name)
                                                sys.exit(15)
                                elif firsttype == float:
                                        if not type(chunk) == float:
                                                print("Error: mixed types in floating-point chunk '%s'." % name)
                                                sys.exit(15)
                                elif type(chunk) == list or type(chunk) == tuple:
                                        islist = True
                                        self._writechunk(chunk, "subchunk of "+name)
                                else:
                                        if not islist and len(chunks) == 2:
                                                break
                                        print("Error: trying to write less/more than one chunk, which is not a subchunk:")
                                        pprint.pprint(chunks)
                                        sys.exit(15)
                        if len(chunks) == 0 or islist:
                                return

                        if firsttype == int or firsttype == float:
                                #print("Writing number list.")
                                for chunk in chunks:
                                        self._writenumber(chunk)
                                return
                        if firsttype != str:
                                print("Error: trying to write chunk without header signature (%s, type=%s, value='%s')." % (name, str(type(chunks[0])), str(chunks[0])))
                                sys.exit(16)
                        if len(chunks) == 2 and type(chunks[1]) == int:
                                self._writeheader(chunks[0], 4)
                                self._writeint(chunks[1])
                        elif len(chunks) == 2 and type(chunks[1]) == str:
                                head = self._writeheader(chunks[0])
                                self._writestr(chunks[1])
                                self._rewriteheadersize(head)
                        else:
                                #print("Writing %s with data of type %s." % (chunks[0], type(chunks[1])))
                                head = self._writeheader(chunks[0])
                                self._writechunk(chunks[1], chunks[0])
                                self._rewriteheadersize(head)
                elif t == Node:
                        node = chunks
                        if node.nodetype == "transform":
                                self._writebone(node)
                        elif node.nodetype.startswith("engine:"):
                                self._writeengine(node)
                elif t == shape.Shape:
                        shapeChunk = chunks
                        self._writeshape(shapeChunk)
                elif t == PhysMeshPtr:
                        self._writephysmeshptr(chunks)
                else:
                        print("Error: trying to write unknown chunk data in %s, type='%s', value='%s'." % (name, str(t), str(chunks)))
                        sys.exit(17)


        def _writebone(self, node):
                q = node.get_local_quat()
                if not q:
                        print("Error: trying to get rotation from node '%s', but none available." % node.getFullName())
                        sys.exit(18)
                pos = node.get_local_translation()
                data = q[:]+pos[:3]
                #print("Writing bone with data", data)
                self._addfeat("bone:bones", 1)
                self._writexform(data)


        def _writexform(self, xform):
                if len(xform) != 7:
                        print("Error: trying to store transform with len != 7!")
                        sys.exit(18)
                for f in xform:
                        self._writefloat(f)


        def _writeheader(self, signature, size=0):
                self._writesignature(signature)
                sizeoffset = self.f.tell()
                self._writeint(size)
                return sizeoffset

        def _rewriteheadersize(self, offset):
                endoffset = self.f.tell()
                content_size = endoffset - offset - 4
                self.f.seek(offset)
                #print("Writing header size. Header at offset %i, end at offset %i." % (offset, endoffset))
                self._writeint(content_size, "chunk size")
                self.f.seek(endoffset)

        def _writesignature(self, sign):
                self._dowrite(sign.encode('latin-1'), 4, "signature")

        def _writestr(self, string):
                res = string.encode('utf-16')
                # Overwrite 2-byte Unicode BOM with string length.
                chrcnt = (len(res)-2)/2
                res[0] = chrcnt)
                res[1] = (chrcnt>>8)
                reslen = ((len(res)+1+3) & (~3))
                resremainder = reslen - len(res)
                res += b"\0" * resremainder
                self._dowrite(res, reslen, "str data")

        def _writenumber(self, val):
                if type(val) == int:
                        self._writeint(val)
                elif type(val) == float:
                        self._writefloat(val)
                else:
                        print("Error: could not write number type %s." % str(type(val)))
                        sys.exit(15)

        def _writefloat(self, val, name="float"):
                data = struct.pack(">f", val)
                self._dowrite(data, 4, name)

        def _writeint(self, val, name="int"):
                data = struct.pack(">i", val)
                self._dowrite(data, 4, name)

        def _dowrite(self, data, length, name):
                if len(data) != length:
                        print("Error: trying to write bad %s '%s'." % (name, data))
                        sys.exit(10)
                self.f.write(data)


        @staticmethod
        def _geteuler(node):
                q = node.get_local_quat()
                w2 = q[0]*q[0]
                x2 = q[1]*q[1]
                y2 = q[2]*q[2]
                z2 = q[3]*q[3]
                unitlength = w2 + x2 + y2 + z2  # Normalised == 1, otherwise correction divisor.
                abcd = q[0]*q[1] + q[2]*q[3]
                if abcd > (0.5-0.0001)*unitlength:
                        yaw = 2 * atan2(q[2], q[0])
                        pitch = math.pi
                        roll = 0
                elif abcd < (-0.5+0.0001)*unitlength:
                        yaw = -2 * math.atan2(q[2], q[0])
                        pitch = -math.pi
                        roll = 0
                else:
                        adbc = q[0]*q[3] - q[1]*q[2]
                        acbd = q[0]*q[2] - q[1]*q[3]
                        yaw = math.atan2(2*adbc, 1 - 2*(z2+x2))
                        pitch = math.asin(2*abcd/unitlength)
                        roll = math.atan2(2*acbd, 1 - 2*(y2+x2))
                return yaw, pitch, roll


        def _getaxes(self, node):
                q = node.get_local_quat()
                return q.rotateVec((1,0,0)), q.rotateVec((0,1,0)), q.rotateVec((0,0,1))


        def _findglobalnode(self, simplename):
                for node in self.group:
                        if node.getName() == simplename:
                                return node
                print("Warning: node '%s' not found!" % name)
                return None



class PhysWriter(ChunkyWriter):
        """Translates a node/attribute group and writes it to disk as physics+mesh chunky files."""

        def __init__(self, basename, group, config):
                ChunkyWriter.__init__(self, basename, group, config)


        def dowrite(self):
                filename = self.basename+".phys"
                with open(filename, "wb") as f:
                        self.f = f
                        bones = []
                        engines = []
                        data =  (
                                        CHUNK_PHYSICS,
                                        (
                                                (CHUNK_PHYSICS_BONE_COUNT, self._count_transforms()),
                                                (CHUNK_PHYSICS_PHYSICS_TYPE, physics_type[self.config["type"]]),
                                                (CHUNK_PHYSICS_ENGINE_COUNT, self._count_engines()),
                                                (CHUNK_PHYSICS_BONE_CONTAINER, bones),
                                                (CHUNK_PHYSICS_ENGINE_CONTAINER, engines)
                                        )
                                )
                        for node in self.bodies:
                                # TODO: add optional saves of child bones.
                                bones.append((CHUNK_PHYSICS_BONE_TRANSFORM, node))
                                bones.append((CHUNK_PHYSICS_BONE_SHAPE, self._getshape(node)))
                        for node in self.group:
                                if node.nodetype.startswith("engine:"):
                                        engines.append((CHUNK_PHYSICS_ENGINE, node))
                        #pprint.pprint(data)
                        self._writechunk(data)


        def _writeshape(self, shape):
                # Write all general parameters first.
                types = {"capsule":1, "sphere":2, "box":3}
                self._writeint(types[shape.type])
                node = shape.getnode()
                self._writefloat(node.get_fixed_attribute("mass"))
                self._writefloat(node.get_fixed_attribute("friction"))
                self._writefloat(node.get_fixed_attribute("bounce"))
                self._writeint(-1 if not node.phys_parent else self.bodies.index(node.phys_parent))
                joints = {None:1, "exclude":1, "suspend_hinge":2, "hinge2":3, "hinge":4, "ball":5, "universal":6}
                jointtype = node.get_fixed_attribute("joint", True)
                jointvalue = joints[jointtype]
                if jointtype:
                        self._addfeat("joint:joints", 1)
                self._writeint(jointvalue)
                self._writeint(1 if node.get_fixed_attribute("affected_by_gravity") else 0)
                # Write joint parameters.
                parameters = [0.0]*16
                parameters[0] = node.get_fixed_attribute("spring_constant", True, 0.0)
                parameters[1] = node.get_fixed_attribute("spring_damping", True, 0.0)
                yaw, pitch, roll = ChunkyWriter._geteuler(node)
                #if jointvalue != 1 and (pitch < -0.1 or pitch > 0.1):
                #        print("Error: euler rotation pitch of jointed body '%s' must be zero." % node.getFullName())
                #        sys.exit(19)
                parameters[2] = yaw
                parameters[3] = roll
                joint_min, joint_max = node.get_fixed_attribute("joint_angles", True, [0.0,0.0])
                joint_min, joint_max = math.radians(joint_min), math.radians(joint_max)
                parameters[4] = joint_min
                parameters[5] = joint_max
                lq = node.get_local_quat()
                lp = node.get_local_pivot()
                j = lq*lp
                parameters[6] = j.x
                parameters[7] = j.y
                parameters[8] = j.z
                for x in parameters:
                        self._writefloat(x)
                # Write connecor type (may hook other stuff, may get hooked by hookers :).
                connectors = node.get_fixed_attribute("connector_types", True)
                if connectors:
                        if not type(connectors) == list and not type(connetors) == tuple:
                                print("Error: connector_types for '%s' must be a list." % node.getFullName())
                                sys.exit(19)
                        self._writeint(len(connectors))
                        connector_types = {"connector_3dof":1, "connectee_3dof":2}
                        for c in connectors:
                                self._writeint(connector_types[c])
                else:
                        self._writeint(0)
                # Write shape data (dimensions of shape).
                for x in shape.data:
                        self._writefloat(x)
                #print("Wrote shape with axes", self._getaxes(node))
                self._addfeat("physical geometry:physical geometries", 1)


        def _writeengine(self, node):
                # Write all general parameters first.
                types = {"walk":1, "cam_flat_push":2, "hinge_roll":3, "hinge2_roll":3, "hinge2_turn":4, "hinge2_break":5, "hinge":6, "glue":7}
                self._writeint(types[node.get_fixed_attribute("type")])
                self._writefloat(node.get_fixed_attribute("strength"))
                self._writefloat(node.get_fixed_attribute("max_velocity")[0])
                self._writefloat(node.get_fixed_attribute("max_velocity")[1])
                self._writefloat(node.get_fixed_attribute("controller_index"))
                connected_to = node.get_fixed_attribute("connected_to")
                connected_to = self._expand_connected_list(connected_to)
                if len(connected_to) < 1:
                        print("Error: could not find any matching nodes to connected engine '%s' to." % node.getFullName())
                        sys.exit(19)
                self._writeint(len(connected_to))
                for connection in connected_to:
                        body, scale, connectiontype = connection
                        self._writeint(self.bodies.index(body))
                        self._writefloat(scale)
                        connectiontypes = {"normal":1, "half_lock":2}
                        self._writeint(connectiontypes[connectiontype])
                #print("Wrote engine '%s' for %i nodes." % (node.getName()[6:], len(connected_to)))
                self._addfeat("physical engine:physical engines", 1)


        def _expand_connected_list(self, unexpanded):
                expanded = []
                for e in unexpanded:
                        noderegexp, scale, ctype = e
                        for body in self.bodies:
                                if re.search("^"+noderegexp+"$", body.getFullName()[1:]):
                                        expanded += [(body, scale, ctype)]
                return expanded


        def _getshape(self, node):
                shapenode = self._findphyschildnode(parent=node, nodetype="mesh")
                if not shapenode:
                        print("Error: shape for node '%s' does not exist." % node.getFullName())
                        sys.exit(11)
                in_nodename = shapenode.getInNode("i", "i")[0]
                if not in_nodename:
                        print("Error: input shape for node '%s' does not exist." % shapenode.getFullName())
                        sys.exit(12)
                in_node = self._findglobalnode(in_nodename)
                if not in_node:
                        print("Error: unable to find input shape node '%s'." % in_nodename)
                        sys.exit(13)
                if not in_node.nodetype in ["polyCube", "polySphere"]:
                        print("Error: input shape node '%s' is of unknown type '%s'." % (in_node.getFullName(), in_node.nodetype))
                        sys.exit(14)
                return shape.Shape(node, in_node)


        def _findphyschildnode(self, parent, nodetype):
                for node in self.group:
                        if node.getName().startswith("phys_") and node.nodetype == nodetype and node.getParent() == parent:
                                return node
                print("Warning: certain node not found!")
                return None


        def _count_transforms(self):
                count = 0
                for node in self.group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                 count += 1
                return count

        def _count_engines(self):
                count = 0
                for node in self.group:
                        if node.nodetype.startswith("engine:"):
                                 count += 1
                return count



class MeshWriter(ChunkyWriter):
        """Translates a node/attribute group and writes it to disk as mesh chunky files."""

        def __init__(self, basename, group, config):
                ChunkyWriter.__init__(self, basename, group, config)

        def dowrite(self):
                for node in self.group:
                        if node.get_fixed_attribute("rgvtx", optional=True):
                                nodemeshname = node.getName().replace("Shape", "")
                                if nodemeshname.startswith("m_"):
                                        nodemeshname = nodemeshname[2:]
                                meshbasename = self.basename+"_"+nodemeshname
                                node.getParent().meshbasename = meshbasename
                                self.writemesh(meshbasename+".mesh", node)

        def writemesh(self, filename, node):
                #print("Writing mesh %s with %i triangles..." % (filename, len(node.get_fixed_attribute("rgtri"))/3))
                self._addfeat("mesh:meshes", 1)
                self._addfeat("gfx triangle:gfx triangles", len(node.get_fixed_attribute("rgtri"))/3)
                with open(filename, "wb") as f:
                        self.f = f
                        default_mesh_type = {"static":1, "dynamic":2, "volatile":3}
                        data =  (
                                        CHUNK_MESH,
                                        (
                                                (CHUNK_MESH_VERTICES, node.get_fixed_attribute("rgvtx")),
                                                (CHUNK_MESH_TRIANGLES, node.get_fixed_attribute("rgtri")),
                                                (CHUNK_MESH_VOLATILITY, default_mesh_type["static"]),
                                        )
                                )
                        self._writechunk(data)



class ClassWriter(ChunkyWriter):
        """Translates a node/attribute class and writes it to disk as a class chunky file."""

        def __init__(self, basename, group, config):
                ChunkyWriter.__init__(self, basename, group, config)


        def dowrite(self):
                self._listchildmeshes()
                filename = self.basename+".class"
                with open(filename, "wb") as f:
                        self.f = f
                        meshes = []
                        data =  (
                                        CHUNK_CLASS,
                                        (
                                                (CHUNK_CLASS_PHYSICS, self.basename),
                                                (CHUNK_CLASS_MESH_LIST, meshes),
                                        )
                                )
                        physidx = 0
                        for phys in self.bodies:
                                if phys.childmeshes:
                                        #print("%s:" % phys.getFullName())
                                        for m in phys.childmeshes:
                                                tm = m.get_world_transform()
                                                tp = phys.get_world_transform()
                                                lpm = tm * vec4(m.get_local_pivot()[:]+[1])
                                                lpp = tp * vec4(phys.get_local_pivot()[:]+[1])
                                                t = tp.inverse() * tm
                                                q = quat().fromMat(t).normalize()
                                                p = (lpm-lpp)[0:3]
                                                meshes += [(CHUNK_CLASS_PHYS_MESH, PhysMeshPtr(physidx, m.meshbasename, q, p))]
                                        physidx += 1
                        #pprint.pprint(data)
                        self._writechunk(data)
                        self._addfeat("class:classes", 1)


        def _writephysmeshptr(self, physmeshptr):
                self._writeint(physmeshptr.physidx)
                self._writestr(physmeshptr.meshbasename)
                self._writexform(physmeshptr.t)
                self._addfeat("phys->mesh ptr:phys->mesh ptrs", 1)


        def _listchildmeshes(self):
                for node in self.bodies:
                        node.childmeshes = []
                        if not node.getParent().getName().startswith("m_"):
                                continue
                        parent = node.getParent()
                        node.childmeshes += [parent]
                        def recurselistmeshes(n, to):
                                mc = []
                                if not n.phys_children:
                                        for m in n.mesh_children:
                                                mc += [n]
                                #print(n.getName(), str(n.mesh_children))
                                for cn in n.mesh_children:
                                        mc += recurselistmeshes(cn, to)
                                return mc
                        node.childmeshes += recurselistmeshes(parent, parent.getParent())
