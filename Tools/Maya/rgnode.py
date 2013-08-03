# License: LGPL 2.1 (utilizes Python Computer Graphics Kit (cgkit), see other .py files for more info).
# Created by Jonas Byström, 2009-11-13 for Righteous Engine tool chain.


from mat4 import *
#from mayaascii import *
from quat import *
from vec3 import *
from vec4 import *


def adjustnode(node):
	"""Overridden to create fixed attributes, used for being able to override
	   configuration settings separate to the Maya ASCII .ma file."""
	node._fixattr = {}
	node.xformparent = node.getParent()
	node.__class__.enable_print_transform = False
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
	def get_final_local_transform(self):
		pm = self.xformparent.get_world_transform()
		wt = self.get_world_transform()
		if not self.phys_root:
			wt = node.gettransformto(None, "inverse_initial_r")
		m = pm.inverse() * wt
		jointtype = self.get_fixed_attribute("joint", True)
		if self.is_phys_root and jointtype not in (None, "exclude"):
			m *= self._pointup_adjust_mat()
		t, r, _ = m.decompose()
		q = quat(r).normalize()
		ps = pm.decompose()[2]
		pos = mat4.scaling(ps) * vec4(*t)
		return pos, q
	def get_world_pivot(self):
		return vec4(*self.get_world_pivot_transform().decompose()[0])
	def get_world_pivot_transform(self):
		wt = self.get_world_transform()
		r = self._get_local_quat().toMat4()
		vt = self._get_local_t()
		vs = self._get_local_s()
		vsh = self.get_fixed_attribute("sh")
		vrp = self._get_local_rpivot()
		vrt = self.get_fixed_attribute("rpt")
		vsp = self.get_fixed_attribute("sp")
		vst = self.get_fixed_attribute("spt")
		sp = mat4.translation(-vsp)
		s = mat4.scaling(vs)
		#sh = mat4.identity()
		#sh.mlist[2] = +sh[0]	# X: x(+z), originally x(+y) in Maya.
		#sh.mlist[9] = +sh[1]	# Y: z(-y), originally y(+z) in Maya.
		#sh.mlist[1] = -sh[2]	# Z: x(-y), originally x(+z) in Maya.
		sh = mat4.identity()
		sh[0,1] = +vsh[0]   # According to doc?
		sh[0,2] = +vsh[1]   # According to doc?
		sh[1,2] = +vsh[2]   # According to doc?
		spi = mat4.translation(vsp)
		st = mat4.translation(vst)
		rp = mat4.translation(-vrp)
		ar = self._get_local_ar()
		rpi = mat4.translation(vrp)
		rt = mat4.translation(vrt)
		t = mat4.translation(vt)
		# According to Maya doc (as I understood it): [sp][s][sh][sp^-1][st][rp][ar][r][rp^-1][rt][t].
		# See http://download.autodesk.com/us/maya/2010help/CommandsPython/xform.html for more info.
		# My multiplications are reversed order, since matrices already transposed.
		#m = t * rt * rpi * r * ar * rp * st * spi * sh * s * sp
		mup = mat4.identity()
		mup *= self._pointup_adjust_mat()
		m = wt * (rp * st * spi * sh * s * sp * mup).inverse()
		return m
		#lp = self._get_local_pivot()
		#return m * vec4(0, 0, 0, 1)
	def get_world_translation(self):
		return vec4(*self.get_world_transform().decompose()[0])
	def get_world_transform(self):
		return self.gettransformto(None)
	def get_local_transform(self):
		return self.gettransformto(self.xformparent)
	def csetprinttransform(cls, enable):
		cls.enable_print_transform = enable
	def setprinttransform(self, enable):
		self.__class__.csetprinttransform(enable)
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
			m = getattr(self, matname)
			r = pm * m
			if self.__class__.enable_print_transform:
				print(self.getName(), "used legacy transform %s:" % matname)
				print(m)
			return r
		r = self._get_local_quat().toMat4()
		vt = self._get_local_t()
		vs = self._get_local_s()
		vsh = self.get_fixed_attribute("sh")
		vrp = self._get_local_rpivot()
		vrt = self.get_fixed_attribute("rpt")
		vsp = self.get_fixed_attribute("sp")
		vst = self.get_fixed_attribute("spt")
		sp = mat4.translation(-vsp)
		s = mat4.scaling(vs)
		#sh = mat4.identity()
		#sh.mlist[2] = +sh[0]	# X: x(+z), originally x(+y) in Maya.
		#sh.mlist[9] = +sh[1]	# Y: z(-y), originally y(+z) in Maya.
		#sh.mlist[1] = -sh[2]	# Z: x(-y), originally x(+z) in Maya.
		sh = mat4.identity()
		sh[0,1] = +vsh[0]   # According to doc?
		sh[0,2] = +vsh[1]   # According to doc?
		sh[1,2] = +vsh[2]   # According to doc?
		spi = mat4.translation(vsp)
		st = mat4.translation(vst)
		rp = mat4.translation(-vrp)
		ar = self._get_local_ar()
		rpi = mat4.translation(vrp)
		rt = mat4.translation(vrt)
		t = mat4.translation(vt)
		# According to Maya doc (as I understood it): [sp][s][sh][sp^-1][st][rp][ar][r][rp^-1][rt][t].
		# See http://download.autodesk.com/us/maya/2010help/CommandsPython/xform.html for more info.
		# My multiplications are reversed order, since matrices already transposed.
		m = t * rt * rpi * r * ar * rp * st * spi * sh * s * sp
		m *= self._pointup_adjust_mat()
		#print("Matrix for", self.getName())
		#print(m)
		setattr(self, matname, m)
		r = pm * getattr(self, matname)
		if self.__class__.enable_print_transform:
			print(self.getName(), "just got transform %s:" % matname)
			print(m)
		return r
	def get_world_scale(self):
		return self.get_world_transform().decompose()[2]
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
						#print(i,j,k,u,v,w,s)
						#print("cos(a) of %s is %f." % (self.getFullName(), a))
						return False
		return True
	def _pointup_adjust_mat(self):
		if hasattr(self, "pointup") and self.pointup:
			# Some primitives have different orientation in the editor compared to
			# the runtime environment (Maya along Y-axis, RGE along Z-axis).
			return mat4.rotation(-math.pi/2, vec3(1,0,0))
		return mat4(1)
	def _get_local_s(self):
		return self.get_fixed_attribute("s", default=vec3(1,1,1))
	def _get_local_t(self):
		return self.get_fixed_attribute("t", default=vec3(0,0,0))
	def _get_local_rpivot(self):
		return self.get_fixed_attribute("rp", default=vec3(0,0,0))
	def _get_local_pivot(self):
		return self._get_local_rpivot() - self.get_fixed_attribute("spt")
	def _get_local_quat(self):
		rot = self.get_fixed_attribute("r", default=vec3(0,0,0))
		qx = quat().fromAngleAxis(rot[0], (1, 0, 0))
		qy = quat().fromAngleAxis(rot[1], (0, 1, 0))
		qz = quat().fromAngleAxis(rot[2], (0, 0, 1))
		q = qz*qy*qx
		return q
	def _get_local_ar(self):
		ra = vec3(self.get_fixed_attribute("ra", default=vec3(0,0,0)))
		x, y, z = mat4.identity(), mat4.identity(), mat4.identity()
		x[1,1] = +math.cos(ra.x)
		x[1,2] = +math.sin(ra.x)
		x[2,1] = -math.sin(ra.x)
		x[2,2] = +math.cos(ra.x)
		y[0,0] = +math.cos(ra.y)
		y[0,2] = -math.sin(ra.y)
		y[2,0] = +math.sin(ra.y)
		y[2,2] = +math.cos(ra.y)
		z[0,0] = +math.cos(ra.z)
		z[0,1] = +math.sin(ra.z)
		z[1,0] = -math.sin(ra.z)
		z[1,1] = +math.cos(ra.z)
		ro = self.getAttrValue("ro", "ro", "int", default=0)
		rotorder = ["x*y*z", "y*z*x", "z*x*y", "x*z*y", "y*x*z", "z*y*x"]
		mra = eval(rotorder[ro])
		#print("Result of rot:", mra*vec4(0,1,0,0))
		return mra
	def getphysmaster(self):
		phys = self
		while phys.phys_root:
			phys = phys.phys_root
		return phys
	def getabsirot(self):
		root = self.getphysmaster().getParent()
		#print("getabsirot checking", root)
		ir = root.gettransformto(None, "inverse_initial_r").decompose()[1]
		if root.flipjoints:
			ir = ir.rotate(math.pi, vec3(1,0,0))
		return ir
	node.fix_attribute = types.MethodType(fix_attribute, node)
	node.get_fixed_attribute = types.MethodType(get_fixed_attribute, node)
	node.get_final_local_transform = types.MethodType(get_final_local_transform, node)
	node.get_world_translation = types.MethodType(get_world_translation, node)
	node.get_world_pivot = types.MethodType(get_world_pivot, node)
	node.get_world_pivot_transform = types.MethodType(get_world_pivot_transform, node)
	node.get_world_transform = types.MethodType(get_world_transform, node)
	node.get_local_transform = types.MethodType(get_local_transform, node)
	node.gettransformto = types.MethodType(gettransformto, node)
	node.get_world_scale = types.MethodType(get_world_scale, node)
	node.isortho = types.MethodType(isortho, node)
	node._get_local_s = types.MethodType(_get_local_s, node)
	node._get_local_t = types.MethodType(_get_local_t, node)
	node._get_local_rpivot = types.MethodType(_get_local_rpivot, node)
	node._get_local_pivot = types.MethodType(_get_local_pivot, node)
	node._get_local_quat = types.MethodType(_get_local_quat, node)
	node._get_local_ar = types.MethodType(_get_local_ar, node)
	node.__class__.csetprinttransform = types.MethodType(csetprinttransform, node.__class__)
	node.setprinttransform = types.MethodType(setprinttransform, node)
	node.getphysmaster = types.MethodType(getphysmaster, node)
	node.getabsirot = types.MethodType(getabsirot, node)
	node._pointup_adjust_mat = types.MethodType(_pointup_adjust_mat, node)
	return node
