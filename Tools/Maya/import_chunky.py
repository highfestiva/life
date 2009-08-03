# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-07-17 for Righteous Engine tool chain.


from mat4 import *
from mayaascii import *
from quat import *
from vec3 import *
from vec4 import *
import chunkywriter

from functools import reduce
import configparser
import datetime
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
                                  "renderLayerManager", "renderLayer", "script", "<unknown>", "phong", \
                                  "shadingEngine", "materialInfo", "groupId", "groupParts" ]
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
                islands = self.filterIslands(islands)
                if len(islands) != 2:
                        print("The number of standalone groups is not two (is totally %i) in the .ma file! These are the groups:" % len(islands))
                        for i in islands:
                                print("  "+str(list(map(lambda x: x.getFullName(), i))))
                                print("")
                        print("Terminating due to error.")
                        sys.exit(2)
                phys_group, mesh_group = islands
                if not phys_group[0].getName().startswith("phys"):
                        phys_group, mesh_group = mesh_group, phys_group
                if not phys_group[0].getName().startswith("phys_"):
                        print("Error: physics root must be named 'phys_...' something, not '%s'." % phys_group[0].getName())
                        sys.exit(2)
                if not mesh_group[0].getName().startswith("m_"):
                        print("Error: gfx mesh root must be named 'm_...' something, not '%s'." % mesh_group[0].getName())
                        sys.exit(2)
                config = configparser.SafeConfigParser()
                ininame = self.basename+".ini"
                if not config.read(ininame):
                        print("Error: could not open tweak file '%s'!" % ininame)
                        sys.exit(3)
                self.rotatexaxis(phys_group)
                self.rotatexaxis(mesh_group)

                for n in phys_group+mesh_group:
                        if n.getFullName().find("Shape") < 0 and n.getFullName().find("poly") < 0:
                                print("%-20s  %s  %s." % (n.getFullName(), n.get_world_translation(), n.get_world_pivot()))
                print("----------------")

                self.faces2triangles(mesh_group)
                self.makevertsrelative(mesh_group)
                if not self.applyPhysConfig(config, phys_group):
                        print("Error: could not apply configuration.")
                        sys.exit(4)
                if not self.validatePhysGroup(phys_group):
                        #self.printnodes(phys_group)
                        print("Invalid physics group! Terminating due to error.")
                        sys.exit(3)
                if not self.validateMeshGroup(mesh_group):
                        print("Invalid mesh group! Terminating due to error.")
                        sys.exit(3)
                self.phys_group = phys_group
                self.mesh_group = mesh_group
                if not self.validategroup():
                        print("Invalid group! Terminating due to error.")
                        sys.exit(3)


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
                                value = self.getAttrValue(name, name, None, n=None, default=default)
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
                def get_world_pivot(self):
                        o = self.get_local_pivot() - self.get_fixed_attribute("spt", optional=True, default=vec3(0,0,0))
                        return self.get_world_translation(vec4(o[0],o[1],o[2], 1))
                def get_world_translation(self, origin=vec4(0, 0, 0, 1)):
                        m = self.get_world_transform()
                        return m * origin
                def get_world_transform(self):
                        if self.getParent():
                                pm = self.getParent().get_world_transform()
                        else:
                                pm = mat4.identity()
                        mr = self.get_local_quat().toMat4()
                        t = self.get_local_translation()
                        s = self.get_fixed_attribute("s", optional=True, default=vec3(1,1,1))
                        sh = self.get_fixed_attribute("sh", optional=True, default=vec3(0,0,0))
                        rp = self.get_local_pivot()
                        rt = self.get_fixed_attribute("rpt", optional=True, default=vec3(0,0,0))
                        sp = self.get_fixed_attribute("sp", optional=True, default=vec3(0,0,0))
                        st = self.get_fixed_attribute("spt", optional=True, default=vec3(0,0,0))
                        msp = mat4.translation(-sp)
                        ms = mat4.scaling(s)
                        msh = mat4.identity()
                        msh.mlist[2] = +sh[0]	# X: x(+z), originally x(+y) in Maya.
                        msh.mlist[1] = -sh[1]	# Y: x(-y), originally x(z) in Maya.
                        msh.mlist[9] = -sh[2]	# Z: z(-y), originally y(z) in Maya.
                        mspi = mat4.translation(sp)
                        mst = mat4.translation(st)
                        mrp = mat4.translation(-rp)
                        mar = self.get_local_arq().toMat4()
                        mrpi = mat4.translation(rp)
                        mrt = mat4.translation(rt)
                        mt = mat4.translation(t)
                        # According to Maya doc (as I understood it): [sp][s][sh][sp^-1][st][rp][ar][r][rp^-1][rt][t].
                        # My multiplications are reversed order, since matrices already transposed.
                        m = pm * mt * mrt * mrpi * mr * mar * mrp * mst * mspi * msh * ms * msp
                        return m
                def get_world_scale(self):
                        node = self
                        s = (1,1,1)
                        while node:
                                scale = node.get_fixed_attribute("s", default=vec3(1,1,1))
                                if len(scale) != 3 or reduce(lambda x,y: x*y, scale) == 0:
                                        print("Error: wrong number of scale scalars, or some scale is zero for node '%s'." % node.getFullName())
                                        sys.exit(19)
                                s = tuple(map(lambda x, y: x*y, s, scale))
                                node = node.getParent()
                        return s
                def get_local_translation(self):
                        return self.get_fixed_attribute("t", default=vec3(0,0,0))
                def get_local_pivot(self):
                        return self.get_fixed_attribute("rp", default=vec3(0,0,0))
                def get_local_quat(self):
                        rot = self.get_fixed_attribute("r", default=vec3(0,0,0))
                        qx = quat().fromAngleAxis(rot[0], (1, 0, 0))
                        qy = quat().fromAngleAxis(rot[1], (0, 1, 0))
                        qz = quat().fromAngleAxis(rot[2], (0, 0, 1))
                        q = qy*qz*qx
                        return q
                def get_local_arq(self):
                        ra = self.get_fixed_attribute("ra", default=vec3(0,0,0))
                        qx = quat().fromAngleAxis(ra[0], (1, 0, 0))
                        qy = quat().fromAngleAxis(ra[1], (0, 1, 0))
                        qz = quat().fromAngleAxis(ra[2], (0, 0, 1))
                        q = qy*qz*qx
                        return q
                node.fix_attribute = types.MethodType(fix_attribute, node)
                node.get_fixed_attribute = types.MethodType(get_fixed_attribute, node)
                node.get_inherited_attr = types.MethodType(get_inherited_attr, node)
                node.get_fixed_tab = types.MethodType(get_fixed_tab, node)
                node.get_world_translation = types.MethodType(get_world_translation, node)
                node.get_world_pivot = types.MethodType(get_world_pivot, node)
                node.get_world_transform = types.MethodType(get_world_transform, node)
                node.get_world_scale = types.MethodType(get_world_scale, node)
                node.get_local_translation = types.MethodType(get_local_translation, node)
                node.get_local_pivot = types.MethodType(get_local_pivot, node)
                node.get_local_quat = types.MethodType(get_local_quat, node)
                node.get_local_arq = types.MethodType(get_local_arq, node)
                return node


        def gatherIslands(self, nodes):
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
                                        if not self._insertInSameIslandAs(islands, parentNode, node, False):
                                                return None
                                        nodes.remove(node)
                                outnodes = node.getOutNodes("out", "out")
                                if len(outnodes) == 1:
                                        if self._insertInSameIslandAs(islands, outnodes[0][0], node, allow_no_association):
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
                print("Error: you need to manually do an internal export of meshes.")
                sys.exit(19)


        def filterIslands(self, islands):
                for island in islands:
                        for n in island:
                                if n.nodetype in self.bad_types or \
                                        (island.index(n) == 0 and n.getName().startswith("ignore_")):
                                        #print("Removing %s." % n.nodetype)
                                        islands.remove(island)
                                        self.filterIslands(islands)
                                        break
                return islands


        def rotatexaxis(self, group):
                for node in group:
                        if node.nodetype == "transform":
                                def fix(node, aname, default, conv=None, flip=lambda x,y,z: (x,-z,y)):
                                        val = node.getAttrValue(aname, aname, None, default=default)
                                        if val:
                                                if flip:
                                                        val = flip(*val)
                                                if conv:
                                                        val = tuple(map(conv, val))
                                                val = vec3(val)
                                                node.fix_attribute(aname, val)
                                                return val
                                fix(node, "t", (0,0,0))
                                fix(node, "rp", (0,0,0))
                                fix(node, "rpt", (0,0,0))
                                fix(node, "sp", (0,0,0))
                                fix(node, "spt", (0,0,0))
                                fix(node, "r", (0,0,0), math.radians)
                                fix(node, "s", (1,1,1), math.fabs)
                                fix(node, "sh", (0,0,0), None, None)
                                fix(node, "ra", (0,0,0), math.radians)
                        vtx = node.getAttrValue("rgvtx", "rgvtx", None, n=None)
                        if vtx:
                                for x in range(0, len(vtx), 3):
                                        tmp = vtx[x+1]
                                        vtx[x+1] = -vtx[x+2]
                                        vtx[x+2] = tmp
                                node.fix_attribute("rgvtx", vtx)


        def faces2triangles(self, group):
                for node in group:
                        faces = node.get_fixed_attribute("rgf", optional=True)
                        if faces:
                                triangles = []
                                if not type(faces) == str:
                                        faces = faces[1]        # From ("(", "...", ")") to "..."
                                faces = eval(faces[1:-1])
                                for face in faces:
                                        if len(face) == 4:
                                                l = [face[0], face[1], face[2], face[0], face[2], face[3]]
                                                #print(l)
                                                triangles += l
                                        elif len(face) == 3:
                                                #print(face)
                                                triangles += face
                                                pass
                                        else:
                                                print("Error: cannot yet handle faces with less than three or more than four vertices!")
                                                sys.exit(19)
                                node.fix_attribute("rgtri", triangles)


        def makevertsrelative(self, group):
                for node in group:
                        vtx = node.get_fixed_attribute("rgvtx", optional=True)
                        if vtx:
                                pos = node.getParent().get_world_translation()
                                idx = 0
                                for idx in range(len(vtx)):
                                        vtx[idx] -= pos[idx%3]


        def applyPhysConfig(self, config, group):
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
                required = [("type", lambda x: chunkywriter.physics_type.get(x) != None)]
                for name, config_check in required:
                        allApplied &= config_check(self.config.get(name))

                for section in config.sections():
                        if not used_sections.get(section):
                                print("Error: configuration secion '%s' not in use!" % section)
                                allApplied = False
                return allApplied


        def validatePhysGroup(self, group):
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
                                        isGroupValid &= self.validate_non_recursive(node, group)
                                transformIsRoot = False
                return isGroupValid


        def validateMeshGroup(self, group):
                isGroupValid = True
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
                                if len(vtx) % 3 != 0:
                                        print("Error: vertex count in '%s' is not a multiple of three!" % node.getFullName())
                                        isGroupValid = False
                                usedlastvertex = False
                                for p in polys:
                                        if p == len(vtx)/3-1:
                                                usedlastvertex = True
                                        if p >= len(vtx)/3:
                                                print("Error: polygon %i in '%s' is out of vertex range." % (polys.index(p), node.getFullName()))
                                                isGroupValid = False
                                if not usedlastvertex:
                                        print("Error: not all vertices in '%s' is used." % node.getFullName())
                                        isGroupValid = False
                return isGroupValid


        def validategroup(self):
                ok = True
                for mesh in self.mesh_group:
                        mesh.phys_ref = []
                for phys in self.phys_group:
                        if phys.nodetype != "transform":
                                continue
                        phys.mesh_ref = []
                        phys.loose_mesh_ref = []
                        phys_mesh_ref = phys.mesh_ref
                        if phys.getParent() and not phys.get_fixed_attribute("joint", optional=True):
                                phys_mesh_ref = phys.loose_mesh_ref
                        phys_name = phys.getName()
                        if phys_name.startswith("phys_"):
                                phys_name = phys_name[5:]
                        for mesh in self.mesh_group:
                                mesh_name = mesh.getName()
                                if mesh_name.startswith("m_"):
                                        mesh_name = mesh_name[2:]
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
                for phys in self.phys_group:
                        if phys.nodetype != "transform":
                                continue
                        for mesh in phys.mesh_ref:
                                if not self._is_valid_phys_ref(phys, mesh, False):
                                        ok = False
                                        print("Error: mesh node '%s' referenced by jointed phys node '%s' is not a valid reference (due to above error(s))." % \
                                              (mesh.getFullName(), phys.getFullName()))
                        if phys.getParent() == None or phys.get_fixed_attribute("joint", optional=True):
                                if not phys.mesh_ref:
                                        ok = False
                                        print("Error: jointed phys node '%s' does not seem to have corresponding mesh node(s)." % phys.getFullName())
                        for mesh in phys.loose_mesh_ref:
                                if not self._is_valid_phys_ref(phys, mesh, True):
                                        ok = False
                                        print("Error: mesh node '%s' referenced by non-jointed phys node '%s' is not a valid reference (due to above error(s))." % \
                                              (mesh.getFullName(), phys.getFullName()))
                        if phys.getParent() and not phys.get_fixed_attribute("joint", optional=True):
                                if not phys.loose_mesh_ref:
                                        ok = False
                                        print("Error: non-jointed phys node '%s' does not seem to have corresponding mesh node8s)." % phys.getFullName())
                for mesh in self.mesh_group:
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
                                if self._is_valid_phys_ref(None, mesh, False):
                                        ok = False
                                        print("Error: mesh node '%s' must be referenced by a phys node, but is not!" % mesh.getFullName())
                return ok
                        


        def validate_non_recursive(self, basenode, group):
                isHierarchyValid = True
                childcount = {}
                childlist = self._listchildnodes(basenode.getFullName(), basenode, group, True)
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


        def _is_valid_phys_ref(self, phys, mesh, loose):
                if mesh.nodetype != "transform":
                        return False
                invalid_child_list = ["polyCube", "polySphere", "polyCylinder", "polyTorus", "polySmoothFace", "polyTweak", \
                                      "polyMergeVert", "groupId", "groupParts", "polyUnite", "script", "camera"]
                meshcnt = 0
                invcnt = 0
                children = self._listchildnodes(mesh.getFullName(), mesh, self.mesh_group, False)
                for child in children:
                        if child.get_fixed_attribute("rgvtx", optional=True):
                                meshcnt += 1
                        if child in invalid_child_list:
                                invcnt += 1
                hangaround_child = False
                if mesh.getParent() and phys in mesh.getParent().phys_ref:
                        hangaround_child = True
                if phys:
                        valid_ref = True
                        if meshcnt > 1:
                                valid_ref = False
                                print("Error: mesh '%s' (referenced by '%s') contains %i mesh shapes (only one allowed)." %
                                      (mesh.getFullName(), phys.getFullName(), meshcnt))
                        elif not loose and meshcnt == 0 and not hangaround_child:
                                valid_ref = False
                                print("Error: mesh '%s' (referenced by '%s') contains no child mesh shapes." %
                                      (mesh.getFullName(), phys.getFullName()))
                        if invcnt:
                                valid_ref = False
                                print("Error: mesh '%s' (referenced by '%s') contains %i invalid child nodes." %
                                      (mesh.getFullName(), phys.getFullName(), invcnt))
                        if not phys.getParent() and mesh.getParent():
                                valid_ref = False
                                print("Error: root mesh node '%s' (referenced by '%s') erroneously has parent." %
                                      (mesh.getFullName(), phys.getFullName()))
                        if not loose and phys.getParent() and not phys.get_fixed_attribute("joint", optional=True):
                                valid_ref = False
                                print("Error: mesh '%s' referenced by non-jointed phys node '%s'." %
                                      (mesh.getFullName(), phys.getFullName()))
                        if loose and (not phys.getParent() or phys.get_fixed_attribute("joint", optional=True)):
                                valid_ref = False
                                print("Error: attached mesh '%s' referenced by jointed phys node '%s'." %
                                      (mesh.getFullName(), phys.getFullName()))
                        jointed_phys = phys
                        while jointed_phys.getParent() and not jointed_phys.get_fixed_attribute("joint", optional=True):
                                jointed_phys = jointed_phys.getParent()
                        mt = mesh.get_world_pivot()
                        pt = jointed_phys.get_world_pivot()
                        eps = 0.0001
                        if math.fabs(mt[0]-pt[0]) >= eps or \
                                math.fabs(mt[1]-pt[1]) >= eps or \
                                math.fabs(mt[2]-pt[2]) >= eps:
                                valid_ref = False
                                print("Error: mesh '%s' and phys node '%s' do not share the exact same World Space Pivot (%s and %s). " \
                                      "Note that Y and Z axes are transformed compared to Maya." %
                                      (mesh.getFullName(), jointed_phys.getFullName(), str(mt), str(pt)))
                else:
                        valid_ref = False
                        if meshcnt > 0:
                                valid_ref = True
                                print("Error: mesh '%s' contains mesh shape, but is not linked to." % mesh.getFullName())
                return valid_ref


        def _is_valid_mesh_ref(self, mesh, phys):
                valid_ref = True
                if phys.nodetype != "transform":
                        valid_ref = False
                        print("Error: phys node '%s' may not reference any mesh node (accidental to mesh '%s'?)." %
                                (phys.getFullName(), mesh.getFullName()))
                return valid_ref


        def _listchildnodes(self, path, basenode, group, recursive):
                childlist = []
                for node in group:
                        if node.getParent() == basenode:
                              childlist += [node]
                #if childlist:
                #        print("Children for %s:" % path)
                #        for child in childlist:
                #                print("  - %s" % child.getName())
                grandchildlist = []
                if recursive:
                        for child in childlist:
                                childpath = path+"|"+child.getName()
                                grandchildlist += self._listchildnodes(childpath, child, group, True)
                return childlist+grandchildlist


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


        def _insertInSameIslandAs(self, islands, same, node, allow_disconnected):
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


def main():
        if len(sys.argv) != 2:
                print("Usage: %s <basename>")
                print("Reads basename.ma and basename.ini and writes some output chunky files.")
                sys.exit(20)
        rd = GroupReader(sys.argv[1])
        rd.doread()
        gwr = chunkywriter.GroupWriter(sys.argv[1], rd.phys_group, rd.mesh_group, rd.config)
        gwr.dowrite()
        pmwr = chunkywriter.PhysMeshWriter(sys.argv[1], rd.phys_group, rd.mesh_group, rd.config)
        pmwr.dowrite()
        #rd.printnodes(rd.phys_group, False)
        #rd.printnodes(rd.mesh_group, False)


if __name__=="__main__":
        main()
