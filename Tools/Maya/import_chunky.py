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
                if len(islands) != 1:
                        print("The number of standalone groups is not one (is totally %i) in the .ma file! These are the groups:" % len(islands))
                        for i in islands:
                                print("  "+str(list(map(lambda x: x.getFullName(), i))))
                                print("")
                        print("Terminating due to error.")
                        sys.exit(2)
                group = islands[0]
                if not group[0].getName().startswith("m_"):
                        print("Error: root must be a mesh named 'm_...' something, not '%s'." % group[0].getName())
                        sys.exit(2)
                config = configparser.SafeConfigParser()
                ininame = self.basename+".ini"
                if not config.read(ininame):
                        print("Error: could not open tweak file '%s'!" % ininame)
                        sys.exit(3)
                self.rotatexaxis(group)

                self.faces2triangles(group)
                self.makevertsrelative(group)
                if not self.validatehierarchy(group):
                        print("Invalid hierarchy! Terminating due to error.")
                        sys.exit(3)
                if not self.validate_orthogonality(group):
                        print("Group not completely orthogonal. Terminating due to error.")
                        sys.exit(3)
                if not self.validateMeshGroup(group):
                        print("Invalid mesh group! Terminating due to error.")
                        sys.exit(3)
                if not self.applyPhysConfig(config, group):
                        print("Error: could not apply configuration.")
                        sys.exit(4)
                if not self.validatePhysGroup(group):
                        print("Invalid physics group! Terminating due to error.")
                        sys.exit(3)
                if not self.validategroup(group):
                        print("Invalid group! Terminating due to error.")
                        sys.exit(3)
                if not self.makephysrelative(group):
                        print("Internal vector math failed! Terminating due to error.")
                        sys.exit(3)
                self.group = group


        # Override
        def createNode(self, nodetype, opts):
                """Overridden to create fixed attributes, used for being able to override
                   configuration settings separate to the Maya ASCII .ma file."""
                node = super(GroupReader, self).createNode(nodetype, opts)
                node._fixattr = {}
                node.xformparent = node.getParent()
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
                        o = self.get_local_pivot()
                        return self.get_world_translation(vec4(o[0],o[1],o[2], 1))
                def get_world_translation(self, origin=vec4(0, 0, 0, 1)):
                        m = self.get_world_transform()
                        return m * origin
                def get_world_transform(self):
                        return self.gettransformto(None)
                def get_local_transform(self):
                        return self.gettransformto(self.xformparent)
                def gettransformto(self, toparent, matname="localmat4", getparent=lambda n: n.xformparent):
                        if self == toparent:
                                return mat4.identity()
                        parent = getparent(self)
                        if parent and (toparent == None or parent != toparent):
                                pm = parent.gettransformto(toparent, matname, getparent)
                                if not pm:
                                        return None
                        else:
                                pm = mat4.identity()
                                if toparent and parent == None:
                                        print("Warning: could not transform to parent '%s'" % toparent.getFullName())
                                        return None
                        if hasattr(self, matname):
                                return pm * getattr(self, matname)
                        mr = self.get_local_quat().toMat4()
                        t = self.get_local_translation()
                        s = self.get_local_scale()
                        sh = self.get_fixed_attribute("sh", optional=True, default=vec3(0,0,0))
                        rp = self.get_local_rpivot()
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
                        m = mt * mrt * mrpi * mr * mar * mrp * mst * mspi * msh * ms * msp
                        setattr(self, matname, m)
                        return pm * getattr(self, matname)
                def get_local_scale(self):
                        return self.get_fixed_attribute("s", default=vec3(1,1,1))
                def get_local_translation(self):
                        return self.get_fixed_attribute("t", default=vec3(0,0,0))
                def get_local_rpivot(self):
                        return self.get_fixed_attribute("rp", default=vec3(0,0,0))
                def get_local_pivot(self):
                        return self.get_local_rpivot() - self.get_fixed_attribute("spt", optional=True, default=vec3(0,0,0))
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
                def get_world_quat(self):
                        if self.xformparent:
                                pq = self.xformparent.get_world_quat()
                        else:
                                pq = quat()
                        return pq * self.get_local_quat() * self.get_local_arq()
                def isortho(self):
                        l = [vec4(1,0,0,1), vec4(0,1,0,1), vec4(0,0,1,1)]
                        p = vec3(self.get_world_translation()[0:3])
                        t = self.get_world_transform()
                        for i in range(3):
                                l[i] = t * l[i]
                        for i in range(3):
                                for j in range(3):
                                        if i != j:
                                                k = ((i+1)^(j+1))-1
                                                u = vec3(l[i][0:3])-p
                                                v = vec3(l[j][0:3])-p
                                                w = vec3(l[k][0:3])-p
                                                s = u.cross(v)
                                                a = abs((s * w) / (s.length()*w.length()))
                                                if a <= 0.99:
                                                        print(i,j,k,u,v,w,s)
                                                        print("cos(a) of %s is %f." % (self.getFullName(), a))
                                                        return False
                        return True
                node.fix_attribute = types.MethodType(fix_attribute, node)
                node.get_fixed_attribute = types.MethodType(get_fixed_attribute, node)
                node.get_inherited_attr = types.MethodType(get_inherited_attr, node)
                node.get_fixed_tab = types.MethodType(get_fixed_tab, node)
                node.get_world_translation = types.MethodType(get_world_translation, node)
                node.get_world_pivot = types.MethodType(get_world_pivot, node)
                node.get_world_transform = types.MethodType(get_world_transform, node)
                node.get_local_transform = types.MethodType(get_local_transform, node)
                node.gettransformto = types.MethodType(gettransformto, node)
                node.get_local_scale = types.MethodType(get_local_scale, node)
                node.get_local_translation = types.MethodType(get_local_translation, node)
                node.get_local_rpivot = types.MethodType(get_local_rpivot, node)
                node.get_local_pivot = types.MethodType(get_local_pivot, node)
                node.get_local_quat = types.MethodType(get_local_quat, node)
                node.get_local_arq = types.MethodType(get_local_arq, node)
                node.get_world_quat = types.MethodType(get_world_quat, node)
                node.isortho = types.MethodType(isortho, node)
                return node


        def _recursiveremove(self, node, nodes):
                nodes.remove(node)
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
                                # Get transformation to origo without rescaling.
                                transform = mat4.translation(-vec3(node.get_world_translation()[0:3]))
                                transform = (transform * node.get_world_quat().toMat4()).inverse()
                                vp = vec4(0,0,0,1);
                                idx = 0
                                for idx in range(len(vtx)):
                                        cidx = idx%3
                                        vp[cidx] = vtx[idx]
                                        if cidx == 2:
                                                vp = transform*vp
                                                vtx[idx-2] = vp.x
                                                vtx[idx-1] = vp.y
                                                vtx[idx-0] = vp.z


        def validate_orthogonality(self, group):
                isGroupValid = True
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                if not node.isortho():
                                        isGroupValid = False
                                        print("Error: phys node '%s' must be orthogonal." % node.getFullName())
                return isGroupValid


        def validatehierarchy(self, group):
                isGroupValid = self._validate_non_recursive(group[0], group)
                isGroupValid &= self._validatenaming(group[0], group)
                return isGroupValid


        def validateMeshGroup(self, group):
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
                        if node.getName().startswith("m_") and node.nodetype == "transform":
                                node.mesh_children = list(filter(lambda x: x.nodetype == "transform", self._listchildnodes(node.getFullName(), node, "m_", group, False)))
                                node.phys_children = self._listchildnodes(node.getFullName(), node, "phys_", group, False)
                return isGroupValid

                                
        def applyPhysConfig(self, config, group):
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
                                engineOk = enginetype in ["hinge2_roll", "hinge2_turn", "hinge2_break", "hinge"]
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
                                        return ok
                                required = [("type", lambda x: type(x) == str),
                                            ("strength", lambda x: x > 0 and x < 30000),
                                            ("max_velocity", lambda x: len(x)==2 and x[0]>=0 and x[0]<=300 and x[1]>=0 and x[1]<=300),
                                            ("controller_index", lambda x: x >= 0 and x < 20),
                                            ("connected_to", check_connected_to)]
                                for name, engine_check in required:
                                        allApplied &= self._queryAttribute(node, name, engine_check)[0]
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


        def validatePhysGroup(self, group):
                isGroupValid = True
                for node in group:
                        if node.getName().startswith("phys_") and node.nodetype == "transform":
                                isGroupValid &= self._resolve_phys(node, group)
                                # Check attributes.
                                def jointCheck(t):
                                        return (t == "suspend_hinge") or (t == "hinge2") or (t == "hinge")
                                isValid, hasJoint = self._queryAttribute(node, "joint", jointCheck, False)
                                isGroupValid &= isValid
                                if not node.phys_parent and isValid and hasJoint:
                                        print("Error: root node %s may not be jointed to anything else!" % node.getFullName())
                                        isGroupValid = False
                                isGroupValid &= self._queryAttribute(node, "mass", lambda x: (x > 0 and x < 1000000))[0]
                                isGroupValid &= self._queryAttribute(node, "bounce", lambda x: (x >= 0 and x <= 1))[0]
                                isGroupValid &= self._queryAttribute(node, "friction", lambda x: (x >= 0 and x <= 100))[0]
                                isGroupValid &= self._queryAttribute(node, "affected_by_gravity", lambda x: x==True or x==False)[0]
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
                        if phys.phys_parent and not phys.get_fixed_attribute("joint", optional=True):
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
                                        ok = False
                                        print("Error: mesh node '%s' must be referenced by a phys node, but is not!" % mesh.getFullName())
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
                phpar = node.phys_parent
                if phpar:
                        if not hasattr(phpar, "localmat4"):
                               ok = self._physrelativemat4(phpar)
                        own_branch_xform = node.gettransformto(phpar.xformparent)
                        parent_branch_invxform = phpar.gettransformto(phpar.xformparent).inverse()
                        node.localmat4 = parent_branch_invxform * own_branch_xform
                        node.xformparent = phpar
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
                        if count > 1:
                                print("Error: more than one path (currently %i paths) lead to node '%s'." % (count, key.getFullName()))
                                isHierarchyValid = False
                return isHierarchyValid


        def _validatenaming(self, rootnode, group):
                ok = True
                for node in group:
                        if node.getParent() == rootnode and node.nodetype == "transform":
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


        def printnode(self, node, nodes, lvl=0):
                if node.nodetype == "transform":
                        print("%s" % node.getFullName())
                children = self._listchildnodes(node.getFullName(), node, "", nodes, False)
                for child in children:
                        self.printnode(child, nodes, lvl+1)


        def _resolve_phys(self, phys, group):
                phys.phys_parent = None
                if not hasattr(phys, "phys_children"):
                        phys.phys_children = []
                        print("Reset self=", phys)
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
                # Check children of grandparents.
                parent = parent.getParent()
                while parent != None:
                        phys_nodes = self._listchildnodes(parent.getFullName(), parent, "phys_", group, False)
                        if len(phys_nodes) > 1:
                                print("Error: mesh node '%s' may only have one phys node child (but has %i children)." % (parent.getName(), len(phys_nodes)))
                                return False
                        elif len(phys_nodes) == 1:
                                parent = phys_nodes[0]
                                break
                print("Phys %s has parent:" % phys.getFullName(), parent)
                if parent == None:
                        print("Error: phys node '%s' has no related parent phys node higher up in the hierarchy." % phys.getFullName())
                        return False
                phys.phys_parent = parent
                if not hasattr(parent, "phys_children"):
                        print("Init parent", parent, "with self=", phys)
                        parent.phys_children = [phys]
                else:
                        print("Adding self=", phys, "to parent.")
                        parent.phys_children += [phys]
                        print(parent.phys_children)
                return True


        def _is_valid_phys_ref(self, group, phys, mesh, loose):
                if mesh.nodetype != "transform":
                        return False
                invalid_child_list = ["polyCube", "polySphere", "polyCylinder", "polyTorus", "polySmoothFace", "polyTweak", \
                                      "polyMergeVert", "groupId", "groupParts", "polyUnite", "script", "camera"]
                meshcnt = 0
                invcnt = 0
                children = self._listchildnodes(mesh.getFullName(), mesh, "m_", group, False)
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
                        if not phys.phys_parent and mesh.getParent():
                                valid_ref = False
                                print("Error: root mesh node '%s' (referenced by '%s') erroneously has parent." %
                                      (mesh.getFullName(), phys.getFullName()))
                        if not loose and phys.phys_parent and not phys.get_fixed_attribute("joint", optional=True):
                                valid_ref = False
                                print("Error: mesh '%s' referenced by non-jointed phys node '%s'." %
                                      (mesh.getFullName(), phys.getFullName()))
                        if loose and (not phys.phys_parent or phys.get_fixed_attribute("joint", optional=True)):
                                valid_ref = False
                                print("Error: attached mesh '%s' referenced by jointed phys node '%s'." %
                                      (mesh.getFullName(), phys.getFullName()))
                        mt = mesh.get_world_pivot()
                        pt = phys.get_world_pivot()
                        eps = 0.0001
                        if math.fabs(mt[0]-pt[0]) >= eps or \
                                math.fabs(mt[1]-pt[1]) >= eps or \
                                math.fabs(mt[2]-pt[2]) >= eps:
                                valid_ref = False
                                print("Error: mesh '%s' and jointed phys node '%s' do not share the exact same World Space Pivot (%s and %s). " \
                                      "Note that Y and Z axes are transformed compared to Maya." %
                                      (mesh.getFullName(), phys.getFullName(), str(mt), str(pt)))
                else:
                        valid_ref = False
                        if meshcnt > 0:
                                valid_ref = True
                                print("Error: mesh '%s' contains mesh shape, but is not linked to phys node." % mesh.getFullName())
                return valid_ref


        def _is_valid_mesh_ref(self, mesh, phys):
                valid_ref = True
                if phys.nodetype != "transform":
                        valid_ref = False
                        print("Error: phys node '%s' may not reference any mesh node (accidental to mesh '%s'?)." %
                                (phys.getFullName(), mesh.getFullName()))
                return valid_ref


        def _listchildnodes(self, path, basenode, prefix, group, recursive):
                childlist = []
                for node in group:
                        if node.getParent() == basenode and node.getName().startswith(prefix):
                              childlist += [node]
                grandchildlist = []
                if recursive:
                        for child in childlist:
                                childpath = path+"|"+child.getName()
                                grandchildlist += self._listchildnodes(childpath, child, prefix, group, True)
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
        pwr = chunkywriter.PhysWriter(sys.argv[1], rd.group, rd.config)
        mwr = chunkywriter.MeshWriter(sys.argv[1], rd.group, rd.config)
        cwr = chunkywriter.ClassWriter(sys.argv[1], rd.group, rd.config)
        pwr.dowrite()
        mwr.dowrite()
        cwr.dowrite()
        pwr.printfeats()
        mwr.printfeats()
        cwr.printfeats()
        print("Import successful.")


if __name__=="__main__":
        main()
