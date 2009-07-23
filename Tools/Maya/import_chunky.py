# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-07-17 for Righteous Engine tool chain.


from mayaascii import *
from quat import *
import shape

from functools import reduce
import configparser
import pprint
import re
import struct
import sys
import types


physics_type = {"static":1, "dynamic":2, "collision_detect_only":3}

CHUNK_STRUCTURE                    = "STRU"
CHUNK_STRUCTURE_BONE_COUNT         = "STBC"
CHUNK_STRUCTURE_PHYSICS_TYPE       = "STPT"
CHUNK_STRUCTURE_ENGINE_COUNT       = "STEC"
CHUNK_STRUCTURE_BONE_CONTAINER     = "STBO"
CHUNK_STRUCTURE_BONE_CHILD_LIST    = "SBCL"
CHUNK_STRUCTURE_BONE_TRANSFORM     = "STBT"
CHUNK_STRUCTURE_BONE_SHAPE         = "STSH"
CHUNK_STRUCTURE_ENGINE_CONTAINER   = "STEO"
CHUNK_STRUCTURE_ENGINE             = "STEN"



class GroupReader(DefaultMAReader):
        """Reads two files: a .ma and a .ini, sort outs islands. The islands that contain "bad" types
        are sorted out, the others are kept for writing to Chunky file in the future.
        Only ONE group is allowed to exist per .ma file."""

        def __init__(self, basename):
                DefaultMAReader.__init__(self)
                self.bad_types = ["camera", "lightLinker", "displayLayerManager", "displayLayer", \
                                  "renderLayerManager", "renderLayer", "script", "<unknown>"]
                self.basename = basename
                self.read(basename+".ma")


        def end(self):
                islands = self.gatherIslands(self.nodelist)
                if not islands:
                        print("Bad file! Terminating due to error.")
                        sys.exit(1)
                islands = self.filterIslands(islands)
                if len(islands) != 1:
                        # TODO: print objects from each island.
                        print("More than a single island (total %i) exists in the .ma file! Terminating due to error." % len(islands))
                        sys.exit(2)
                group = islands[0]
                config = configparser.SafeConfigParser()
                ininame = self.basename+".ini"
                if not config.read(ininame):
                        print("Error: could not open tweak file '%s'!" % ininame)
                        sys.exit(3)
                self.flipaxis(group)
                if not self.applyConfig(config, group):
                        print("Error: could not apply configuration.")
                        sys.exit(4)
                if not self.validateGroup(group):
                        #self.printnodes(group)
                        print("Invalid group/island! Terminating due to error.")
                        sys.exit(3)
                self.group = group


        # Override
        def createNode(self, nodetype, opts):
                """Overridden to create fixed attributes, used for being able to override
                   configuration settings separate to the Maya ASCII .ma file."""
                node = super(GroupReader, self).createNode(nodetype, opts)
                node._fixattr = {}
                def fix_attribute(self, name, value):
                        try:
                                value = eval(value)
                        except:
                                pass
                        self._fixattr[name] = value
                def get_fixed_attribute(self, name, optional=False, default=None):
                        try:
                                return self._fixattr[name]
                        except KeyError:
                                value = self.getAttrValue(name, name, None, default=default)
                                if value == None:
                                        if not optional:
                                                raise KeyError("Error fetching required attribute '%s'." % name)
                                        value = default
                                return value
                def get_inherited_attr(self, name):
                        try:
                                return self.get_fixed_attribute(name)
                        except KeyError:
                                parent = self.getParent()
                                if not parent:
                                        raise AttributeError("Error: could not fetch inherited attr '%s' from node '%s'." % (name, node.getFullName()))
                                return parent.get_inherited_attr(name)
                def get_fixed_tab(self):
                        tab = self._fixattr.copy()
                        tab.update(self._setattr)
                        return tab
##                def get_world_translation(self):
##                        if self.getParent():
##                                pt = self.get_world_translation(self.getParent())
##                                pq = self.getParent().get_world_orientation()
##                        else:
##                                pt = (0, 0, 0)
##                                pq = quat(1,0,0,0)
##                        p = self.get_fixed_attribute("t", optional=True, default=(0,0,0))
##                        p = pq.rotateVec(p)
##                        p = tuple(map(lambda x,y: x+y, pt, p))
##                        return p
##                def get_world_orientation(self):
##                        if self.getParent():
##                                pq = self.get_world_orientation(self.getParent())
##                        else:
##                                pq = quat(1, 0, 0, 0)
##                        rot = self.get_fixed_attribute("r", optional=True, default=(0,0,0))
##                        qx = quat().fromAngleAxis(rot[0], (1, 0, 0))
##                        qy = quat().fromAngleAxis(rot[1], (0, 1, 0))
##                        qz = quat().fromAngleAxis(rot[2], (0, 0, 1))
##                        q = pq*qx*qz*qy
##                        return q
                def gettrans(self):
                        return self.get_fixed_attribute("t", default=(0,0,0))
                def getrot(self):
                        rot = self.get_fixed_attribute("r", default=(0,0,0))
                        qx = quat().fromAngleAxis(rot[0], (1, 0, 0))
                        qy = quat().fromAngleAxis(rot[1], (0, 1, 0))
                        qz = quat().fromAngleAxis(rot[2], (0, 0, 1))
                        q = qy*qz*qx
                        return q
                def get_world_scale(self):
                        node = self
                        s = (1.0, 1.0, 1.0)
                        while node:
                                scale = node.get_fixed_attribute("s", default=(1,1,1))
                                if len(scale) != 3 or reduce(lambda x,y: x*y, scale) == 0:
                                        print("Error: wrong number of scale scalars, or some scale is zero for node '%s'." % node.getFullName())
                                        sys.exit(19)
                                s = tuple(map(lambda x, y: x*y, s, scale))
                                node = node.getParent()
                        return s
                node.fix_attribute = types.MethodType(fix_attribute, node)
                node.get_fixed_attribute = types.MethodType(get_fixed_attribute, node)
                node.get_inherited_attr = types.MethodType(get_inherited_attr, node)
                node.get_fixed_tab = types.MethodType(get_fixed_tab, node)
                node.gettrans = types.MethodType(gettrans, node)
                node.getrot = types.MethodType(getrot, node)
                node.get_world_scale = types.MethodType(get_world_scale, node)
                return node


        def gatherIslands(self, nodes):
                islands = []
                for node in nodes:
                        parentName = node.getParentName()
                        if parentName:
                                parentNode = self.findNode(parentName)
                                if not parentNode:
                                        print("Error: parent %s does not exist!" % parentName)
                                        return None
                                if not self._insertInSameIslandAs(islands, parentNode, node):
                                        return None
                        outnodes = node.getOutNodes("out", "out")
                        if len(outnodes) == 1:
                                if not self._insertInSameIslandAs(islands, outnodes[0][0], node):
                                        return None
                        elif len(outnodes) > 1:
                                print("Error: more than one output node not yet supported!")
                                return None
                        elif not parentName:
                                islands.append([node])
                return islands


        def filterIslands(self, islands):
                for island in islands:
                        for n in island:
                                if n.nodetype in self.bad_types:
                                        #print("Removing %s." % n.nodetype)
                                        islands.remove(island)
                                        self.filterIslands(islands)
                                        break
                return islands


        def flipaxis(self, group):
                for node in group:
                        if node.nodetype == "transform":
                                pos = node.getAttrValue("t", "t", None, default=(0,0,0))
                                node.fix_attribute("t", (pos[0], -pos[2], pos[1]))
                                rot = node.getAttrValue("r", "r", None, default=(0,0,0))
                                node.fix_attribute("r", (math.radians(rot[0]), -math.radians(rot[2]), math.radians(rot[1])))
                                scale = node.getAttrValue("s", "s", None, default=(1,1,1))
                                node.fix_attribute("s", (scale[0], scale[2], scale[1]))

        def applyConfig(self, config, group):
                allApplied = True

                used_sections = {}
                # Apply config to bodies.
                for node in group:
                        for section in config.sections():
                                if section.startswith("body:") and re.search("^"+section[5:]+"$", node.getFullName()[1:]):
                                        used_sections[section] = True
                                        params = config.items(section)
                                        for name, value in params:
                                                node.fix_attribute(name, value)

                # Create motors.
                for section in config.sections():
                        if section.startswith("motor:"):
                                motortype = stripQuotes(config.get(section, "type"))
                                motorOk = motortype in ["hinge2_roll", "hinge2_turn", "hinge2_break", "hinge"]
                                allApplied &= motorOk
                                if not motorOk:
                                        print("Error: invalid engine type '%s'." % motortype)
                                node = self.onCreateNode("motor:"+motortype, {"name":[section]})
                                motor_attribute = {}
                                params = config.items(section)
                                for name, value in params:
                                        node.fix_attribute(name, value)
                                def check_connected_to(l):
                                        ok = (len(l) >= 1)
                                        for e in l:
                                                ok = ok and (len(e) == 3)
                                                ok = ok and (type(e[0]) == str)
                                                ok = ok and (e[1] >= -100 and e[1] <= 100)
                                                ok = ok and (e[2] in ["normal", "half_lock"])
                                                # TODO: verify that this actually gets connected to a body.
                                        return ok
                                required = [("type", lambda x: type(x) == str),
                                            ("strength", lambda x: x > 0 and x < 30000),
                                            ("max_velocity", lambda x: len(x)==2 and x[0]>=0 and x[0]<=300 and x[1]>=0 and x[1]<=300),
                                            ("controller_index", lambda x: x >= 0 and x < 20),
                                            ("connected_to", check_connected_to)]
                                for name, motor_check in required:
                                        allApplied &= self._queryAttribute(node, name, motor_check)[0]
                                group.append(node)
                                used_sections[section] = True

                # Fetch general settings.
                self.config = {}
                for section in config.sections():
                        if section.startswith("config:"):
                                params = config.items(section)
                                for name, value in params:
                                        self.config[name] = stripQuotes(value)
                                used_sections[section] = True
                required = [("type", lambda x: physics_type.get(x) != None)]
                for name, config_check in required:
                        allApplied &= config_check(self.config.get(name))

                for section in config.sections():
                        if not used_sections.get(section):
                                print("Error: configuration secion '%s' not in use!" % section)
                                allApplied = False
                return allApplied


        def validateGroup(self, group):
                isGroupValid = True
                transformIsRoot = True
                for node in group:
                        if node.nodetype == "transform":
                                # Check attributes.
                                def jointCheck(t):
                                        return (t == "suspend_hinge") or (t == "hinge2") or (t == "hinge")
                                isValid, hasJoint = self._queryAttribute(node, "joint", jointCheck, False)
                                isGroupValid &= isValid
                                if transformIsRoot and isValid and hasJoint:
                                        print("Error: root node %s may not be jointed to anything else!" % node.getFullName())
                                        isGroupValid = False
                                isGroupValid &= self._queryAttribute(node, "mass", lambda x: (x > 0 and x < 1000000))[0]
                                isGroupValid &= self._queryAttribute(node, "bounce", lambda x: (x >= 0 and x <= 1))[0]
                                isGroupValid &= self._queryAttribute(node, "friction", lambda x: (x >= 0 and x <= 100))[0]
                                isGroupValid &= self._queryAttribute(node, "affected_by_gravity", lambda x: x==True or x==False)[0]
                                if transformIsRoot:
                                        isGroupValid &= self.validateHierarchy(node, group)
                                transformIsRoot = False
                return isGroupValid



        def validateHierarchy(self, basenode, group):
                isHierarchyValid = True
                childcount = {}
                childlist = self._listchildnodes(basenode.getName(), basenode, group)
                for node in childlist:
                        count = childcount.get(node)
                        if not count:
                                count = 0
                        count += 1
                        childcount[node] = count
                for key, count in childcount.items():
                        if count > 1:
                                print("Error: more than one path (currently %i paths) lead to node '%s'." % (count, key.getFullName()))
                                isHierarchyValid = False
                return isHierarchyValid


        def _listchildnodes(self, path, basenode, group):
                childlist = []
                for node in group:
                        if node.getParent() == basenode:
                              childlist += [node]
                if childlist:
                        print("Children for %s:" % path)
                        for child in childlist:
                                print("  - %s" % child.getName())
                grandchildlist = []
                for child in childlist:
                        childpath = path+"->"+child.getName()
                        grandchildlist += self._listchildnodes(childpath, child, group)
                return childlist+grandchildlist


        def printnodes(self, nodes, attrs=True):
                for node in nodes:
                        print('%-30s %-20s parent:%s' % ('"'+node.getFullName()+'"', node.nodetype, node.getParentName()))
                        if not attrs:
                                continue
                        for attrname in node.get_fixed_tab().keys():
                                try:
                                        val = node.get_fixed_attribute(attrname)
                                except:
                                        val = "<error retrieving value, need more type information>"
                                val = str(val)
                                if len(val)>60:
                                        val = val[:60]+"..."
                                print("  %s = %s" % (attrname, val))
                        in_node = node.getInNode("i", "i")
                        if in_node != (None, None):
                                print("  input node = %s" % in_node[0])


        def _queryAttribute(self, node, attrname, check, err_missing=True):
                try:
                        value = node.get_fixed_attribute(attrname)
                except KeyError:
                        if err_missing:
                                print("Error: missing required attribute '%s' in node %s." % (attrname, node.getFullName()))
                        return not err_missing, None
                isValid = check(value)
                if not isValid:
                        print("Error: attribute '%s' in node %s is invalid (value=%s)." % (attrname, node.getFullName(), str(value)))
                return isValid, value


        def _insertInSameIslandAs(self, islands, same, node):
                if self._isInIslands(islands, node):
                        print("Error: trying to re-insert already inserted node!")
                        return False
                for island in islands:
                        for n in island:
                                if n == same:
                                        island.append(node)
                                        return True
                print("Error: parent/associated node '%s' (related to '%s') does not exist in islands!" % (same.getFullName(), node.getFullName()))
                return False


        def _isInIslands(self, islands, node):
                for island in islands:
                        for n in island:
                                if n == node:
                                        return True
                return False


class GroupWriter:
        """Translates a node/attribute group and writes it to disk as chunky files."""

        def __init__(self, basename, group, config):
                self.group = group
                self.config = config
                self.writephysics(basename+".phys")

        def writephysics(self, filename):
                self.bodies = []
                with open(filename, "wb") as f:
                        self.f = f
                        bones = []
                        engines = []
                        data =  (
                                        CHUNK_STRUCTURE,
                                        (
                                                (CHUNK_STRUCTURE_BONE_COUNT, self._count_transforms()),
                                                (CHUNK_STRUCTURE_PHYSICS_TYPE, physics_type[self.config["type"]]),
                                                (CHUNK_STRUCTURE_ENGINE_COUNT, self._count_engines()),
                                                (CHUNK_STRUCTURE_BONE_CONTAINER, bones),
                                                (CHUNK_STRUCTURE_ENGINE_CONTAINER, engines)
                                        )
                                )
                        for node in self.group:
                                if node.nodetype == "transform":
                                        self.bodies.append(node)
                                        # TODO: add optional saves of child bones.
                                        bones.append((CHUNK_STRUCTURE_BONE_TRANSFORM, node))
                                        bones.append((CHUNK_STRUCTURE_BONE_SHAPE, self._getshape(node)))
                        for node in self.group:
                                if node.nodetype.startswith("motor:"):
                                        engines.append((CHUNK_STRUCTURE_ENGINE, node))
                        pprint.pprint(data)
                        self._writechunk(data)

        def _writechunk(self, chunks, name=None):
                t = type(chunks)
                if t == tuple or t == list:
                        islist = False
                        for chunk in chunks:
                                if type(chunk) != list and type(chunk) != tuple:
                                        if not islist and len(chunks) == 2:
                                                break
                                        print("Error: trying to write less/more than one chunk, which is not a subchunk.")
                                        sys.exit(15)
                                islist = True
                                self._writechunk(chunk, "subchunk of "+name)
                        if len(chunks) == 0 or islist:
                                return

                        if type(chunks[0]) != str:
                                print("Error: trying to write chunk without header signature (%s, type=%s, value='%s')." % (name, str(type(chunks[0])), str(chunks[0])))
                                sys.exit(16)
                        if type(chunks[1]) == int:
                                head = self._writeheader(chunks[0], 4)
                                self._writeint(chunks[1])
                        else:
                                head = self._writeheader(chunks[0])
                                self._writechunk(chunks[1], chunks[0])
                                self._rewriteheadersize(head)
                elif t == Node:
                        node = chunks
                        if node.nodetype == "transform":
                                self._writebone(node)
                        elif node.nodetype.startswith("motor:"):
                                self._writeengine(node)
                elif t == shape.Shape:
                        shapeChunk = chunks
                        self._writeshape(shapeChunk)
                else:
                        print("Error: trying to write unknown chunk data, type='%s', value='%s'." % str(t), str(chunks))
                        sys.exit(17)

        def _writebone(self, node):
                q = node.getrot()
                if not q:
                        print("Error: trying to get rotation from node '%s', but none available." % node.getFullName())
                        sys.exit(18)
                pos = node.gettrans()
                data = q.totuple()+pos
                for f in data:
                        self._writefloat(f)

        def _writeshape(self, shape):
                # Write all general parameters first.
                types = {"capsule":1, "sphere":2, "box":3}
                self._writeint(types[shape.type])
                node = shape.getnode()
                self._writefloat(node.get_fixed_attribute("mass"))
                self._writefloat(node.get_fixed_attribute("friction"))
                self._writefloat(node.get_fixed_attribute("bounce"))
                self._writeint(-1 if not node.getParent() else self.bodies.index(node.getParent()))
                joints = {None:1, "exclude":1, "suspend_hinge":2, "hinge2":3, "hinge":4, "ball":5, "universal":6}
                jointvalue = joints[node.get_fixed_attribute("joint", True)]
                self._writeint(jointvalue)
                self._writeint(1 if node.get_fixed_attribute("affected_by_gravity") else 0)
                # Write joint parameters.
                parameters = [0.0]*16
                parameters[0] = node.get_fixed_attribute("spring_constant", True, 0.0)
                parameters[1] = node.get_fixed_attribute("spring_damping", True, 0.0)
                yaw, pitch, roll = self._geteuler(node)
                #if jointvalue != 1 and (pitch < -0.1 or pitch > 0.1):
                #        print("Error: euler rotation pitch of jointed body '%s' must be zero." % node.getFullName())
                #        sys.exit(19)
                parameters[2] = yaw
                parameters[3] = roll
                parameters[4] = 0.0     # TODO: pick joint end-values from .ma!
                parameters[5] = 0.0     # TODO: pick joint end-values from .ma!
                parameters[6] = 0.0     # TODO: pick joint anchor from .ma!
                parameters[7] = 0.0     # TODO: pick joint anchor from .ma!
                parameters[8] = 0.0     # TODO: pick joint anchor from .ma!
                for x in parameters:
                        self._writefloat(x)
                # Write connecor type (may hook other stuff, may be hooked by hookers :).
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
                print("Wrote shape with axes", self._getaxes(node))

        def _writeengine(self, node):
                print("Would have written motor.")

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

        def _geteuler(self, node):
                q = node.getrot()
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
                q = node.getrot()
                return q.rotateVec((1,0,0)), q.rotateVec((0,1,0)), q.rotateVec((0,0,1))

        def _getshape(self, node):
                shapenode = self._findchildnode(parent=node, nodetype="mesh")
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

        def _findchildnode(self, parent, nodetype):
                for node in self.group:
                        if node.getParent() == parent and node.nodetype == nodetype:
                                return node
                print("Warning: certain node not found!")
                return None

        def _findglobalnode(self, simplename):
                for node in self.group:
                        if node.getName() == simplename:
                                return node
                print("Warning: node '%s' not found!" % name)
                return None

        def _count_transforms(self):
                count = 0
                for node in self.group:
                        if node.nodetype == "transform":
                                 count += 1
                return count

        def _count_engines(self):
                count = 0
                for node in self.group:
                        if node.nodetype.startswith("motor:"):
                                 count += 1
                return count


def main():
        if len(sys.argv) != 2:
                print("Usage: %s <basename>")
                print("Reads basename.ma and basename.ini and writes some output chunky files.")
                sys.exit(20)
        rd = GroupReader(sys.argv[1])
        wr = GroupWriter(sys.argv[1], rd.group, rd.config)
        rd.printnodes(rd.group, False)


if __name__=="__main__":
        main()
