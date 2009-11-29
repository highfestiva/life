# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-07-17 for Righteous Engine tool chain.


from mat4 import *
from mayaascii import *
from quat import *
from vec3 import *
from vec4 import *
import rgnode
import chunkywriter
import options
import shape

from functools import reduce
import configparser
import datetime
import optparse
import os.path
import re
import sys
import types


modified_time = None


class TimePreProcessor(MAPreProcessor):
        "Overrides the default pre processor to try to fetch .ma modification time."
        def iterStringIntervals(self, _s):
                if _s.startswith("//Last modified: "):
                        M, D, Y, h, m, s, ToD = _s[22:].replace(",", "").replace(":", " ").split()
                        getmonth = lambda M: ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"].index(M)+1
                        def gettodoff(h, ToD):
                                h = int(h)
                                if h >= 12:
                                        h -= 12
                                h += ["AM", "PM"].index(ToD)*12
                                return h
                        M, D, Y, h, m, s = getmonth(M), int(D), int(Y), gettodoff(h, ToD), int(m), int(s)
                        global modified_time
                        modified_time = "%i-%2.2i-%2.2iT%2.2i:%2.2i:%2.2i" % (Y, M, D, h, m, s);
                return super(MAPreProcessor, self).iterStringIntervals(_s)


class GroupReader(DefaultMAReader):
        """Reads two files: a .ma and a .ini, sort outs islands. The islands that contain "bad" types
        are sorted out, the others are kept for writing to Chunky file in the future.
        Exactly TWO groups must to exist per .ma file: phys and mesh."""

        def __init__(self, basename):
                DefaultMAReader.__init__(self)
                self.bad_types = ["camera", "lightLinker", "displayLayerManager", "displayLayer", \
                                  "renderLayerManager", "renderLayer", "script", \
                                  "materialInfo", "groupId", "groupParts", \
                                  "deleteComponent", "softModHandle", "softMod", \
                                  "objectSet", "tweak", "imagePlane", "place2dTexture"]
                self.mat_types = ["lambert", "blinn", "phong", "shadingEngine", "layeredShader", \
                                  "file"]
                self.basename = basename


        def doread(self):
                self.read(self.basename+".ma")


        # Override.
        def create_pre_processor(self):
                return TimePreProcessor(self.lineHandler)


        # Override.
        def end(self):
                islands = self.gatherIslands(self.nodelist)
                if not islands:
                        print("Bad file! Terminating due to error.")
                        sys.exit(1)
                self.check_mesh_export(islands)
                islands, mat_islands = self.filterIslands(islands)
                if len(islands) != 1:
                        print("The number of standalone groups is not one (is totally %i) in the .ma file! These are the groups:" % len(islands))
                        for i in islands:
                                print("  "+str(list(map(lambda x: x.getFullName(), i))))
                                print("")
                        print("Terminating due to error.")
                        sys.exit(2)
                group = islands[0]
                mat_group = []
                for mati in mat_islands:
                        mat_group.extend(mati)
                if not group[0].getName().startswith("m_"):
                        print("Error: root must be a mesh named 'm_...' something, not '%s'." % group[0].getName())
                        sys.exit(2)
                config = configparser.SafeConfigParser()
                ininame = self.basename+".ini"
                if not config.read(ininame):
                        print("Error: could not open tweak file '%s'!" % ininame)
                        sys.exit(3)
                self.fixparams(group)

                self.fixroottrans(group)

                self.faces2triangles(group)
                if not self.validatehierarchy(group):
                        print("Invalid hierarchy! Terminating due to error.")
                        sys.exit(3)
                #if not self.validate_orthogonality(group):
                #        print("Error: the group is not completely orthogonal!")
                #        sys.exit(3)
                group = self.sortgroup(group)
                if not self.validate_mesh_group(group):
                        print("Invalid mesh group! Terminating due to error.")
                        sys.exit(3)
                if not self.apply_phys_config(config, group):
                        print("Error: could not apply configuration.")
                        sys.exit(4)
                if not self.validate_phys_group(group):
                        print("Invalid physics group! Terminating due to error.")
                        sys.exit(3)
                if not self.validate_shape_instances(group):
                        print("Invalid physics shapes! Terminating due to error.")
                        sys.exit(3)
                if not self.validategroup(group):
                        print("Invalid group! Terminating due to error.")
                        sys.exit(3)
                if not self.makephysrelative(group):
                        print("Internal vector math failed! Terminating due to error.")
                        sys.exit(3)
                self.makevertsrelative(group)
                self.mesh_instance_reuse(group)
                self.setphyspivot(group)

                if not self.fixmaterials(group, mat_group):
                        print("Materials are incorrectly modelled! Terminating due to error.")
                        sys.exit(3)

                #self.printnodes(group)

                self.group = group


        # Override
        def createNode(self, nodetype, opts):
                node = super(GroupReader, self).createNode(nodetype, opts)
                rgnode.adjustnode(node)
                return node


        def _recursiveremove(self, node, nodes):
                try:
                        nodes.remove(node)
                except ValueError:
                        pass
                children = tuple(filter(lambda n: n.getParent() == node, nodes))
                for child in children:
                        self._recursiveremove(child, nodes)


        def gatherIslands(self, nodes):
                for node in tuple(nodes):
                        if node.getName().startswith("i_"):
                                self._recursiveremove(node, nodes)
                islands = []
                allow_no_association = True
                prevnodecnt = len(nodes)
                while len(nodes) > 0:
                        curr_nodes = list(nodes)
                        for node in curr_nodes:
                                #print(node.getFullName())
                                parentName = node.getParentName()
                                if parentName:
                                        parentNode = self.findNode(parentName)
                                        if not parentNode:
                                                print("Error: parent %s does not exist!" % parentName)
                                                return None
                                        if not self._insert_in_same_island_as(islands, parentNode, node, False):
                                                return None
                                        nodes.remove(node)
                                outnodes = node.getOutNodes("out", "out")
                                if len(outnodes) == 1:
                                        if self._insert_in_same_island_as(islands, outnodes[0][0], node, allow_no_association):
                                                nodes.remove(node)
                                elif len(outnodes) > 1:
                                        print("Error: more than one output node not yet supported!")
                                        return None
                                elif not parentName:
                                        islands.append([node])
                                        nodes.remove(node)
                        #print("Inserted %i nodes in loop." % (prevnodecnt-len(nodes)))
                        if prevnodecnt == len(nodes):
                                if not allow_no_association:
                                        print("Error: could not insert all nodes in islands. Currently pending are:")
                                        for node in nodes:
                                                print("  "+node.getFullName())
                                        return None
                                allow_no_association = False
                        prevnodecnt = len(nodes)
                return islands


        def check_mesh_export(self, islands):
                for island in islands:
                        for n in island:
                                if n.getName() == "rg_export":
                                        if not modified_time:
                                                print("Error: maya file does not contain the v2009 format modification string.")
                                                sys.exit(3)
                                        t = n.get_fixed_attribute("time").strip("\"")
                                        t0 = datetime.datetime.strptime(t.split(".")[0], "%Y-%m-%dT%H:%M:%S")
                                        t1 = datetime.datetime.strptime(modified_time.split(".")[0], "%Y-%m-%dT%H:%M:%S")
                                        diff = t1-t0
                                        secondlimit = 3
                                        ok = (diff.days == 0 and diff.seconds <= secondlimit)
                                        if not ok:
                                                print("Error: .ma file was not saved within %i seconds from internal export. Time diff is %s." % (secondlimit, str(diff)))
                                                sys.exit(19)
                                        return True
                                else:
                                        #print(islands.index(island), n.getName(), "-", n.nodetype)
                                        pass
                print("Error: you need to manually do an internal export of meshes.")
                sys.exit(19)


        def filterIslands(self, islands):
                mat_islands = []
                for_islands = islands[:]
                for island in for_islands:
                        for n in island:
                                if n.nodetype in self.bad_types or n.getName().startswith("ignore_"):
                                        #print("Removing bad %s." % n.nodetype)
                                        islands.remove(island)
                                        break
                                if n.nodetype == "<unknown>" and n.getParent() == None:
                                        islands.remove(island)
                                        break
                                if n.nodetype in self.mat_types:
                                        islands.remove(island)
                                        mat_islands.append(island)
                return islands, mat_islands


        def fixparams(self, group):
                for node in group:
                        if node.nodetype == "transform":
                                def fix(node, aname, default, conv=None):
                                        val = node.getAttrValue(aname, aname, None, default=default)
                                        if val:
                                                if conv:
                                                        val = tuple(map(conv, val))
                                                val = vec3(val)
                                        node.fix_attribute(aname, val)
                                fix(node, "t", (0,0,0))
                                fix(node, "rp", (0,0,0))
                                fix(node, "rpt", (0,0,0))
                                fix(node, "sp", (0,0,0))
                                fix(node, "spt", (0,0,0))
                                fix(node, "r", (0,0,0), math.radians)
                                fix(node, "s", (1,1,1))
                                fix(node, "sh", (0,0,0))
                                fix(node, "ra", (0,0,0), math.radians)
                        vtx = node.getAttrValue("rgvtx", "rgvtx", None, n=None)
                        if vtx:
                                node.fix_attribute("rgvtx", vtx)


        def fixroottrans(self, group):
                '''Do some magic with the (mesh) root transformation. When writing the physics root we
                   will need the inverted initial transform, but 'til then we'll use our own coordinate
                   system (more natural when playing, less so when editing).'''
                t = group[0]
                #o = t.get_fixed_attribute("t")
                #if o.length() != 0:
                #        print("Error: root node %s must be placed in origo." % t.getName())
                #        sys.exit(3)
                ro = t.get_fixed_attribute("ro", optional=True, default=0)
                if ro != 0:
                        print("Error: root %s must have xyz rotation order!" % t.getName())
                        sys.exit(3)
                # Store inverse of rotation for later, then  set our preferred rotation angles in the exported vehicle.
                t.fix_attribute("ra", -t.get_fixed_attribute("ra"))
                #t.fix_attribute("r", -t.get_fixed_attribute("r"))
                ir = t.gettransformto(None, "inverse_initial_r")       # Store transformation for writing.
                #print("Had this inverse_initial_r:\n", ir)
                t.fix_attribute("ra", vec3(math.pi/2, 0, math.pi))
                t.fix_attribute("r", vec3(0,0,0))

                o = t.get_fixed_attribute("t", optional=True, default=(0,0,0))
                if vec3(o).length() > 1e-12:
                        print("Error: root object %s must be placed in origo!" % t.getName())
                        sys.exit(3)



        def faces2triangles(self, group):
                for node in group:
                        faces = node.get_fixed_attribute("rgf", optional=True)
                        if faces:
                                triangles = []
                                if not type(faces) == str:
                                        faces = faces[1]        # From ("(", "...", ")") to "..."
                                faces = eval(faces[1:-1])
                                for face in faces:
                                        for x in range(1, len(face)-1):
                                                triangles += [face[0], face[x], face[x+1]]
                                node.fix_attribute("rgtri", triangles)


        def makevertsrelative(self, group):
                for node in group:
                        vtx = node.get_fixed_attribute("rgvtx", optional=True)
                        if vtx:
                                for p in node.getparents():
                                        if not p.shape:
                                                p.shape = node
                                mnode = node.getParent()
                                meshroot = None
                                m_tr = mnode.gettransformto(meshroot, "original", getparent=lambda n: n.getParent())
                                if not m_tr:
                                        print("Mesh crash!")
                                m_s = m_tr.decompose()[2]
                                transform = mat4.scaling(m_s)
                                vp = vec4(0,0,0,1)
                                for idx in range(0, len(vtx), 3):
                                        vp[:3] = vtx[idx:idx+3]
                                        vp = transform*vp
                                        vtx[idx:idx+3] = vp[:3]


        def mesh_instance_reuse(self, group):
                instancecount = 0
                meshnames = {}
                for node in group:
                        if node.get_fixed_attribute("rgvtx", optional=True):
                                nodemeshname = node.getName().replace("Shape", "")
                                if nodemeshname.startswith("m_"):
                                        nodemeshname = nodemeshname[2:]
                                meshbasename = self.basename+"_"+nodemeshname
                                cnt = meshnames.get(meshbasename)
                                if cnt == None:
                                        cnt = 0
                                cnt += 1
                                meshnames[meshbasename] = cnt
                                #print("Setting", node.getParent(), "meshbasename to", meshbasename)
                                pcnt = 0
                                for p in node.getparents():
                                        p.meshbasename = meshbasename
                                        if pcnt > 0:
                                                instancecount += 1
                                                #print("Got mesh instance:", meshbasename)
                                        pcnt += 1
                for k,v in meshnames.items():
                        if v > 1:
                                instancecount += 1
                                #print("Got mesh instance:", k, v)
                if len(group) >= 8 and instancecount == 0:
                        print("%s: warning: has no mesh instances; highly unlikely! At least the wheels should be, right?" % self.basename)


        def setphyspivot(self, group):
                physroot = None
                lowestp = None
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                if not node.phys_root and node.getParent().phys_children[0] == node:
                                        physroot = node
                                physshape = shape.Shape(node, node.shape)
                                p = physshape.get_lowest_world_point()
                                if not lowestp or p.z < lowestp.z:
                                        lowestp = p
                #print("Setting physics lowest pos to", lowestp, "on", physroot)
                physroot.lowestpos = lowestp


        def fixmaterials(self, group, mat_group):
                ok = True
                for node in group:
                        if node.getName().startswith("m_") and node.nodetype == "transform":
                                class Material:
                                        pass
                                node.mat = Material()
                                node.mat.ambient  = [1.0]*3 + [1.0]
                                node.mat.diffuse  = [1.0, 0.0, 1.0, 1.0]
                                node.mat.specular = [0.7]*3 + [1.0]
                                node.mat.textures = []
                                node.mat.shader   = "???"

                                mesh = self._listchildnodes(node.getFullName(), node, "m_", group, False, \
                                        lambda n: n.get_fixed_attribute("rgvtx", optional=True))[0]
                                outs = mesh.getOutNodes("iog", "iog")
                                shaders = []
                                for o, _, attr in outs:
                                        if o.nodetype == "shadingEngine":
                                                if not o in shaders:
                                                        shaders.append(o)
                                if len(shaders) != 1:
                                        print("Warning: mesh %s om %s has wrong number of materials (%i)." % (mesh.getName(), node.getName(), len(shaders)))
                                        continue
                                shader = shaders[0]
                                material = shader.getInNode("ss", "ss")
                                material = self._getnode(material[0], mat_group)
                                node.mat.ambient  = [0.5]*3 + [1.0]
                                c = material.get_fixed_attribute("c", optional=True, default=[0.5]*3)
                                node.mat.diffuse  = list(c) + [1.0]
                                node.mat.specular = [0.7]*3 + [1.0]
                                node.mat.textures = ["some_texture.png"]
                                node.mat.shader   = "SomeShader"
                return ok


        def validate_orthogonality(self, group):
                isGroupValid = True
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                if not node.isortho():
                                        isGroupValid = False
                                        print("Warning: phys node '%s' should be orthogonal." % node.getFullName())
                return isGroupValid


        def validatehierarchy(self, group):
                isGroupValid = self._validate_non_recursive(group[0], group)
                isGroupValid &= self._validatenaming(group[0], group)
                return isGroupValid


        def sortgroup(self, group):
                meshes = []
                phys = []
                meshes = list(filter(lambda n: n.getName().startswith("m_"), group))
                rest = list(filter(lambda n: n not in meshes, group))
                physes = []
                for m in meshes:
                        phys = self.find_phys_root(m, rest)
                        if phys:
                                physes.append(phys)
                                rest.remove(phys)
                group = list(meshes) + physes + rest
                return group


        def validate_mesh_group(self, group):
                isGroupValid = True
                if not isGroupValid:
                        print("Error: hierarchy graph recursion not allowed!")
                for node in group:
                        vtx = node.get_fixed_attribute("rgvtx", optional=True)
                        polys = node.get_fixed_attribute("rgtri", optional=True)
                        if vtx and not polys:
                                isGroupValid = False
                                print("Error: mesh '%s' contains vertices, but no triangle definition." % node.getFullName())
                        elif not vtx and polys:
                                isGroupValid = False
                                print("Error: mesh '%s' contains triangle definitions but no vertices." % node.getFullName())
                        elif vtx and polys:
                                node.physcnt = 0
                                #print("Found mesh:", node.getName())
                                if not node.getName().startswith("m_"):
                                        isGroupValid = False
                                        print("Error: mesh '%s' must have a name prefixed with 'm_'!" % node.getFullName())
                                if len(vtx) % 3 != 0:
                                        isGroupValid = False
                                        print("Error: vertex count in '%s' is not a multiple of three!" % node.getFullName())
                                usedlastvertex = False
                                for p in polys:
                                        if p == len(vtx)/3-1:
                                                usedlastvertex = True
                                        if p >= len(vtx)/3:
                                                isGroupValid = False
                                                print("Error: polygon %i in '%s' is out of vertex range." % (polys.index(p), node.getFullName()))
                                if not usedlastvertex:
                                        isGroupValid = False
                                        print("Error: not all vertices in '%s' is used." % node.getFullName())
##                        elif node.nodetype == "mesh" and not node.getName().startswith("phys_") and not node.getName().startswith("m_"):
##                                isGroupValid = False
##                                print("Error: mesh '%s' must be prefixed 'phys_' or 'm_'." % node.getFullName())
                        if node.getName().startswith("m_") and node.nodetype == "transform":
                                node.mesh_children = list(filter(lambda x: x.nodetype == "transform", self._listchildnodes(node.getFullName(), node, "m_", group, False)))
                                node.phys_children = self._listchildnodes(node.getFullName(), node, "phys_", group, False)
                return isGroupValid

                                
        def apply_phys_config(self, config, group):
                allApplied = True

                used_sections = {}
                # Apply config to bodies.
                for node in group:
                        for section in config.sections():
                                if section.startswith("body:") and re.search("^"+section[5:]+"$", node.getFullName()[1:]):
                                        if not node.getName().startswith("phys_"):
                                                allApplied = False
                                                print("Error: node '%s' matched with config rule '%s' must be prefixed with 'phys_'" %
                                                        (node.getFullName(), section))
                                        used_sections[section] = True
                                        params = config.items(section)
                                        for name, value in params:
                                                node.fix_attribute(name, value)

                # Create engines.
                for section in config.sections():
                        if section.startswith("engine:"):
                                enginetype = stripQuotes(config.get(section, "type"))
                                pushengines = ["cam_flat_push"]
                                engineOk = enginetype in pushengines+["hinge_roll", "hinge_gyro", "hinge_break", "hinge_torque", "hinge2_turn", "rotor", "tilter"]
                                allApplied &= engineOk
                                if not engineOk:
                                        print("Error: invalid engine type '%s'." % enginetype)
                                node = self.onCreateNode("engine:"+enginetype, {"name":[section]})
                                engine_attribute = {}
                                params = config.items(section)
                                for name, value in params:
                                        node.fix_attribute(name, value)
                                def check_connected_to(l):
                                        ok = (len(l) >= 1)
                                        for e in l:
                                                ok &= (len(e) == 3)
                                                ok &= (type(e[0]) == str)
                                                ok &= (e[1] >= -100 and e[1] <= 100)
                                                ok &= (e[2] in ["normal", "half_lock"])
                                                connected_to = self._regexpnodes(e[0], group)
                                                ok &= (len(connected_to) > 0)
                                                for cn in connected_to:
                                                        ok &= cn.getName().startswith("phys_")
                                                        isValid, hasJoint = self._query_attribute(cn, "joint", lambda x: True, False)
                                                        if not hasJoint:
                                                                if enginetype not in pushengines:
                                                                        ok = False
                                                                        print("Error: %s is not jointed, but has an engine connected_to it!" % cn.getFullName())
                                        return ok
                                required = [("type", lambda x: type(x) == str),
                                            ("strength", lambda x: x > 0 and x < 30000),
                                            ("max_velocity", lambda x: len(x)==2 and x[0]>=-300 and x[0]<=300 and x[1]>=-300 and x[1]<=300),
                                            ("controller_index", lambda x: x >= 0 and x < 20),
                                            ("connected_to", check_connected_to)]
                                for name, engine_check in required:
                                        allApplied &= self._query_attribute(node, name, engine_check)[0]
                                friction = node.get_fixed_attribute("friction", optional=True, default=0.01)
                                node.fix_attribute("friction", friction)
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
                required = [("type", lambda x: chunkywriter.physics_type.get(x) != None)]
                for name, config_check in required:
                        allApplied &= config_check(self.config.get(name))

                for section in config.sections():
                        if not used_sections.get(section):
                                allApplied = False
                                print("Error: configuration secion '%s' not in use!" % section)
                return allApplied


        def validate_phys_group(self, group):
                isGroupValid = True
                # Check that joints and tweaks are correct.
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                node.shape = None
                                node.is_phys_root = False
                                #print("Before resolve...")
                                isGroupValid &= self._resolve_phys(node, group)
                                #print("After resolve...")
                                node.phys_root = node.phys_parent
                                # Check attributes.
                                def jointCheck(t):
                                        return (t == "suspend_hinge") or (t == "hinge2") or (t == "hinge")
                                isValid, hasJoint = self._query_attribute(node, "joint", jointCheck, False)
                                isGroupValid &= isValid
                                if not node.phys_root and isValid and hasJoint:
                                        print("Error: root node %s may not be jointed to anything else!" % node.getFullName())
                                        isGroupValid = False
                                isGroupValid &= self._query_attribute(node, "mass", lambda x: (x > 0 and x < 1000000))[0]
                                isGroupValid &= self._query_attribute(node, "bounce", lambda x: (x >= 0 and x <= 1))[0]
                                isGroupValid &= self._query_attribute(node, "friction", lambda x: (x >= 0 and x <= 100))[0]
                                isGroupValid &= self._query_attribute(node, "affected_by_gravity", lambda x: x==True or x==False)[0]
                # Check move all physics nodes to their respective physics root.
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                root = node
                                current_parent = node.phys_parent
                                del node.phys_parent
                                isValid, hasJoint = self._query_attribute(root, "joint", jointCheck, False)
                                while root and isValid and not hasJoint:
                                        if not hasattr(root, "phys_root"):
                                                isGroupValid = False
                                                print("Error: phys_root search for", node.getName(), "failed. Skipping root", root.getName())
                                                break
                                        root = root.phys_root
                                        if root and root.getName().startswith("phys_"):
                                                #print("Moving %s from parent %s (meshp %s) into root %s" %
                                                #      (node.getName(), current_parent.getName(), node.getParent().getName(), root.getName()))
                                                node.phys_root = root
                                                current_parent.phys_children.index(node)
                                                current_parent.phys_children.remove(node)
                                                try:
                                                        current_parent.phys_children.index(node)
                                                        print("Error: multiple instances of", node.getName(), "was children to", current_parent.getName())
                                                        isGroupValid = False
                                                except ValueError:
                                                        pass
                                                root.phys_children += [node]
                                                current_parent = root
                                                isValid, hasJoint = self._query_attribute(root, "joint", jointCheck, False)
                                if node.phys_root:
                                        node.phys_root.is_phys_root = True
                # Make it so that we only have one physical root.
                physroots = 0
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                if not node.phys_root:
                                        if node.getParent().phys_children.index(node) > 0:
                                                # Move to actual root.
                                                root = node.getParent().phys_children[0]
                                                root_xform = root.gettransformto(node.getParent(), "original", lambda n: n.getParent())
                                                own_xform = node.gettransformto(node.getParent(), "original", lambda n: n.getParent())
                                                node.localmat4 = root_xform.inverse() * own_xform
                                                node.phys_root = root
                                                node.xformparent = root
                                                root.phys_children += [node]
                                        else:
                                                physroots += 1
                if physroots != 1:
                        isGroupValid = False
                        print("Internal error: model has %i phys roots, must have exactly one!" % physroots)
                return isGroupValid


        def validate_shape_instances(self, group):
                isGroupValid = True
                # Connect phys transform with shape.
                for node in group:
                        if node.nodetype == "transform" and not hasattr(node, "shape"):
                                node.shape = None
                for node in group:
                        if node.nodetype == "mesh":
                                for parent in node.getparents():
                                        if parent.nodetype == "transform":
                                                if not parent.shape:
                                                        in_nodename = node.getInNode("i", "i")[0]
                                                        if in_nodename:
                                                                parent.shape = self.findNode(in_nodename)
                                                                if not parent.shape:
                                                                        print("Error: %s's input node %s does not exist!" % (node.getFullName(), in_nodename))
                                                                        isGroupValid = False
                                                        elif node.getName().startswith("phys_"):
                                                                print("Error: %s does not have an input node. Create node by instancing instead." % (node.getFullName()))
                                                                isGroupValid = False
                                                elif node.getName().startswith("phys_"):
                                                        print("Error: %s's parent %s seems to have >1 shape!" % (node.getName(), parent.getFullName()))
                                                        isGroupValid = False
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                if not node.shape:
                                        print("Error: %s has no primitive shape for physics (did you 'delete history or copy without instancing'?)!" % node.getFullName())
                                        isGroupValid = False
                return isGroupValid


        def validategroup(self, group):
                ok = True
                for mesh in group:
                        if mesh.getName().startswith("m_"):
                                mesh.phys_ref = []
                for phys in group:
                        if not phys.getName().startswith("phys_") or phys.nodetype != "transform":
                                continue
                        phys_name = phys.getName()[5:]
                        phys.mesh_ref = []
                        phys.loose_mesh_ref = []
                        phys_mesh_ref = phys.mesh_ref
                        if phys.phys_root and not phys.get_fixed_attribute("joint", optional=True):
                                phys_mesh_ref = phys.loose_mesh_ref
                        for mesh in group:
                                if not mesh.getName().startswith("m_"):
                                        continue
                                mesh_name = mesh.getName()[2:]
                                samesamebutdifferent = False
                                if mesh_name == phys_name:
                                        samesamebutdifferent = True
                                elif mesh_name.startswith(phys_name):
                                        s = mesh_name[len(phys_name):]
                                        if s.startswith("_"):
                                                try:
                                                        int(s[1:])
                                                        samesamebutdifferent = True
                                                except ValueError:
                                                        pass
                                if samesamebutdifferent:
                                        mesh.phys_ref += [phys]
                                        phys_mesh_ref += [mesh]
                # Check for erroneous references.
                for phys in group:
                        if not phys.getName().startswith("phys_") or phys.nodetype != "transform":
                                continue
                        for mesh in phys.mesh_ref:
                                if not self._is_valid_phys_ref(group, phys, mesh, False):
                                        ok = False
                        for mesh in phys.loose_mesh_ref:
                                if not self._is_valid_phys_ref(group, phys, mesh, True):
                                        ok = False
                for mesh in group:
                        if not mesh.getName().startswith("m_"):
                                continue
                        for phys in mesh.phys_ref:
                                if not self._is_valid_mesh_ref(mesh, phys):
                                        ok = False
                                        print("Error: phys node '%s' is illegally referencing mesh node '%s'." % \
                                              (phys.getFullName(), mesh.getFullName()))
                        if len(mesh.phys_ref) > 1:
                                ok = False
                                print("Error: mesh node '%s' is illegaly referenced by several physics nodes:" % mesh.getFullName())
                                for phys in mesh.phys_ref:
                                        print("Error:   - '%s'" % phys.getFullName())
                        elif not mesh.phys_ref:
                                if self._is_valid_phys_ref(group, None, mesh, False):
                                        if not mesh.phys_ref:
                                                ok = False
                                                print("Error: mesh %s has no suitable physics nodes to hook up to." % mesh.getName())
                                elif mesh.nodetype == "transform":
                                        p = mesh.getParent()
                                        if p and p.phys_ref:
                                                p.phys_ref[0].mesh_ref += [mesh]
                                                mesh.phys_ref += [p.phys_ref[0]]
                                        else:
                                                ok = False
                                                print("Error: mesh %s has invalid connections to phys nodes." % mesh.getName())
                return ok


        def makephysrelative(self, group):
                ok = True
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                ok &= self._physrelativemat4(node)
                return ok


        def _regexpnodes(self, regexp, group):
                found = []
                for node in group:
                        if re.search("^"+regexp+"$", node.getFullName()[1:]):
                                found += [node]
                return found


        def _physrelativemat4(self, node):
                ok = True
                if not node.getParent().getName().startswith("m_"):
                        ok = False
                        print("Error: %s must have a mesh parent (%s does not start with 'm_')." % (node.getFullName(), node.getParent().getName()))
                phroot = node.phys_root
                if phroot:
                        if not hasattr(phroot, "localmat4"):
                               ok = self._physrelativemat4(phroot)
                        own_branch_xform = node.gettransformto(None, "original", lambda n: n.getParent())
                        parent_branch_xform = phroot.gettransformto(None, "original", lambda n: n.getParent())
                        #print("Parent xform:")
                        #print(phroot.localmat4)
                        #print("To parent xform:")
                        #print(parent_branch_xform)
                        #print("Before trasform own xform:")
                        #print(own_branch_xform)
                        #print("Parent inverted xform:")
                        #print(parent_branch_xform.inverse())
                        #print(node.getName(), "is trying to work out localmat4 with xp = ", node.xformparent, "phroot =", phroot.getName(), "and phxp =", phroot.xformparent)
                        node.localmat4 = parent_branch_xform.inverse() * own_branch_xform
                        node.xformparent = phroot
                        #sys.exit(1)
                        
                else:
                        node.get_world_transform()
                return ok


        def _validate_non_recursive(self, basenode, group):
                isHierarchyValid = True
                childcount = {}
                childlist = self._listchildnodes(basenode.getFullName(), basenode, "", group, True)
                for node in childlist:
                        count = childcount.get(node)
                        if not count:
                                count = 0
                        count += 1
                        childcount[node] = count
                for key, count in childcount.items():
                        if key.nodetype == "transform" and count > 1:
                                print("Error: more than one path (currently %i paths) lead to node '%s'." % (count, key.getFullName()))
                                isHierarchyValid = False
                return isHierarchyValid


        def _validatenaming(self, rootnode, group):
                ok = True
                # Drop empty transforms that do nothing.
                kills = []
                for node in group:
                        if node.getName().startswith("transform") or node.getName().startswith("pasted__transform"):
                                if node.nodetype == "transform":
                                        if not node._setattr:
                                                kills += [node]
                for node in group:
                        if node.getParent() in kills:
                                node.setParent(node.getParent().getParent())
                for kill in kills:
                        group.remove(kill)
                # Traverse and check names.
                for node in group:
                        if node.getParent() == rootnode and node.nodetype == "transform":
                                node.kill_empty = False
                                if node.getName().startswith("phys_"):
                                        pass
                                elif node.getName().startswith("m_"):
                                        if node.getParent() != None and node.getParent().getName().startswith("phys_"):
                                                ok = False
                                                print("Error: mesh node '%s' may not have a 'phys_' parent (set to '%s')." %
                                                      (node.getFullName(), node.getParent().getName()))
                                else:
                                        ok = False
                                        print("Error: node '%s' must be either prefixed 'phys_' or 'm_'" % node.getFullName());
                                ok &= self._validatenaming(node, group)
                return ok


        def printnodes(self, nodes):
                def printnode(node, lvl=0):
                        if not node.isprinted and node.nodetype == "transform":
                                node.isprinted = True
                                if lvl == 0:
                                        print("----------")
                                print("  "*lvl + node.getFullName(), node.xformparent)
                                def printattr(attr):
                                        if hasattr(node, attr):
                                                for child in getattr(node, attr):
                                                        printnode(child, lvl+1)
                                printattr("mesh_ref")
                                printattr("phys_ref")
                for node in nodes:
                        for n in nodes:
                                n.isprinted = False
                        printnode(node)


        def _resolve_phys(self, phys, group):
                phys.phys_parent = None
                if not hasattr(phys, "phys_children"):
                        phys.phys_children = []
                        #print("Reset self=", phys)
                if phys.getParent() == group[0]:
                        return True
                parent = phys.getParent()
                if parent != None and parent.getName().startswith("phys_") and parent.nodetype == "transform":
                        phys.phys_parent = parent
                        if not hasattr(parent, "phys_children"):
                                parent.phys_children = [phys]
                        else:
                                parent.phys_children += [phys]
                        return True
                # Check children of parents, grandparents...
                mparent = parent
                parent = None
                while mparent != None:
                        #print("Looking for phys parent for %s in %s..." % (phys.getName(), mparent.getName()))
                        children = self._listchildnodes(mparent.getFullName(), mparent, "phys_", group, False)
                        phys_nodes = list(filter(lambda x: x.getName() != phys.getName(), children))
                        if phys_nodes:
                                parent = self.find_phys_root(mparent, phys_nodes)
                                if parent:
                                        #print("*********** Picked parent node", parent)
                                        break
                        mparent = mparent.getParent()
                #print("Phys %s has parent %s" % (phys.getFullName(), parent.getFullName()))
                if parent == None:
                        print("Error: phys node '%s' has no related parent phys node higher up in the hierarchy." % phys.getFullName())
                        return False
                phys.phys_parent = parent
                if not hasattr(parent, "phys_children"):
                        #print("Init parent", parent, "with self=", phys)
                        parent.phys_children = [phys]
                else:
                        #print("Adding self=", phys, "to parent.")
                        parent.phys_children += [phys]
                        #print(parent.phys_children)
                return True


        def find_phys_root(self, mesh, physlist):
                r = None
                for phys in physlist:
                        if phys.getName()[5:] == mesh.getName()[2:]:
                                if r:
                                        print("Error: mesh %s holds more than one phys root!" % node.getFullName())
                                        sys.exit(3)
                                r = phys
##                if not r and physlist:
##                        print("Error: could not find phys parent to mesh %s!" % mesh.getName())
##                        sys.exit(3)
                #print("Found phys root", r, "for", mesh)
                return r


        def _is_valid_phys_ref(self, group, phys, mesh, loose):
                if mesh.nodetype != "transform":
                        return False
                invalid_child_list = ["polyCube", "polySphere", "polyCylinder", "polyTorus", "polySmoothFace", "polyTweak", \
                                      "polyMergeVert", "groupId", "groupParts", "polyUnite", "script", "camera"]
                meshcnt = 0
                invcnt = 0
                if mesh.shape:
                        meshcnt += 1
                children = self._listchildnodes(mesh.getFullName(), mesh, "m_", group, False)
                #print("Mesh %s has kids:" % mesh.getName())
                for child in children:
                        #print(" - Child %s.", child.getName())
                        if child.get_fixed_attribute("rgvtx", optional=True):
                                meshcnt += 1
                        if child in invalid_child_list:
                                invcnt += 1

                while not phys:
                        p = mesh.getParent()
                        while p:
                                if p.phys_ref:
                                        phys = self.find_phys_root(p, p.phys_ref)
                                        if phys:
                                                #print("Mesh %s hooking onto phys %s." % (mesh.getName(), phys.getName()))
                                                mesh.phys_ref += [phys]
                                                phys.mesh_ref += [mesh]
                                                break
                                p = p.getParent()

                hangaround_child = False
                if mesh.getParent() and phys in mesh.getParent().phys_ref:
                        hangaround_child = True
                if phys:
                        #print("Checking phys validity on", phys.getName(), ", ", mesh.getName(), ", mwp:", mesh.get_world_pivot())
                        valid_ref = True
##                        if meshcnt > 1:
##                                valid_ref = False
##                                print("Error: mesh '%s' (referenced by '%s') contains %i mesh shapes (only one allowed)." %
##                                      (mesh.getFullName(), phys.getFullName(), meshcnt))
                        if not loose and meshcnt == 0 and not hangaround_child:
                                valid_ref = False
                                print("Error: mesh '%s' (referenced by '%s') contains no child mesh shapes." %
                                      (mesh.getFullName(), phys.getFullName()))
                        if invcnt:
                                valid_ref = False
                                print("Error: mesh '%s' (referenced by '%s') contains %i invalid child nodes." %
                                      (mesh.getFullName(), phys.getFullName(), invcnt))
##                        if not phys.phys_root and mesh.getParent():
##                                valid_ref = False
##                                print("Error: root mesh node '%s' (referenced by '%s') erroneously has parent." %
##                                      (mesh.getFullName(), phys.getFullName()))
                        if not loose and phys.phys_root and not phys.get_fixed_attribute("joint", optional=True):
                                valid_ref = False
                                print("Error: mesh '%s' referenced by non-jointed phys node '%s' (root %s)." %
                                      (mesh.getFullName(), phys.getFullName(), phys.phys_root.getName()))
                        if loose and (not phys.phys_root or phys.get_fixed_attribute("joint", optional=True)):
                                valid_ref = False
                                print("Error: attached mesh '%s' referenced by jointed phys node '%s'." %
                                      (mesh.getFullName(), phys.getFullName()))
                        mt = mesh.get_world_pivot()
                        pt = phys.get_world_pivot()
                        d = vec3((mt-pt)[:3])
                        eps = 0.0001
                        if d.length() >= eps:
                                valid_ref = False
                                print("Error: mesh '%s' and phys node '%s' do not share the exact same World Space Pivot (%s and %s). " \
                                      "Note that Y and Z axes are transformed compared to Maya." %
                                      (mesh.getFullName(), phys.getFullName(), str(mt), str(pt)))
                else:
                        valid_ref = False
                        print("Error: no phys...")
                        if meshcnt > 0:
                                print("Error: mesh '%s' contains mesh shape, but is not linked to phys node. loose=%s" %
                                      (mesh.getFullName(), loose))
                                sys.exit(3)
                return valid_ref


        def _is_valid_mesh_ref(self, mesh, phys):
                valid_ref = True
                if phys.nodetype != "transform":
                        valid_ref = False
                        print("Error: phys node '%s' may not reference any mesh node (accidental to mesh '%s'?)." %
                                (phys.getFullName(), mesh.getFullName()))
                return valid_ref


        def _listchildnodes(self, path, basenode, prefix, group, recursive, f=None):
                childlist = []
                for node in group:
                        if node.getName().startswith(prefix) and basenode in node.getparents():
                                if not f or f(node):
                                        childlist += [node]
                grandchildlist = []
                if recursive:
                        for child in childlist:
                                childpath = path+"|"+child.getName()
                                grandchildlist += self._listchildnodes(childpath, child, prefix, group, True, f)
                return childlist+grandchildlist


        def _query_attribute(self, node, attrname, check, err_missing=True):
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


        def _insert_in_same_island_as(self, islands, same, node, allow_disconnected):
                if self._isInIslands(islands, node):
                        print("Error: trying to re-insert already inserted node '%s'!" % node.getFullName())
                        return False
                for island in islands:
                        for n in island:
                                if n == same:
                                        island.append(node)
                                        return True
                if not allow_disconnected:
                        print("Error: parent/associated node '%s' (related to '%s') does not exist in islands!" % (same.getFullName(), node.getFullName()))
                return False


        def _isInIslands(self, islands, node):
                for island in islands:
                        for n in island:
                                if n == node:
                                        return True
                return False


        def _getnode(self, nodename, group):
                for node in group:
                        if node.getName() == nodename:
                                return node
                return None


def printfeats(feats):
        feats_items = sorted(feats.items(), key=lambda x:x[1])
        for k,v in feats_items:
                singular, plural = k.split(":")
                v = feats[k]
                if v == 1:
                        name = singular
                else:
                        name = plural
                print("Wrote %6i %s." % (v, name))


def _maimport(filename):
        filebasename = os.path.splitext(filename)[0]
        rd = GroupReader(filebasename)
        rd.doread()

        #print()
        #print("This is what the physics look like:")
        def p_p(n, indent=0):
                print("  "*indent + n.getName())
                for child in n.phys_children:
                        p_p(child, indent+1)
        #[p_p(root) for root in filter(lambda n: n.getName()=="phys_body", rd.group)]
        #print()

        pwr = chunkywriter.PhysWriter(filebasename, rd.group, rd.config)
        mwr = chunkywriter.MeshWriter(filebasename, rd.group, rd.config)
        cwr = chunkywriter.ClassWriter(filebasename, rd.group, rd.config)
        pwr.write()
        mwr.write()
        cwr.write()
        if options.options.verbose:
                feats = {}
                pwr.addfeats(feats)
                mwr.addfeats(feats)
                cwr.addfeats(feats)
                printfeats(feats)
        print("%s: import ok." % filebasename)


def main():
        usage = "usage: %prog [options] <filespec>\n" + \
                "Reads filespec.ma and filespec.ini and writes some output chunky files."
        parser = optparse.OptionParser(usage=usage, version="%prog 0.1")
        parser.add_option("-v", "--verbose", action="store_true", dest="verbose", default=False, help="make lots of noise")
        options.options, options.args = parser.parse_args()

        if len(options.args) < 1:
                parser.error("no filebasename supplied")

        import glob
        for arg in options.args:
                files = glob.glob(arg)
                if not files:
                        _maimport(arg)
                for fn in files:
                        _maimport(fn)


if __name__=="__main__":
        main()
