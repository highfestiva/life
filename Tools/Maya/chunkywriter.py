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
import os
import pprint
import re
import struct
import sys

physics_type = {"world":1, "static":1, "dynamic":2, "collision_detect_only":3}
guide_mode = {"never":0, "external":1, "always":2, None:1}

CHUNK_CLASS				= "CLAS"
CHUNK_CLASS_PHYSICS			= "CLPH"
CHUNK_CLASS_MESH_LIST			= "CLML"
CHUNK_CLASS_PHYS_MESH			= "CLPM"
CHUNK_CLASS_TAG_LIST			= "CLTL"
CHUNK_CLASS_TAG				= "CLTA"

CHUNK_PHYSICS				= "PHYS"
CHUNK_PHYSICS_BONE_COUNT		= "PHBC"
CHUNK_PHYSICS_PHYSICS_TYPE		= "PHPT"
CHUNK_PHYSICS_PHYSICS_GUIDE_MODE	= "PHGM"
CHUNK_PHYSICS_BONE_CONTAINER		= "PHBO"
CHUNK_PHYSICS_BONE_CHILD_LIST		= "PHCL"
CHUNK_PHYSICS_BONE_TRANSFORM		= "PHBT"
CHUNK_PHYSICS_BONE_SHAPE		= "PHSH"
CHUNK_PHYSICS_ENGINE_COUNT		= "PHEC"
CHUNK_PHYSICS_ENGINE_CONTAINER		= "PHEO"
CHUNK_PHYSICS_ENGINE			= "PHEN"
CHUNK_PHYSICS_TRIGGER_COUNT		= "PHTC"
CHUNK_PHYSICS_TRIGGER_CONTAINER		= "PHTO"
CHUNK_PHYSICS_TRIGGER			= "PHTR"
CHUNK_PHYSICS_SPAWNER_COUNT		= "PHSC"
CHUNK_PHYSICS_SPAWNER_CONTAINER		= "PHSO"
CHUNK_PHYSICS_SPAWNER			= "PHSR"

CHUNK_MESH				= "MESH"
CHUNK_MESH_VERTICES			= "MEVX"
CHUNK_MESH_NORMALS			= "MENO"
CHUNK_MESH_UV				= "MEUV"
CHUNK_MESH_COLOR			= "MECO"
CHUNK_MESH_COLOR_FORMAT			= "MECF"
CHUNK_MESH_TRIANGLES			= "METR"
CHUNK_MESH_STRIPS			= "MEST"
CHUNK_MESH_VOLATILITY			= "MEVO"
CHUNK_MESH_CASTS_SHADOWS		= "MECS"



class PhysMeshPtr:
	def __init__(self, physidx, meshbasename, t, mat):
		self.physidx = physidx
		self.meshbasename = meshbasename
		self.t = t
		self.mat = mat



class ChunkyWriter:
	def __init__(self, basename, group, config):
		self.basename = basename
		self.group = group
		self.config = config
		self.feats = {}


	def write(self):
		self.bodies, self.meshes, self.engines, self.phys_triggers = self._sortgroup(self.group)
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
		writeerror = list(filter(lambda n: n.writecount != 1, nodes))
		if writeerror:
			print("Error: the following objects were missed, and never written to disk, or written more than once (when writing %s):" % when)
			def _p(n):
				print("  - %s (written %i times)" % (n.getFullName(), n.writecount))
			[_p(n) for n in writeerror]
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
		engines = []
		phys_triggers = []
		for node in self.group:
			node.writecount = 0
			if node.getName().startswith("phys_trig_") and node.nodetype == "transform":
				phys_triggers += [node]
				bodies += [node]	# A trigger is both trigger and body...
			elif node.getName().startswith("phys_pos_") and node.nodetype == "transform":
				bodies += [node]	# A position placed in body list, but will not be created in physical world...
			elif node.getName().startswith("phys_") and node.nodetype == "transform":
				bodies += [node]
			elif node.getName().startswith("m_") and node.nodetype == "transform":
				meshes += [node]
			elif node.getName().startswith("engine:") and node.nodetype.startswith("engine:"):
				engines += [node]
		def childlevel(node):
			c = 0;
			while node.getParent():
				node = node.getParent()
				c += 1
			return c
		bodies.sort(key=childlevel)
		meshes.sort(key=childlevel)
		return bodies, meshes, engines, phys_triggers


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
			elif node.nodetype.startswith("trigger:"):
				self._writetrigger(node)
			elif node.nodetype.startswith("spawner:"):
				self._writespawner(node)
			elif node.nodetype.startswith("tag:"):
				self._writetag(node)
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


	def _clampdata(self, xform, length):
		for x in range(length):
			xabs = math.fabs(xform[x])
			if xabs <= 1e-3:	# Close to zero -> 0.
				xform[x] = 0.0
			elif xabs >= 1-1e-3 and xabs <= 1:      # Close to one -> 1.
				xform[x] = math.copysign(1.0, xform[x])
			else:
				for y in range(x+1, length): # Close to other value -> both gets average.
					yabs = math.fabs(xform[y])
					if math.fabs(xabs-yabs) < 1e-3:
						avg = (xabs+yabs)/2
						xform[x] = math.copysign(avg, xform[x])
						xform[y] = math.copysign(avg, xform[y])


	def _normalizexform(self, xform):
		"Normalize the hard way. Most importantly the orientation."
##		self._clampdata(xform, 4)
##		xform[:4] = quat(xform[:4]).normalize()[:]
##		m = quat(xform[:4]).toMat4()
##		self._clampdata(m.mlist, 16)
##		xform[:4] = quat().fromMat(m).normalize()[:]
		self._clampdata(xform, 4)
		xform[:4] = quat(xform[:4]).normalize()[:]

		for x in range(4, 7):
			xround = round(xform[x], 1)
			if math.fabs(xround-xform[x]) < 1e-3:
				xform[x] = xround
		return xform


	def _writexform(self, xform):
		if len(xform) != 7:
			print("Error: trying to store transform with len != 7!")
			sys.exit(18)
		for f in xform:
			self._writefloat(f)


	def _writematerial(self, name, mat):
		self._writematerial_(name, mat.ambient, mat.diffuse, mat.specular, \
			mat.shininess, mat.alpha, mat.smooth, mat.textures, mat.shader)


	def _writematerial_(self, name, ambient, diffuse, specular, shininess, alpha, smooth, textures, shader):
		if len(ambient) != 3 or len(diffuse) != 3 or len(specular) != 3:
			print("Error: bad number of material elements for %s:" % name, diffuse)
			#print(ambient, diffuse, specular)
			sys.exit(18)
		for f in ambient+diffuse+specular+[shininess,alpha]:
			self._writefloat(f)
		if options.options.verbose and not smooth:
			print("Flat shading on %s." % name)
		self._writefloat(float(smooth))
		self._writeint(len(textures))
		for texture in textures:
			self._writestr(os.path.basename(texture))
		self._writestr(shader)


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
		res = list(string.encode('utf-8'))
		res += [0]      # C-string: zero termination character.
		# Overwrite 2-byte Unicode BOM with string length.
		chrcnt = len(res)
		res = [chrcnt&0xFF, chrcnt>>8] + res
		reslen = ((len(res)+3) & (~3))
		resremainder = reslen - len(res)
		res += [0] * resremainder
		#print("Writing string", res)
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
		#print("Warning: node '%s' not found!" % simplename)
		return None


	def _expand_connected_list(self, unexpanded, group):
		expanded = []
		for e in unexpanded:
			if type(e) == str:
				noderegexp = e
			else:
				noderegexp, rest = e[0], e[1:]
			subgroup = []
			for body in group:
				if re.search("^"+noderegexp+"$", body.getFullName()[1:]):
					if type(e) == str:
						subgroup += [body]
					else:
						subgroup += [(body,)+rest]
				else:
					#print("%s != %s..." % (noderegexp, body.getFullName()))
					pass
			subgroup = sorted(subgroup, key=lambda li: li.getFullName() if type(e) == str else li[0].getName())
			expanded += subgroup
		if len(expanded) < len(unexpanded):
			print("Error: could not expand %s into more than %i nodes!" % (str(unexpanded), len(expanded)))
			sys.exit(100)
		return expanded



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
			triggers = []
			spawners = []
			for node in self.bodies:
				#print("Children of %s: %s." % (node.getFullName(), repr(node.phys_children)))
				#[print("  - "+n.getName()) for n in node.phys_children]
				childlist = filter(lambda n: n in self.bodies, node.phys_children)
				childlist = [self.bodies.index(n) for n in childlist]
				bones.append((CHUNK_PHYSICS_BONE_CHILD_LIST, childlist))
				bones.append((CHUNK_PHYSICS_BONE_TRANSFORM, node))
				bones.append((CHUNK_PHYSICS_BONE_SHAPE, self._getshape(node)))
			for node in self.group:
				if node.nodetype.startswith("engine:"):
					engines.append((CHUNK_PHYSICS_ENGINE, node))
				elif node.nodetype.startswith("trigger:"):
					triggers.append((CHUNK_PHYSICS_TRIGGER, node))
				elif node.nodetype.startswith("spawner:"):
					spawners.append((CHUNK_PHYSICS_SPAWNER, node))
			data =  (
					CHUNK_PHYSICS,
					(
						(CHUNK_PHYSICS_BONE_COUNT, len(self.bodies)),
						(CHUNK_PHYSICS_PHYSICS_TYPE, physics_type[self.config["type"]]),
						(CHUNK_PHYSICS_PHYSICS_GUIDE_MODE, guide_mode[self.config.get("guide_mode")]),
						(CHUNK_PHYSICS_ENGINE_COUNT, len(engines)),
						(CHUNK_PHYSICS_TRIGGER_COUNT, len(triggers)),
						(CHUNK_PHYSICS_SPAWNER_COUNT, len(spawners)),
						(CHUNK_PHYSICS_BONE_CONTAINER, bones),
						(CHUNK_PHYSICS_ENGINE_CONTAINER, engines),
						(CHUNK_PHYSICS_TRIGGER_CONTAINER, triggers),
						(CHUNK_PHYSICS_SPAWNER_CONTAINER, spawners),
					)
				)
			if options.options.verbose:
				pprint.pprint(data)
				print("Number of physical triggers:", len(self.phys_triggers))
				print("Number of logic triggers:", len(triggers))
			self._writechunk(data)
		self._verifywritten("physics", self.bodies)
		self._addfeat("file:files", 1)


	def _writebone(self, node):
		pos, q = node.get_final_local_transform()
		if not node.phys_root and node.getParent().phys_children[0] == node:
			pos = [pos.x, pos.y, -node.lowestpos.z]
		data = q[:]+pos[:3]
		self._normalizexform(data)
		if options.options.verbose:
			print("Writing bone %s (index %i) with relative data %s." % (node.getName(), self.bodies.index(node), data))
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
		types = {"capsule":1, "cylinder":2, "sphere":3, "box":4, "mesh":5}
		self._writeint(types[shape.type])
		node = shape.getnode()
		totalmass = self._gettotalmass()
		self._writefloat(float(node.get_fixed_attribute("mass")))
		friction = float(node.get_fixed_attribute("friction"))
		if self.config["type"] == "dynamic":
			friction *= totalmass / 1000.0
		self._writefloat(friction)
		self._writefloat(float(node.get_fixed_attribute("bounce")))
		rootindex = -1 if not node.phys_root else self.bodies.index(node.phys_root)
		self._writeint(rootindex)
		joints = {None:1, "exclude":1, "suspend_hinge":2, "hinge2":3, "hinge":4, "ball":5, "slider":6, "universal":7}
		jointtype = node.get_fixed_attribute("joint", True)
		jointvalue = joints[jointtype]
		#print(node.getName(), "is jointed by type", jointvalue)
		if jointtype:
			self._addfeat("joint:joints", 1)
		self._writeint(jointvalue)
		is_affected_by_gravity = 1 if node.get_fixed_attribute("affected_by_gravity") else 0
		if options.options.verbose and is_affected_by_gravity:
			print("Writing shape %s as affected by gravity." % node.getName())
		self._writeint(is_affected_by_gravity)
		type_body, type_trigger, type_position = 1, 2, 3
		if node.getName().startswith("phys_trig_"):
			phys_type = type_trigger
			if options.options.verbose:
				print("Writing shape %s as trigger." % node.getName())
		elif node.getName().startswith("phys_pos_"):
			phys_type = type_position
			if options.options.verbose:
				print("Writing shape %s as position." % node.getName())
		else:
			phys_type = type_body
		self._writeint(phys_type)
		# Write joint parameters.
		parameters = [0.0]*16
		#print("Total mass:", totalmass)
		parameters[0] = node.get_fixed_attribute("joint_spring_constant", True, 0.0) * totalmass
		parameters[1] = node.get_fixed_attribute("joint_spring_damping", True, 0.0) * totalmass

		ir = node.getabsirot()

		yaw = node.get_fixed_attribute("joint_yaw", True, 0.0)*math.pi/180
		pitch = node.get_fixed_attribute("joint_pitch", True, 0.0)*math.pi/180
		if options.options.verbose and jointvalue >= 2:
			print("Joint %s euler angles are %s, damping is %f." % (node.getName(), (yaw, pitch), parameters[1]))
		parameters[2:4] = yaw, pitch

		joint_min, joint_max = node.get_fixed_attribute("joint_stops", True, [0.0,0.0])
		if jointtype == "slider":
			parameters[4:6] = joint_min, joint_max
		else:
			parameters[4:6] = math.radians(joint_min), math.radians(joint_max)

		mp = node.get_world_pivot_transform()
		mt = node.get_world_transform()
		wp = mp * vec4(0,0,0,1)
		wt = mt * vec4(0,0,0,1)
		#j = wt-wp
		j = wp-wt
		j = ir * j

		parameters[6:9] = j[:3]

		parameters[9] = node.get_fixed_attribute("impact_factor", True, 1.0)

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

		# Write material.
		material = node.get_fixed_attribute("material")
		if options.options.verbose:
			print("Writing physical material", material);
		self._writestr(material)

		# Write shape data (dimensions of shape).
		if options.options.verbose:
			print("Writing shape %s with rootindex %i." % (node.getName(), rootindex))
		for x in shape.data:
			self._writenumber(x)
		self._addfeat("physical geometry:physical geometries", 1)


	def _writeengine(self, node):
		# Write all general parameters first.
		types = {"walk":1, "push_relative":2, "push_absolute":3, "hover":4, "hinge_roll":5, "hinge_gyro":6, "hinge_brake":7, "hinge_torque":8, "hinge2_turn":9, "rotor":10, "tilter":11, "jet":12, "slider_force":13, "glue":14, "ball_brake":15, "yaw_brake":16, "air_brake":17}
		self._writeint(types[node.get_fixed_attribute("type")])
		totalmass = self._gettotalmass()
		self._writefloat(node.get_fixed_attribute("strength")*totalmass)
		self._writefloat(float(node.get_fixed_attribute("max_velocity")[0]))
		self._writefloat(float(node.get_fixed_attribute("max_velocity")[1]))
		self._writefloat(float(node.get_fixed_attribute("friction")))
		self._writeint(node.get_fixed_attribute("controller_index"))
		connected_to = node.get_fixed_attribute("connected_to")
		connected_to = self._expand_connected_list(connected_to, self.bodies)
		if len(connected_to) < 1:
			print("Error: could not find any matching nodes to connect engine '%s' to." % node.getFullName())
			sys.exit(19)
		self._writeint(len(connected_to))
		if options.options.verbose:
			print("%s expands connected_to list as:\n" % node.getName(), connected_to)
		for connection in connected_to:
			body, scale, connectiontype = connection
			idx = self.bodies.index(body)
			if options.options.verbose:
				print("Engine '%s' connected to body index %i (scale %f)."% (node.getName(), idx, scale))
			self._writeint(idx)
			self._writefloat(float(scale))
			connectiontypes = {"normal":1, "half_lock":2, "release":3}
			self._writeint(connectiontypes[connectiontype])
		#print("Wrote engine '%s' for %i nodes." % (node.getName()[6:], len(connected_to)))
		node.writecount += 1
		self._addfeat("physical engine:physical engines", 1)


	def _writetrigger(self, node):
		types = {"non_stop":1, "always":2, "movement":3}
		self._writeint(types[node.get_fixed_attribute("type")])
		self._writestr(node.get_fixed_attribute("function"))
		#self._writeint(node.get_fixed_attribute("trigger_group_index"))
		self._writeint(node.get_fixed_attribute("priority"))

		triggered_by_list = node.get_fixed_attribute("triggered_by")
		triggered_by_list = self._expand_connected_list(triggered_by_list, self.phys_triggers)
		self._writeint(len(triggered_by_list))
		for triggered_by in triggered_by_list:
			idx = self.bodies.index(triggered_by)
			if options.options.verbose:
				print("Trigger '%s' connected to bone index %i."% (node.getName(), idx))
			self._writeint(idx)

		connected_to = node.get_fixed_attribute("connected_to")
		connected_to = self._expand_connected_list(connected_to, self.engines)
		#if len(connected_to) < 1:
		#	print("Error: could not find any matching nodes to connect trigger '%s' to." % node.getFullName())
		#	sys.exit(19)
		self._writeint(len(connected_to))
		for connection in connected_to:
			engine, delay, function = connection
			idx = self.engines.index(engine)
			if options.options.verbose:
				print("Trigger '%s' connected to engine index %i."% (node.getName(), idx))
			self._writeint(idx)
			self._writefloat(float(delay))
			self._writestr(function)
		node.writecount += 1
		self._addfeat("trigger:triggers", 1)


	def _writespawner(self, node):
		types = {"teleport":1, "creator":2}
		self._writeint(types[node.get_fixed_attribute("type")])
		self._writestr(node.get_fixed_attribute("function"))

		connected_to_name = node.get_fixed_attribute("connected_to")
		connected_to = self._findglobalnode(connected_to_name)
		idx = self.bodies.index(connected_to)
		if options.options.verbose:
			print("Spawner '%s' connected to body index %i."% (node.getName(), idx))
		self._writeint(idx)
		self._writefloat(float(node.get_fixed_attribute("number")))
		intervals = node.get_fixed_attribute("intervals")
		self._writeint(len(intervals))
		for interval in intervals:
			self._writefloat(float(interval))
		spawn_objects = node.get_fixed_attribute("spawn_objects")
		self._writeint(len(spawn_objects))
		for spawn_class_distribution in spawn_objects:
			spawn_class, percent = spawn_class_distribution
			self._writestr(spawn_class)
			self._writefloat(float(percent))

		node.writecount += 1
		self._addfeat("spawner:spawners", 1)


	def _getshape(self, node):
##		shapenode = self._findphyschildnode(parent=node, nodetype="mesh")
##		if not shapenode:
##			print("Error: shape for node '%s' does not exist." % node.getFullName())
##			sys.exit(11)
##		in_nodename = shapenode.getInNode("i", "i")[0]
##		if not in_nodename:
##			print("Error: input shape for node '%s' does not exist." % shapenode.getFullName())
##			sys.exit(12)
##		in_node = self._findglobalnode(in_nodename)
		in_node = node.shape
		if not in_node:
			print("Error: unable to find input shape node '%s'." % in_nodename)
			sys.exit(13)
##		if not in_node.nodetype in ["polyCube", "polySphere", "polyCylinder"]:
##			print("Error: input shape node '%s' is of unknown type '%s'." % (in_node.getFullName(), in_node.nodetype))
##			sys.exit(14)
		return shape.Shape(node, in_node)


	def _findphyschildnode(self, parent, nodetype):
		return parent.shape
##		for node in self.group:
##			if node.getName().startswith("phys_") and node.nodetype == nodetype and node.getParent() == parent:
##				return node
##		print("Warning: certain node not found!")
##		return None


class MeshWriter(ChunkyWriter):
	"""Translates a node/attribute group and writes it to disk as mesh chunky files."""

	def __init__(self, basename, group, config):
		ChunkyWriter.__init__(self, basename, group, config)

	def dowrite(self):
		for node in self.group:
			if node.get_fixed_attribute("rgvtx", optional=True):
				meshbasename = node.getParent().meshbasename
				self.writemesh(meshbasename+".mesh", node)
				if options.options.verbose:
					print("Write %s's mesh parent %s." % (node.getFullName(), meshbasename))
		self._verifywritten("meshes", self.meshes)

	def writemesh(self, filename, node):
		if not self._isunique("file", filename):
			if options.options.verbose:
				print("Skipping write of instance %s." % filename)
			self._addfeat("mesh instance:mesh instances", 1)
			for p in node.getparents():
				p.writecount += 1
			return
		def getshadows(node):
			for parent in node.getparents():
				casts_shadows = parent.get_fixed_attribute("casts_shadows", optional=True)
				if casts_shadows:
					break
			if casts_shadows == None:
				casts_shadows = self.config.get("casts_shadows")
			if casts_shadows == None:
				return []
			elif casts_shadows:
				return [(CHUNK_MESH_CASTS_SHADOWS, +1)]
			if options.options.verbose:
				print("%s will not cast shadows!" % node.getName())
			return [(CHUNK_MESH_CASTS_SHADOWS, -1)]
		#print("Writing mesh %s with %i triangles..." % (filename, len(node.get_fixed_attribute("rgtri"))/3))
		self._addfeat("mesh:meshes", 1)
		self._addfeat("gfx triangle:gfx triangles", len(node.get_fixed_attribute("rgtri"))/3)
		with self._fileopenwrite(filename) as f:
			self.f = f
			default_mesh_type = {"static":1, "semi_static":2, "dynamic":3, "volatile":4}
			mesh_type = "static" if self.config["type"] == "static" else "semi_static"
			volatility = [(CHUNK_MESH_VOLATILITY, default_mesh_type[mesh_type])]
			shadows = getshadows(node)
			verts = [(CHUNK_MESH_VERTICES, node.get_fixed_attribute("rgvtx"))]
			polys = [(CHUNK_MESH_TRIANGLES, node.get_fixed_attribute("rgtri"))]
			normals = [] # node.get_fixed_attribute("rgn", optional=True)
			uvs = node.get_fixed_attribute("rguv", optional=True)
			# if uvs and options.options.verbose:
				# print("Mesh %s has UVs." % node.getFullName())
			textureuvs = [(CHUNK_MESH_UV, uvs)] if uvs else []
			inner_data = volatility+shadows+verts+polys+normals+textureuvs
			data = (
					CHUNK_MESH,
					inner_data
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
				tmt, tmr, _ = tm.decompose()
				tpt, tpr, _ = tp.decompose()
				q = quat(tpr.inverse()).normalize()
				p = tmt-tpt
				p = q.toMat4() * vec4(p[:])
				q = quat(tpr.inverse() * tmr).normalize()
				p = p[0:3]
				t = self._normalizexform(q[:]+p[:])
				physidx = self.bodies.index(phys)
				meshptrs += [(CHUNK_CLASS_PHYS_MESH, PhysMeshPtr(physidx, os.path.basename(m.meshbasename), t, m.mat))]
			tags = []
			for node in self.group:
				if node.getName().startswith("tag:"):
					#print("Adding tag %s." % node.getFullName())
					tags += [(CHUNK_CLASS_TAG, node)]
			data =  (
					CHUNK_CLASS,
					(
						(CHUNK_CLASS_PHYSICS, os.path.basename(self.basename)),
						(CHUNK_CLASS_MESH_LIST, meshptrs),
						(CHUNK_CLASS_TAG_LIST, tags),
					)
				)
			if options.options.verbose:
				pprint.pprint(data)
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
		col = [1.0, 0.0, 0.0, 1.0]
		#self._writematerial(col, col, col, ["da_texture"], "da_shader")
		self._writematerial(physmeshptr.meshbasename, physmeshptr.mat)
		if options.options.verbose:
			print("Wrote material diffuse %s for %s." % (str(physmeshptr.mat.diffuse), physmeshptr.meshbasename))
		self._addfeat("phys->mesh ptr:phys->mesh ptrs", 1)


	def _writetag(self, node):
		tagtype = node.get_fixed_attribute("type")
		self._writestr(tagtype)
		float_values = node.get_fixed_attribute("float_values")
		if options.options.verbose:
			print("Writing mesh tag %s: %s." % (tagtype, str(float_values)))
		self._writeint(len(float_values))
		for fv in float_values:
			self._writefloat(float(fv))
		string_values = node.get_fixed_attribute("string_values")
		self._writeint(len(string_values))
		for sv in string_values:
			self._writestr(sv)
		names = ["phys_list", "engine_list", "mesh_list"]
		objectlists = [self.bodies, self.engines, self.meshes]
		for x in range(len(names)):
			connected_to_names = node.get_fixed_attribute(names[x])
			connected_to = self._expand_connected_list(connected_to_names, objectlists[x])
			self._writeint(len(connected_to))
			for cn in connected_to:
				self._writeint(objectlists[x].index(cn))
		node.writecount += 1
		self._addfeat("mesh tag:mesh tags", 1)


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
		#	print(node, "has mesh children", node.childmeshes)
