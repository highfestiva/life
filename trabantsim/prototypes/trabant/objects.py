#!/usr/bin/env python3


from trabant.math import *


class GfxMesh:
	def __init__(self, q, pos, vertices, indices):
		self.q = q
		self.pos = pos
		self.vertices = vertices
		self.indices = indices
		self._radius = None
	def orthoscale(self, orientation, scale):
		self.pos = self.pos.mulvec((orientation*scale).abs())
		self.vertices = [v.mulvec(scale) for v in self.vertices]
	@property
	def radius(self):
		if not self._radius:
			self._radius = max(v*v for v in self.vertices)
			self._radius = sqrt(self._radius)
		return self._radius
	def __repr__(self):
		return 'gfx-mesh (%g,%g,%g,%g,%g,%g,%g) %s %s' % tuple(list(self.q)+list(self.pos)+['('+', '.join(str(v) for v in self.vertices)+')']+[tuple(self.indices)])
	def __eq__(self,o):
		if type(o)==GfxMesh: return self.q==o.q and self.pos==o.pos and self.vertices==o.vertices and self.indices==o.indices
		return False

class PhysBox:
	def __init__(self, q, pos, size):
		self.q = q
		self.pos = pos
		self.size = size
	def orthoscale(self, orientation, scale):
		self.pos = self.pos.mulvec((orientation*scale).abs())
		self.size = self.size.mulvec(scale)
	def __repr__(self):
		return 'phys-box (%g,%g,%g,%g,%g,%g,%g) %g %g %g' % tuple(list(self.q)+list(self.pos)+[float(s) for s in self.size])
	def __eq__(self,o):
		if type(o)==PhysBox: return self.q==o.q and self.pos==o.pos and self.size==o.size
		return False

class PhysSphere:
	def __init__(self, q, pos, radius):
		self.q = q
		self.pos = pos
		self.radius = radius
	def orthoscale(self, orientation, scale):
		self.pos = self.pos.mulvec((orientation*scale).abs())
		self.radius *= scale.length()/3
	def __repr__(self):
		return 'phys-sphere (%g,%g,%g,%g,%g,%g,%g) %g' % tuple(list(self.q)+list(self.pos)+[float(self.radius)])
	def __eq__(self,o):
		if type(o)==PhysSphere: return self.q==o.q and self.pos==o.pos and self.radius==o.radius
		return False

class PhysCapsule:
	def __init__(self, q, pos, radius, length):
		self.q = q
		self.pos = pos
		self.radius = radius
		self.length = length
	def orthoscale(self, orientation, scale):
		self.pos = self.pos.mulvec((orientation*scale).abs())
		self.radius *= scale.with_z(0).length()/2
		self.length *= scale.with_x(0).with_y(0)
	def __repr__(self):
		return 'phys-capsule (%g,%g,%g,%g,%g,%g,%g) %g %g' % tuple(list(self.q)+list(self.pos)+[float(self.radius),float(self.length)])
	def __eq__(self,o):
		if type(o)==PhysCapsule: return self.q==o.q and self.pos==o.pos and self.radius==o.radius and self.length==o.length
		return False

class PhysMesh:
	def __init__(self, q, pos, vertices, indices):
		self.q = q
		self.pos = pos
		self.vertices = vertices[:]
		self.indices = indices[:]
	def orthoscale(self, orientation, scale):
		self.pos = self.pos.mulvec((orientation*scale).abs())
		self.vertices = [v.mulvec(scale) for v in self.vertices]
	def __repr__(self):
		return 'phys-mesh (%g,%g,%g,%g,%g,%g,%g) %s %s' % tuple(list(self.q)+list(self.pos)+['('+', '.join(str(v) for v in self.vertices)+')']+[tuple(self.indices)])
	def __eq__(self,o):
		if type(o)==PhysMesh: return self.q==o.q and self.pos==o.pos and self.vertices==o.vertices and self.indices==o.indices
		return False

class Obj:
	def __init__(self, gfxmesh, physgeoms):
		self.gfxmesh = gfxmesh
		self.physgeoms = physgeoms
	def __repr__(self):
		return str(self.gfxmesh) + '\n'.join((print(geom) for geom in self.physgeoms))


def orthoscale(scale, scalephys=True):
	def doscale(orientation,gfx,phys):
		s = tovec3(scale)
		o = toquat(orientation)
		gfx.orthoscale(o,s)
		if scalephys:
			[p.orthoscale(o,s) for p in phys]
		return o,gfx,phys
	return doscale

def gfxscale(scale):
	def doscale(orientation,gfx,phys):
		s = tovec3(scale)
		o = toquat(orientation)
		gfx.orthoscale(o,s)
		return o,gfx,phys
	return doscale

def gfxoffset(x,y,z):
	def dooffset(orientation,gfx,phys):
		gfx.pos += vec3(x,y,z)
		return orientation,gfx,phys
	return dooffset

def gfxrot(x,y,z):
	def dooffset(orientation,gfx,phys):
		o = quat().rotate_x(x).rotate_y(y).rotate_z(z)
		gfx.vertices = [o*v for v in gfx.vertices]
		return orientation,gfx,phys
	return dooffset

def gfx_vertex_func(func):
	def dovfunc(orientation,gfx,phys):
		gfx.vertices = [func(gfx,v) for v in gfx.vertices]
		return orientation,gfx,phys
	return dovfunc

def gfx_ortho_pinch(x,y,z, amp=1, rimscale=1, func=lambda a:abs(sin(a))-0.9):
	pinch_axes = vec3(x,y,z)
	def pinch_func(gfx, v):
		vp = v.mulvec(pinch_axes).abs()
		angle = vp.length() * pi * rimscale / gfx.radius
		A = amp * func(angle)
		return v + v.mulvec(vp*A)
	return gfx_vertex_func(pinch_func)

def nophys(orientation,gfx,phys):
	return orientation,gfx,[]

def process_rot(angle):
    angle = tovec3(angle)
    def dorot(orientation,gfx,phys):
        o = orientation.rotate_y(angle.y).rotate_x(angle.x).rotate_z(angle.z)
        return o,gfx,phys
    return dorot

def process_chain(*processors):
	def doprocess(orientation,gfx,phys):
		for process in processors:
			orientation,gfx,phys = process(orientation,gfx,phys)
		return orientation,gfx,phys
	return doprocess

def sphere_resolution(radius):
	latitude = int(min(8, max(4,radius**0.3)*8))
	longitude = int(latitude*1.5)
	return latitude,longitude

def capsule_resolution(radius, length):
	return sphere_resolution(radius)
