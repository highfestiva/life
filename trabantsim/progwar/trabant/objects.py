#!/usr/bin/env python3


from trabant.math import *


class GfxMesh:
	def __init__(self, q, pos, vertices, indices):
		self.q = q
		self.pos = pos
		self.vertices = vertices
		self.indices = indices
	def orthoscale(self, orientation, scale):
		self.pos = self.pos.mulvec((orientation*scale).abs())
		self.vertices = [v.mulvec(scale) for v in self.vertices]
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

def orthoscale(scale):
	def doscale(orientation,gfx,phys):
		s = tovec3(scale)
		o = toquat(orientation)
		gfx.orthoscale(o,s)
		ophys = o.rotate_x(-pi/2)   # Physics flip table.
		[p.orthoscale(ophys,s) for p in phys]
		return o,gfx,phys
	return doscale

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
