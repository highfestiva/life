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

physics_type = {"world":1, "static":2, "dynamic":3, "collision_detect_only":4}
guide_mode = {"never":0, "external":1, "always":2, None:1}
resize_hints = {"resize_fast":0, "resize_nicest":1, "resize_canvas":2}
connector_types = {"connector_slider":1, "connector_universal":2, "connector_suspend_hinge":3, "connector_hinge":4, "connector_hinge2":5, "connector_3dof":6, "connector_fixed":7, "connectee_3dof":8}
joints = {None:1, "exclude":1, "fixed":2, "suspend_hinge":3, "hinge2":4, "hinge":5, "ball":6, "slider":7, "universal":8}

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
CHUNK_MESH_UVS_PER_VERTEX		= "MEUP"
CHUNK_MESH_COLOR			= "MECO"
CHUNK_MESH_COLOR_FORMAT			= "MECF"
CHUNK_MESH_TRIANGLES			= "METR"
CHUNK_MESH_PRIMITIVE			= "MEPR"
CHUNK_MESH_VOLATILITY			= "MEVO"
CHUNK_MESH_CASTS_SHADOWS		= "MECS"
CHUNK_MESH_SHADOW_DEVIATION		= "MESD"
CHUNK_MESH_TWO_SIDED			= "METS"
CHUNK_MESH_RECV_NO_SHADOWS		= "MERS"



class PhysMeshPtr:
	def __init__(self, physidx, meshbasename, t, scale, mat):
		self.physidx = physidx
		self.meshbasename = meshbasename
		self.t = t
		self.scale = scale
		self.mat = mat



class ChunkyWriter:
	def __init__(self, basename, group, config):
		self.basename = basename
		self.group = group
		self.config = config
		self.feats = {}


	def write(self):
		self.bodies, self.meshes, self.engines, self.phys_triggers = ChunkyWriter.sortgroup(self.group)
		self.physrootpos, q = self.bodies[0].get_final_local_transform()
		if not self.config["center_phys"]:
			self.physrootpos.x = 0
		self.physrootpos.y = 0
		self.physrootpos.z = 0
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


	@staticmethod
	def sortgroup(group):
		bodies = []
		meshes = []
		engines = []
		phys_triggers = []
		for node in group:
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
			elif len(chunks) == 2 and type(chunks[1]) == float:
				self._writeheader(chunks[0], 4)
				self._writefloat(chunks[1])
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
			mat.shininess, mat.alpha, mat.smooth, mat.resize_hint, mat.textures, mat.shader)


	def _writematerial_(self, name, ambient, diffuse, specular, shininess, alpha, smooth, resize_hint, textures, shader):
		if len(ambient) != 3 or len(diffuse) != 3 or len(specular) != 3:
			print("Error: bad number of material elements for %s:" % name, diffuse)
			#print(ambient, diffuse, specular)
			sys.exit(18)
		for f in ambient+diffuse+specular+[shininess,alpha]:
			self._writefloat(f)
		if options.options.verbose and not smooth:
			print("Flat shading on %s." % name)
		self._writefloat(float(smooth))
		self._writeint(resize_hints[resize_hint])
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
			def natural_sort_key(s):
				return [int(text) if text.isdigit() else text.lower() for text in re.split('([0-9]+)', s)] 
			subgroup = sorted(subgroup, key=lambda li: natural_sort_key(li.getFullName() if type(e) == str else li[0].getName()))
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
		center_baseline = (self.config["center_phys"] and self.config["center_phys"] != "ignore_baseline")
		if self.config["type"] == "dynamic" and center_baseline:
			if not node.phys_root and node.getParent().phys_children[0] == node:
				pos.z = -node.lowestpos.z
		#v = vec4(self.physrootpos)
		#v.y, v.z = -v.z, v.y
		#if node != self.bodies[0]:
		pos -= self.physrootpos
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
		bounce = node.get_fixed_attribute("bounce")
		self._writefloat(float(bounce))
		if options.options.verbose:
			print("Writing shape %s friction=%f, bounce=%f." % (node.getName(), friction, bounce))
		rootindex = -1 if not node.phys_root else self.bodies.index(node.phys_root)
		self._writeint(rootindex)
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
		if node.is_phys_root and node.xformparent == node.getphysmaster():
			j = node._pointup_adjust_mat() * j
		j -= self.physrootpos

		#print(node.getName(), "\n", mp, "\n", mt, "\n", wp, "\n", wt, "\n", ir, "\n", j)
		parameters[6:9] = j[:3]

		parameters[9] = node.get_fixed_attribute("impact_factor", True, 1.0)
		parameters[10] = 1.0 if node.get_fixed_attribute("collide_with_self", True, False) else 0.0

		detachable = any(node == mesh.get_mesh_parentphys(self.bodies) for mesh in self.meshes)
		if options.options.verbose:
			print('%s %s detachable.' % (node.getFullName(), 'is' if detachable else 'is not'))
		parameters[11] = 1.0 if detachable else 0.0

		if options.options.verbose:
			print("%s parameters:" % node.getName(), parameters)

		for x in parameters:
			self._writefloat(float(x))
		# Write connecor type (may hook other stuff, may get hooked by hookers :).
		connectors = node.get_fixed_attribute("connector_types", True)
		if connectors:
			if not type(connectors) == list and not type(connetors) == tuple:
				print("Error: connector_types for '%s' must be a list." % node.getFullName())
				sys.exit(19)
			self._writeint(len(connectors))
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
		scale = node.get_fixed_attribute("scale", default=1)
		shape.data = [type(x)(scale*x) for x in shape.data]
		if options.options.verbose:
			print("Writing shape %s with rootindex %i: %s (scale=%f)." % (node.getName(), rootindex, str(shape.data), scale))
		if node == self.bodies[0]:
			shape.adjustmesh(self.physrootpos)
		for x in shape.data:
			self._writenumber(x)
		self._addfeat("physical geometry:physical geometries", 1)


	def _writeengine(self, node):
		# Write all general parameters first.
		types = {v:i for i,v in enumerate("walk push_relative push_absolute push_turn_relative push_turn_absolute velocity_absolute_xy " \
											"hover hinge_roll hinge_gyro hinge_brake hinge_torque hinge2_turn rotor tilt jet slider_force glue ball_brake yaw_brake air_brake".split()}
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

		connected_to = node.get_fixed_attribute("connected_to")
		connected_to = self._expand_connected_list(connected_to, self.bodies)
		self._writeint(len(connected_to))
		for connected in connected_to:
			idx = self.bodies.index(connected)
			if options.options.verbose:
				print("Spawner '%s' connected to body index %i."% (node.getName(), idx))
			self._writeint(idx)
		v = node.get_fixed_attribute("initial_velocity", True, vec3())
		if options.options.verbose:
			print("Spawner '%s' has an initial velocity of %s." % (node.getName(), str(v)))
		[self._writefloat(float(u)) for u in v]
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
		ease_down = node.get_fixed_attribute("ease_down", optional=True)
		self._writeint(1 if ease_down else 0)

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
		def getmeshval(node, attrname, tag, isbool):
			val = node.getparentval(attrname, None)
			if val == None:
				return []
			elif isbool:
				if not val and options.options.verbose:
					print("%s will not /be/have %s!" % (node.getName(), attrname))
				return [(tag, +1 if val else -1)]
			return [(tag, val)]
		def get_casts_shadows(node):
			return getmeshval(node, "casts_shadows", CHUNK_MESH_CASTS_SHADOWS, True)
		def get_shadow_deviation(node):
			return getmeshval(node, "shadow_deviation", CHUNK_MESH_SHADOW_DEVIATION, False)
		def gettwosided(node):
			return getmeshval(node, "two_sided", CHUNK_MESH_TWO_SIDED, True)
		def get_recv_no_shadows(node):
			return getmeshval(node, "recv_no_shadows", CHUNK_MESH_RECV_NO_SHADOWS, True)
		mesh_primitive = node.getparentval("primitive", "triangles")
		primitive1 = mesh_primitive[:-1] if mesh_primitive[-1] == "s" else mesh_primitive
		primitiveM = mesh_primitive if mesh_primitive[-1] == "s" else mesh_primitive+"s"
		div = 4 if mesh_primitive == "quads" else 3
		uvs_per_vertex = node.getparentval("uvs_per_vertex", 2)
		if options.options.verbose:
			print("Writing mesh %s with %i %s..." % (filename, len(node.get_fixed_attribute("rgtri"))/div, primitiveM))
		self._addfeat("mesh:meshes", 1)
		self._addfeat("gfx %s:gfx %s"%(primitive1, primitiveM), len(node.get_fixed_attribute("rgtri"))/div)
		with self._fileopenwrite(filename) as f:
			self.f = f
			default_mesh_primitive = {"triangles":1, "triangle_strip":2, "lines":3, "line_loop":4, "quads":5}
			primitive = [(CHUNK_MESH_PRIMITIVE, default_mesh_primitive[mesh_primitive])]
			default_mesh_type = {"static":1, "semi_static":2, "dynamic":3, "volatile":4}
			mesh_type = "static" if self.config["type"] in ("world", "static") else "semi_static"
			volatility = [(CHUNK_MESH_VOLATILITY, default_mesh_type[mesh_type])]
			casts_shadows = get_casts_shadows(node)
			shadow_deviation = get_shadow_deviation(node)
			two_sided = gettwosided(node)
			recv_shadows = get_recv_no_shadows(node)
			vs = node.get_fixed_attribute("rgvtx")
			verts = [(CHUNK_MESH_VERTICES, vs)]
			polys = [(CHUNK_MESH_TRIANGLES, node.get_fixed_attribute("rgtri"))]
			normals = [] # node.get_fixed_attribute("rgn", optional=True)
			uvs = node.get_fixed_attribute("rguv", optional=True)
			if uvs and options.options.verbose:
				print("Mesh %s has a total UV float count of %i and a total vertex float count of %i." % (node.getFullName(), len(uvs), len(vs)))
				if options.options.super_verbose:
					print('UVs:')
					pairwise = lambda xs: zip(xs[::2], xs[1::2])
					[print('  %6.2f %6.2f' % (x,y)) for x,y in pairwise(uvs)]
			textureuvs = [(CHUNK_MESH_UV, uvs)] if uvs else []
			texture_uvs_per_vertex = [(CHUNK_MESH_UVS_PER_VERTEX, uvs_per_vertex)] if uvs_per_vertex != 2 else []
			inner_data = primitive+volatility+casts_shadows+shadow_deviation+two_sided+recv_shadows+verts+polys+normals+textureuvs+texture_uvs_per_vertex
			data = (
					CHUNK_MESH,
					inner_data
			)
			self._writechunk(data)
		for mesh in node.getparents():
			mesh.writecount += 1
		self._addfeat("file:files", 1)



class ClassWriter(ChunkyWriter):
	"""Translates a node/attribute class and writes it to disk as a class chunky file."""

	def __init__(self, basename, group, config):
		self.firstmeshscale = {}
		ChunkyWriter.__init__(self, basename, group, config)


	def dowrite(self):
		filename = self.basename+".class"
		with self._fileopenwrite(filename) as f:
			self.f = f
			meshptrs = []
			physidx = 0
			for m in self.meshes:
				phys,physidx,q,p,mscale = m.get_final_mesh_transform(self.physrootpos, self.bodies, options.options.verbose)
				if not phys:
					continue
				phys.writecount = 1
				m.writecount += 1
				if m.get_fixed_attribute("center_vertices", optional=True):
					p = [0.0,0.0,0.0]
				t = self._normalizexform(q[:]+p[:])
				try:
					mscale /= self.firstmeshscale[m.meshbasename]
				except:
					self.firstmeshscale[m.meshbasename] = mscale
					mscale = 1.0
				if mscale > 0.97 and mscale < 1.03: mscale = 1.0
				meshptrs += [(CHUNK_CLASS_PHYS_MESH, PhysMeshPtr(physidx, os.path.basename(m.meshbasename), t, mscale, m.mat))]
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
		self._writefloat(physmeshptr.scale)
		self._writematerial(physmeshptr.meshbasename, physmeshptr.mat)
		if options.options.verbose:
			print("Wrote %s's transform %s." % (physmeshptr.meshbasename, physmeshptr.t))
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
			if options.options.verbose:
				print("%s %s: %s" % (node.getName(), names[x], connected_to))
			self._writeint(len(connected_to))
			for cn in connected_to:
				self._writeint(objectlists[x].index(cn))
		node.writecount += 1
		self._addfeat("mesh tag:mesh tags", 1)
