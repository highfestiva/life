#!/usr/bin/env python3


class GfxMesh:
	def __init__(self, q, pos, vertices, indices):
		self.q = q
		self.pos = pos
		self.vertices = vertices
		self.indices = indices
	def __repr__(self):
		return 'gfx-mesh (%g,%g,%g,%g,%g,%g,%g) %s %s' % tuple(list(self.q)+list(self.pos)+['('+', '.join(str(v) for v in self.vertices)+')']+[tuple(self.indices)])

class PhysBox:
	def __init__(self, q, pos, size):
		self.q = q
		self.pos = pos
		self.size = size
	def __repr__(self):
		return 'phys-box (%g,%g,%g,%g,%g,%g,%g) %g %g %g' % tuple(list(self.q)+list(self.pos)+[float(s) for s in self.size])

class PhysSphere:
	def __init__(self, q, pos, radius):
		self.q = q
		self.pos = pos
		self.radius = radius
	def __repr__(self):
		return 'phys-sphere (%g,%g,%g,%g,%g,%g,%g) %g' % tuple(list(self.q)+list(self.pos)+[float(radius)])

class PhysMesh:
	def __init__(self, q, pos, vertices, indices):
		self.q = q
		self.pos = pos
		self.vertices = vertices[:]
		self.indices = indices[:]
	def __repr__(self):
		return 'phys-mesh (%g,%g,%g,%g,%g,%g,%g) %s %s' % tuple(list(self.q)+list(self.pos)+['('+', '.join(str(v) for v in self.vertices)+')']+[tuple(self.indices)])

class Obj:
	def __init__(self, gfxmesh, physgeoms):
		self.gfxmesh = gfxmesh
		self.physgeoms = physgeoms
	def __repr__(self):
		return str(self.gfxmesh) + '\n'.join((print(geom) for geom in self.physgeoms))
