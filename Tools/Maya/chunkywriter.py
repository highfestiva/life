# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-07-17 for Righteous Engine tool chain.


from mat3 import mat3
from mat4 import mat4
from mayaascii import *
from quat import quat
from vec3 import vec3
from vec4 import vec4
import options
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
CHUNK_PHYSICS_BONE_CHILD_LIST      = "PHCL"
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


        def write(self):
                self.bodies, self.meshes = self._sortgroup(self.group)
                self.dowrite()


        def _isunique(self, category, name):
                if not hasattr(self.__class__, category):
                        setattr(self.__class__, category, {})
                if getattr(self.__class__, category).get(name):
                        return False
                return True

        def _regunique(self, category, name):
                if not self._isunique(category, name):
                        print("Error: multiple objects of type %s with same name %s causing ambiguity!" % (category, name))
                        sys.exit(3)
                getattr(self.__class__, category)[name] = True


        def _fileopenwrite(self, filename):
                self._regunique("file", filename)
                return open(filename, "wb")


        def _verifywritten(self, when, nodes):
                #print("nodes[0].writecount =", nodes[0], nodes[0].writecount)
                notwritten = list(filter(lambda n: n.writecount != 1, nodes))
                if notwritten:
                        print("Error: the following objects were missed, and never written to disk, or written more than once (when writing %s):" % when)
                        def _p(n):
                                print("  - %s (written %i times)" % (n.getFullName(), n.writecount))
                        [_p(n) for n in notwritten]
                        sys.exit(3)


        def addfeats(self, feats):
                for k,v in self.feats.items():
                        oldv = feats.get(k)
                        if oldv:
                                feats[k] += v
                        else:
                                feats[k] = v


        def _sortgroup(self, group):
                bodies = []
                meshes = []
                for node in self.group:
                        node.writecount = 0
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                bodies += [node]
                        if node.getName().startswith("m_") and node.nodetype == "transform":
                                meshes += [node]
                def childlevel(node):
                        c = 0;
                        while node.getParent():
                                node = node.getParent()
                                c += 1
                        return c
                bodies.sort(key=childlevel)
                meshes.sort(key=childlevel)
                return bodies, meshes


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
                        elif len(chunks) == 2 and type(chunks[0]) == str and \
                                ((type(chunks[1]) != list and type(chunks[1]) != tuple) or len(chunks[1]) > 0):
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
                        else:
                                print("Error: can not write node '%s' of type '%s'" % (node.getFullName(), node.nodetype))
                                sys.exit(17)
                elif t == shape.Shape:
                        shapeChunk = chunks
                        self._writeshape(shapeChunk)
                elif t == PhysMeshPtr:
                        self._writephysmeshptr(chunks)
                else:
                        print("Error: trying to write unknown chunk data in %s, type='%s', value='%s'." % (name, str(t), str(chunks)))
                        sys.exit(17)


        def _writexform(self, xform):
                if len(xform) != 7:
                        print("Error: trying to store transform with len != 7!")
                        sys.exit(18)
                #x = 0
                for f in xform:
                        #print("Writing transform", x, ": ", f)
                        #x += 1
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
                res = list(string.encode('utf-16'))
                res += [0, 0]
                # Overwrite 2-byte Unicode BOM with string length.
                chrcnt = (len(res)-2)//2
                res[0] = (chrcnt&0xFF)
                res[1] = (chrcnt>>8)
                reslen = ((len(res)+3) & (~3))
                resremainder = reslen - len(res)
                res += [0] * resremainder
                res = bytes(res)
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
                if type(val) != float:
                        raise ValueError("Error: trying to pass '" + str(type(val)) + "' (with value " + str(val) + ") off as 'float'.")
                data = struct.pack(">f", val)
                self._dowrite(data, 4, name)

        def _writeint(self, val, name="int"):
                if type(val) != int:
                        raise ValueError("Error: trying to pass '%s' off as 'int'." % str(type(val)))
                data = struct.pack(">i", val)
                self._dowrite(data, 4, name)

        def _dowrite(self, data, length, name):
                if len(data) != length:
                        print("Error: trying to write bad %s '%s'." % (name, data))
                        sys.exit(10)
                self.f.write(data)


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
                #print("Writing physics...", self.bodies)
                filename = self.basename+".phys"
                with self._fileopenwrite(filename) as f:
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
                                #print("Children of %s: %s." % (node.getFullName(), repr(node.phys_children)))
                                #[print("  - "+n.getName()) for n in node.phys_children]
                                childlist = [self.bodies.index(n) for n in node.phys_children]
                                bones.append((CHUNK_PHYSICS_BONE_CHILD_LIST, childlist))
                                bones.append((CHUNK_PHYSICS_BONE_TRANSFORM, node))
                                bones.append((CHUNK_PHYSICS_BONE_SHAPE, self._getshape(node)))
                        for node in self.group:
                                if node.nodetype.startswith("engine:"):
                                        engines.append((CHUNK_PHYSICS_ENGINE, node))
                        #pprint.pprint(data)
                        self._writechunk(data)
                self._verifywritten("physics", self.bodies)
                self._addfeat("file:files", 1)


        def _writebone(self, node):
                pos, q = node.get_final_local_transform()
                if not node.phys_root and node.getParent().phys_children[0] == node:
                        pos = [pos.x, pos.y, -node.lowestpos.z]
                data = q[:]+pos[:3]
                if options.options.verbose:
                        print("Writing bone %s with relative pos %s." % (node.getName(), pos))
                self._addfeat("bone:bones", 1)
                self._writexform(data)
                node.writecount += 1


        def _gettotalmass(self):
                totalmass = 0.0
                for body in self.bodies:
                        totalmass += body.get_fixed_attribute("mass")
                return totalmass


        def _writeshape(self, shape):
                # Write all general parameters first.
                types = {"capsule":1, "sphere":2, "box":3}
                self._writeint(types[shape.type])
                node = shape.getnode()
                totalmass = self._gettotalmass()
                self._writefloat(float(node.get_fixed_attribute("mass")))
                friction = node.get_fixed_attribute("friction") * totalmass / 1000.0
                self._writefloat(friction)
                self._writefloat(float(node.get_fixed_attribute("bounce")))
                self._writeint(-1 if not node.phys_root else self.bodies.index(node.phys_root))
                joints = {None:1, "exclude":1, "suspend_hinge":2, "hinge2":3, "hinge":4, "ball":5, "universal":6}
                jointtype = node.get_fixed_attribute("joint", True)
                jointvalue = joints[jointtype]
                #print(node.getName(), "is jointed by type", jointvalue)
                if jointtype:
                        self._addfeat("joint:joints", 1)
                self._writeint(jointvalue)
                self._writeint(1 if node.get_fixed_attribute("affected_by_gravity") else 0)
                # Write joint parameters.
                parameters = [0.0]*16
                #print("Total mass:", totalmass)
                parameters[0] = node.get_fixed_attribute("joint_spring_constant", True, 0.0) * totalmass
                parameters[1] = node.get_fixed_attribute("joint_spring_damping", True, 0.0) * totalmass

                ir = node.getabsirot()

                yaw = node.get_fixed_attribute("joint_yaw", True, 0.0)*math.pi/180
                pitch = node.get_fixed_attribute("joint_pitch", True, 0.0)*math.pi/180
##                m = mat4.identity()
##                m.setMat3(mat3.fromEulerXZY(0, pitch, yaw))
##                v = ir * m * vec4(0,0,1,0)
##                # Project onto XY plane.
##                xyv = vec3(v[:3])
##                xyv[2] = 0
##                if xyv.length() < 1e-12:
##                        yaw = 0
##                        if v[2] < 0:
##                                pitch = math.pi
##                        else:
##                                pitch = 0
##                else:
##                        xyv = xyv.normalize()
##                        # Yaw is angle of projection on the XY plane.
##                        if xyv.length():
##                                yaw = math.asin(xyv.y/xyv.length())
##                        else:
##                                yaw = 0
##                        v = v.normalize()
##                        v = vec3(v[:3])
##                        pitch = math.asin(v*xyv)
                if options.options.verbose and jointvalue >= 2:
                        print("Joint %s euler angles are: %s." % (node.getName(), (yaw, pitch)))
                parameters[2:4] = yaw, pitch

                joint_min, joint_max = node.get_fixed_attribute("joint_angles", True, [0.0,0.0])
                parameters[4:6] = math.radians(joint_min), math.radians(joint_max)

                mp = node.get_world_pivot_transform()
                mt = node.get_world_transform()
                wp = mp * vec4(0,0,0,1)
                wt = mt * vec4(0,0,0,1)
                j = wp-wt
                j = ir * j

                parameters[6:9] = j[:3]
                for x in parameters:
                        self._writefloat(float(x))
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
                if options.options.verbose:
                        print("Writing physical shape %s with data %s." % (node.getName(), shape.data))
                for x in shape.data:
                        self._writefloat(math.fabs(x))
                self._addfeat("physical geometry:physical geometries", 1)


        def _writeengine(self, node):
                # Write all general parameters first.
                types = {"walk":1, "cam_flat_push":2, "hinge_roll":3, "hinge_break":4, "hinge_torque":5, "hinge2_turn":6, "glue":7}
                self._writeint(types[node.get_fixed_attribute("type")])
                totalmass = self._gettotalmass()
                self._writefloat(node.get_fixed_attribute("strength")*totalmass)
                self._writefloat(float(node.get_fixed_attribute("max_velocity")[0]))
                self._writefloat(float(node.get_fixed_attribute("max_velocity")[1]))
                self._writeint(node.get_fixed_attribute("controller_index"))
                connected_to = node.get_fixed_attribute("connected_to")
                connected_to = self._expand_connected_list(connected_to)
                if len(connected_to) < 1:
                        print("Error: could not find any matching nodes to connected engine '%s' to." % node.getFullName())
                        sys.exit(19)
                self._writeint(len(connected_to))
                for connection in connected_to:
                        body, scale, connectiontype = connection
                        idx = self.bodies.index(body)
                        #print("Engine '%s's body index is %i."% (node.getName(), idx))
                        self._writeint(idx)
                        self._writefloat(float(scale))
                        connectiontypes = {"normal":1, "half_lock":2}
                        self._writeint(connectiontypes[connectiontype])
                #print("Wrote engine '%s' for %i nodes." % (node.getName()[6:], len(connected_to)))
                node.writecount += 1
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
##                shapenode = self._findphyschildnode(parent=node, nodetype="mesh")
##                if not shapenode:
##                        print("Error: shape for node '%s' does not exist." % node.getFullName())
##                        sys.exit(11)
##                in_nodename = shapenode.getInNode("i", "i")[0]
##                if not in_nodename:
##                        print("Error: input shape for node '%s' does not exist." % shapenode.getFullName())
##                        sys.exit(12)
##                in_node = self._findglobalnode(in_nodename)
                in_node = node.shape
                if not in_node:
                        print("Error: unable to find input shape node '%s'." % in_nodename)
                        sys.exit(13)
                if not in_node.nodetype in ["polyCube", "polySphere"]:
                        print("Error: input shape node '%s' is of unknown type '%s'." % (in_node.getFullName(), in_node.nodetype))
                        sys.exit(14)
                return shape.Shape(node, in_node)


        def _findphyschildnode(self, parent, nodetype):
                return parent.shape
##                for node in self.group:
##                        if node.getName().startswith("phys_") and node.nodetype == nodetype and node.getParent() == parent:
##                                return node
##                print("Warning: certain node not found!")
##                return None


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
                                meshbasename = node.getParent().meshbasename
                                self.writemesh(meshbasename+".mesh", node)
                self._verifywritten("meshes", self.meshes)

        def writemesh(self, filename, node):
                if not self._isunique("file", filename):
                        if options.options.verbose:
                                print("Skipping write of instance %s." % filename)
                        self._addfeat("mesh instance:mesh instances", 1)
                        node.getParent().writecount += 1
                        return
                #print("Writing mesh %s with %i triangles..." % (filename, len(node.get_fixed_attribute("rgtri"))/3))
                self._addfeat("mesh:meshes", 1)
                self._addfeat("gfx triangle:gfx triangles", len(node.get_fixed_attribute("rgtri"))/3)
                with self._fileopenwrite(filename) as f:
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
                for p in node.getparents():
                        p.writecount += 1
                self._addfeat("file:files", 1)



class ClassWriter(ChunkyWriter):
        """Translates a node/attribute class and writes it to disk as a class chunky file."""

        def __init__(self, basename, group, config):
                ChunkyWriter.__init__(self, basename, group, config)


        def dowrite(self):
                self._listchildmeshes()
                filename = self.basename+".class"
                with self._fileopenwrite(filename) as f:
                        self.f = f
                        meshptrs = []
                        physidx = 0
                        for m in self.meshes:
                                def _getparentphys(m):
                                        ph = None
                                        mesh = m
                                        while mesh and not ph:
                                                for phys in self.bodies:
                                                        meshes = list(filter(None, [ch == mesh for ch in phys.childmeshes]))
                                                        if len(meshes) == 1:
                                                                if not ph:
                                                                        ph = phys
                                                                else:
                                                                        print("Error: both phys %s and %s has mesh refs to %s." % (ph.getFullName(), phys.getFullName(), mesh.getFullName()))
                                                                        print(ph.childmeshes, meshes)
                                                                        sys.exit(3)
                                                        elif len(meshes) > 1:
                                                                print("Error: phys %s has multiple mesh children refs to %s." % (phys.getFullName(), mesh.getFullName()))
                                                                sys.exit(3)
                                                mesh = mesh.getParent()
                                        if not ph:
                                                print("Warning: mesh %s is not attached to any physics object!" % m.getFullName())
                                        return ph
                                phys = _getparentphys(m)
                                if not phys:
                                        continue
                                phys.writecount = 1
                                m.writecount += 1
                                tm = m.get_world_transform()
                                tp = phys.get_world_transform()
                                tmt = tm.decompose()[0]
                                tpt, tpr, tps = tp.decompose()
                                tpt = mat4.translation(tpt)
                                tps = mat4.scaling(tps)
                                wpm = m.get_world_translation()
                                wpp = phys.get_world_translation()
                                #mat = tp.inverse() * mat4.translation(tmt)
                                mat = tpt.inverse() * tpr.inverse() * tps.inverse() * mat4.translation(tmt)
                                q = quat(mat.decompose()[1]).normalize()
                                p = wpm-wpp
                                p = q.toMat4() * p
                                p = p[0:3]
                                #print("Writing class", m.meshbasename, "relative to", phys.getName(), "with", q[:]+p[:])
                                physidx = self.bodies.index(phys)
                                meshptrs += [(CHUNK_CLASS_PHYS_MESH, PhysMeshPtr(physidx, m.meshbasename, q, p))]
                        data =  (
                                        CHUNK_CLASS,
                                        (
                                                (CHUNK_CLASS_PHYSICS, self.basename),
                                                (CHUNK_CLASS_MESH_LIST, meshptrs),
                                        )
                                )
                        #pprint.pprint(data)
                        self._writechunk(data)
                        self._addfeat("class:classes", 1)
                self._verifywritten("physics->mesh links", self.meshes)
                self._addfeat("file:files", 1)


        def _writephysmeshptr(self, physmeshptr):
                meshbasename = physmeshptr.meshbasename
                if not self._isunique("meshname", meshbasename):
                        physmeshptr.meshbasename = None
                        for x in range(2, 5000):
                                meshinstancename = meshbasename+";"+str(x)
                                if self._isunique("meshname", meshinstancename):
                                        physmeshptr.meshbasename = meshinstancename
                                        if options.options.verbose:
                                                print("Mesh instance %s picked." % meshinstancename)
                                        break
                        if not physmeshptr.meshbasename:
                                print("Error: could not find free mesh instance name!")
                                sys.exit(3)
                self._regunique("meshname", physmeshptr.meshbasename)
                self._writeint(physmeshptr.physidx)
                self._writestr(physmeshptr.meshbasename)
                self._writexform(physmeshptr.t)
                self._addfeat("phys->mesh ptr:phys->mesh ptrs", 1)


        def _listchildmeshes(self):
                for node in self.meshes:
                        node.isphyschild = False
                for node in self.bodies:
                        node.childmeshes = []
                        if not node.getParent() in self.meshes:
                                continue
                        if node.getParent().isphyschild:
                                continue
                        node.getParent().isphyschild = True
                        parent = node.getParent()
                        node.childmeshes += [parent]
                        def recurselistmeshes(n):
                                mc = []
                                if n and not n.phys_children:
                                        #print("Entering mesh", n)
                                        for m in n.mesh_children:
                                                mc += [n]
                                        for cn in n.mesh_children:
                                                mc += recurselistmeshes(cn)
                                return mc
                        cm = list(map(lambda x: x[0], filter(None, [recurselistmeshes(c) for c in parent.mesh_children])))
                        #print("Taking ownership of", cm, "to", node.getName())
                        node.childmeshes += cm
                #for node in self.bodies:
                #        print(node, "has mesh children", node.childmeshes)
