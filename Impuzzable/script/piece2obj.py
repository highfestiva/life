#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asc
import sys
from functools import partial,reduce
from math import sqrt,pi,cos,sin
from quat import quat
from vec3 import vec3,almosteq


sq2 = sqrt(2)
qa,qc = cos(pi/8),sin(pi/8)
rotq = quat(qa, 0, qc, 0)
invrotq = quat(qa, 0, -qc, 0)


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
		return 'phys-box (%g,%g,%g,%g,%g,%g,%g) %s' % tuple(list(self.q)+list(self.pos)+[str(self.size)])

class PhysMesh:
	def __init__(self, q, pos, vertices, indices):
		self.q = quat(q)
		self.pos = vec3(pos)
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

class _Triangle:
	'''Internal representation of triangle face in a mesh.'''
	def __init__(self, v1,v2,v3, vidxs, baseindex):
		self.v = (v1,v2,v3)
		self.center = (v1+v2+v3)/3
		self.normal = (v2-v1).cross(v2-v3).normalize()
		self.vidxs = vidxs
		self.baseindex = baseindex
		self.inwards = None
	def sides(self, vtxidx):
		sx = [k for k,ia in enumerate(self.vidxs) if ia==vtxidx][0]
		osx = [k for k,ia in enumerate(self.vidxs) if ia!=vtxidx]
		return [self.v[k]-self.v[sx] for k in osx]
	def contains(self, point):
		if not self.inwards:
			self.inwards = [(w-q).normalize().cross(self.normal) for w,q in zip(self.v[1:]+self.v[0:1],self.v)]
		return len([1 for inw,w in zip(self.inwards,self.v) if (point-w)*inw>=-0.1]) == 3
	def prepare_join(self, t):
		# Two things: check the opposing end lines up with either endpoint AND that resulting side length is approximately doubled.
		s = [k for k,ia in enumerate(self.vidxs) if ia not in t.vidxs]
		if len(s) > 1:
			return
		sx = s[0]
		tx = [k for k,ia in enumerate(t.vidxs) if ia not in self.vidxs][0]
		osx = [k for k,ia in enumerate(self.vidxs) if ia in t.vidxs]
		if len(osx) < 2:
			return
		dn = t.v[tx]-self.v[sx]
		dn /= dn.length()
		v1,v2 = self.v[osx[0]]-self.v[sx], self.v[osx[1]]-self.v[sx]
		l1,l2 = v1.length(),v2.length()
		joinable1,joinable2 = almosteq((v1/l1)*dn,1), almosteq((v2/l2)*dn,1)
		if joinable1 or joinable2:
			self.join_from_idx = self.baseindex + osx[0 if joinable1 else 1]
			self.join_to_idx = t.baseindex + tx
		return joinable1 or joinable2
	def extend(self, t, i):
		i[self.join_from_idx] = i[self.join_to_idx]
		i[t.baseindex] = i[t.baseindex+1] = i[t.baseindex+2] = 0
	def __eq__(self, t):
		return self.baseindex==t.baseindex
	def __hash__(self):
		return hash(self.baseindex)
	def __repr__(self):
		return str(self.v)


def printobj(o):
	print('Object has a gfx mesh and %i physical geometries:' % len(o.physgeoms))
	printvi(o.gfxmesh.vertices,o.gfxmesh.indices)
	[print(p) for p in o.physgeoms]

def printvi(v,i):
	print('Mesh has %i vertices, %i triangles=%i indices:' % (len(v),len(i)//3,len(i)))
	print(v)
	for j in range(0,len(i),3):
		p = ''
		if v[i[j]].x == v[i[j+1]].x and v[i[j+1]].x == v[i[j+2]].x:
			p += '-x' if v[i[j]].x<1 else '+x'
		if v[i[j]].y == v[i[j+1]].y and v[i[j+1]].y == v[i[j+2]].y:
			p += '-y' if v[i[j]].y<1 else '+y'
		if v[i[j]].z == v[i[j+1]].z and v[i[j+1]].z == v[i[j+2]].z:
			p += '-z' if v[i[j]].z<1 else '+z'
		if not p:
			p = '??'
		print('  %2i: %s %s %s (%s)' % (j, str(v[i[j]]), str(v[i[j+1]]), str(v[i[j+2]]), p))

def _get_vertex_crush_pos(v, vtxidx, faces, i):
	# Check for corners. Corner vertices may never be moved.
	if len(faces) >= 3:
		n0 = faces[0].normal
		n1 = None
		for f in faces:
			if n0*f.normal < 0.99:
				n1 = f.normal
				break
		if n1:
			c = n0.cross(n1).normalize()
			for f in faces:
				dot = c*f.normal
				if dot<-0.01 or dot>0.01:
					return
	# Check for adjacent sides. They must be equally long, or only reversed direction joins are allowed.
	req_direction = set()
	sides = [(s,s.length(),s.normalize()) for f in faces for s in f.sides(vtxidx)]
	for si,sln in enumerate(sides):
		sa,la,na = sln
		for sj in range(si+1,len(sides)):
			sb,lb,nb = sides[sj]
			if na*nb > 0.99:
				q = la/lb
				if q < 0.99 or q > 1.01:
					req_direction.add(-na)
					break
	if len(req_direction) > 1:	# Can only have one required direction.
		return
	normal = reduce(lambda x,y:x+y, [f.normal for f in faces])
	normal = normal.normalize()
	goodsides = []
	for f in faces:
		for s in f.sides(vtxidx):
			sn = s.normalize()
			dot = sn*normal
			if dot>-0.01 and dot<0.01:
				for rd in req_direction:
					if sn*rd<0.99:
						sn = None
						break
				if not sn:
					continue
				for gs,gsn in goodsides:
					if sn*gsn < -0.99:
						return v[vtxidx]+gs
				goodsides.append((s,sn))
	if goodsides:
		for gs,gsn in goodsides:
			if gs.z == 0:
				return v[vtxidx]+gs	# A flat crush is always better, if available.
		return v[vtxidx]+goodsides[0][0]	# Last pick, some skewed stuff.

def mesh_mergevtxs(v,i):
	found = {}
	drop_v_indices = []
	for vj,va in enumerate(v):
		vi = found.get(va)
		if vi != None:
			drop_v_indices.append((vj,vi))
		else:
			found[va] = vj
	movcnt = [0]*len(v)
	subcnt = [0]*len(v)
	for vj,vi in sorted(drop_v_indices, key=lambda e:e[0]):
		movcnt[vj] = vj-vi+subcnt[vi]
		for z in range(vj+1,len(v)):
			subcnt[z] += 1
	for j,ia in enumerate(i):
		i[j] -= movcnt[ia] if movcnt[ia] else subcnt[ia]
	for vj,_ in sorted(drop_v_indices, key=lambda e:e[0], reverse=True):
		del v[vj]	# Drop vertex.

def mesh_getfaces(v, wanted_indices, i, facecache):
	faces = []
	for wi in wanted_indices:
		bi = wi//3*3
		t = facecache.get(bi)
		if not t:
			idxs = i[bi],i[bi+1],i[bi+2]
			t = _Triangle(v[idxs[0]],v[idxs[1]],v[idxs[2]], idxs, bi)
			facecache[bi] = t
		faces.append(t)
	return faces

def mesh_dropface(baseindex, i):
	del i[baseindex]; del i[baseindex]; del i[baseindex]	# Drop all three indices.

def mesh_dropopposingfaces(v,i):
	usedvtxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): usedvtxs[j].append(ia)
	facecache = {}
	opposingfaces = {}
	for c in usedvtxs:
		faces = mesh_getfaces(v, c, i, facecache)
		for fi,f in enumerate(faces):
			for ti in range(fi+1,len(faces)):
				t = faces[ti]
				if f.normal*t.normal<-0.99 and (f.center-t.center).lengthsq() < asc.GRID*asc.GRID:
					if f.contains(t.center):
						opposingfaces[f.baseindex] = f
						opposingfaces[t.baseindex] = t
	for of in sorted(opposingfaces.values(), key=lambda o:o.baseindex, reverse=True):
		mesh_dropface(of.baseindex, i)

def mesh_dropsquashedfaces(v,i):
	drops = set()
	for bi in range(0,len(i),3):
		b = i[bi:bi+3]
		if b[0]==b[1] or b[0]==b[2] or b[1]==b[2]:
			drops.add(bi)
	[mesh_dropface(bi,i) for bi in sorted(drops,reverse=True)]

def mesh_dropequalfaces(v,i):
	drops = set()
	for bi in range(0,len(i),3):
		bs = i[bi:bi+3]
		for ci in range(bi+3,len(i),3):
			cs = i[ci:ci+3]
			if len([1 for b in bs if b in cs]) == 3:
				drops.add(ci)
	[mesh_dropface(bi,i) for bi in sorted(drops,reverse=True)]

def mesh_extendfaces(v,i):
	vtxidxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxidxs[j].append(ia)
	facecache = {}
	usedfaces = set()
	for c in vtxidxs:
		faces = mesh_getfaces(v, c, i, facecache)
		for fi,f in enumerate(faces):
			if f.baseindex in usedfaces: continue
			for ti in range(fi+1,len(faces)):
				t = faces[ti]
				if t.baseindex in usedfaces: continue
				if f.normal*t.normal > 0.99 and f.prepare_join(t):
					usedfaces.add(f.baseindex)
					usedfaces.add(t.baseindex)
					f.extend(t,i)
					break
	if usedfaces:
		mesh_dropsquashedfaces(v,i)
		#mesh_dropequalfaces(v,i)
		mesh_dropunusedvtxs(v,i)
	return len(usedfaces)

def mesh_crushfaces(v,i):
	vtxidxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxidxs[j].append(ia)
	facecache = {}
	usedfaces = set()
	for vi,c in enumerate(vtxidxs):
		faces = mesh_getfaces(v, c, i, facecache)
		if not faces or [f for f in faces if f in usedfaces]:
			continue
		target = _get_vertex_crush_pos(v, vi, faces, i)
		if target:
			usedfaces.update(faces)
			v[vi] = target
	if usedfaces:
		mesh_mergevtxs(v,i)
		mesh_dropsquashedfaces(v,i)
		#mesh_dropequalfaces(v,i)
		mesh_dropunusedvtxs(v,i)
	return len(usedfaces)

def mesh_joinfaces(v,i):
	while True:
		while mesh_extendfaces(v,i): pass
		if not mesh_crushfaces(v,i): break

def mesh_dropunusedvtxs(v,i):
	vtxusecnt = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxusecnt[j].append(ia)
	unused = [vi for vi,c in enumerate(vtxusecnt) if not c]
	subcnt = [0]*len(v)
	for u in unused:
		for z in range(u+1,len(v)):
			subcnt[z] += 1
	for j in range(len(i)):
		i[j] -= subcnt[i[j]]
	for u in reversed(unused):
		del v[u]

def mesh_optimize(v,i):
	# Merge vertices on same position.
	mesh_mergevtxs(v,i)
	mesh_dropopposingfaces(v,i)
	mesh_joinfaces(v,i)


def shape2mesh(shape):
	v = []
	i = []
	idx = 0
	for crd in asc.get_tri_crds(shape):
		o,topleft = asc.get_tri_pos(crd)
		lx,cx,rx = topleft.x,topleft.x+asc.GRID,topleft.x+asc.GRID*2
		ty,cy,by = topleft.y,topleft.y+asc.GRID,topleft.y+asc.GRID*2
		fz,bz = topleft.z,topleft.z+asc.GRID*2
		if o == asc.W2E:
			v += [vec3(lx,ty,fz),vec3(cx,cy,fz),vec3(lx,by,fz), vec3(lx,ty,bz),vec3(cx,cy,bz),vec3(lx,by,bz)]
		elif o == asc.N2S:
			v += [vec3(rx,ty,fz),vec3(cx,cy,fz),vec3(lx,ty,fz), vec3(rx,ty,bz),vec3(cx,cy,bz),vec3(lx,ty,bz)]
		elif o == asc.E2W:
			v += [vec3(rx,by,fz),vec3(cx,cy,fz),vec3(rx,ty,fz), vec3(rx,by,bz),vec3(cx,cy,bz),vec3(rx,ty,bz)]
		elif o == asc.S2N:
			v += [vec3(lx,by,fz),vec3(cx,cy,fz),vec3(rx,by,fz), vec3(lx,by,bz),vec3(cx,cy,bz),vec3(rx,by,bz)]
		j = [0,2,1, 5,3,4, 0,1,3, 3,1,4, 1,2,4, 4,2,5, 2,0,5, 5,0,3]
		i += [k+idx for k in j]
		idx += 6
	mesh_optimize(v,i)
	return GfxMesh(quat(1,0,0,0), vec3(0,0,0), v, i)

def cover_factor(remains,original):
	originalcnt = asc.tricnt(original)
	f = (originalcnt-asc.tricnt(remains) if remains else 0)/originalcnt
	return f*f

def take(oshape, shape, find, grow, create, directions):
	# Pick center coordinate.
	crds = list(asc.get_tri_crds(shape))
	crd = find(oshape, shape, crds)
	if not crd:
		return None,[]
	size = vec3(0,0,0)
	crds = []
	bestsize = None
	growing = True
	while growing:
		growing = False
		for d in directions:
			_crd,_size,_crds = grow(crd,size,d)
			if asc.any_crd_out_of_bound(oshape,_crds):
				continue
			if asc.any_crd_in_bound(shape,_crds):
				crd,size,crds = _crd,_size,crds+_crds
				bestsize = size
				growing = True
			else:
				# Grow indefinitely in same direction to see if there is any point in "sticking out in the other end."
				while True:
					_crd,_size,__crds = grow(_crd,_size,d)
					_crds += __crds
					if asc.any_crd_out_of_bound(oshape,__crds):
						break
					if asc.any_crd_in_bound(shape,__crds):	# We went through! At least a little... but I suppose some is better than nothing.
						crd,size,crds = _crd,_size,crds+_crds
						bestsize = size
						growing = True
						break
	if not bestsize:
		return None,[]
	remains = asc.clone_shape(shape)
	for c in crds:
		asc._settri(remains, c, False, True)
	return remains,create(bestsize,crds)

def prepgrowcuboid(crd, size, direction):
	if size==vec3(0,0,0):
		size = vec3(1,1,1)
		off = vec3(0,0,0)
	elif direction.x<0 or direction.y<0 or direction.z<0:
		crd += direction
		size = size-direction
		off = vec3(0,0,0)
	else:
		off = vec3(size)
		size = size+direction
	c = vec3(crd)
	d = crd+size
	if direction.x:
		c.x = c.x+off.x
		d.x = c.x+1
	elif direction.y:
		c.y = c.y+off.y
		d.y = c.y+1
	else:	# Z
		c.z = c.z+off.z
		d.z = c.z+1
	return crd,c,d,size

def growcuboid(crd, size, direction):
	crd = vec3(crd.x//2,crd.y//2,crd.z)
	crd,c,d,size = prepgrowcuboid(crd,size,direction)
	crds = []
	for z in range(c.z,d.z):
		for y in range(c.y,d.y):
			for x in range(c.x,d.x):
				crds += [vec3(x*2+0,y*2+0,z), vec3(x*2+1,y*2+0,z), vec3(x*2+0,y*2+1,z), vec3(x*2+1,y*2+1,z)]
	crd.x,crd.y = crd.x*2,crd.y*2
	return crd,size,crds

def crd2diamondcrd(crd):
	c = vec3(crd)
	o,topleft = asc.get_tri_pos(crd)
	if o == asc.N2S:
		c += vec3(-1,-1,0)
		o = asc.S2N
	elif o == asc.W2E:
		c += vec3(-1,+1,0)
	c.x = c.x//2*2 + (0 if o==asc.S2N else 1)
	c.y //= 2
	yoff = c.x//2
	c.x -= yoff+c.y
	c.y += yoff
	return c

def diamondcrd2crds(c):
	x = c.x+c.y
	yoff = x//2
	c.x = x
	c.y -= yoff
	if c.x&1 == 0:
		return [vec3(c.x,c.y*2+1,c.z), vec3(c.x+1,c.y*2+2,c.z)]
	return [vec3(c.x,c.y*2+1,c.z), vec3(c.x+1,c.y*2,c.z)]

def growdiamond(crd, size, direction):
	crd = crd2diamondcrd(crd)
	crd,c,d,size = prepgrowcuboid(crd,size,direction)
	crds = []
	for z in range(c.z,d.z):
		for y in range(c.y,d.y):
			for x in range(c.x,d.x):
				crds += diamondcrd2crds(vec3(x,y,z))
	crd = diamondcrd2crds(crd)[0]
	return crd,size,crds

def center(crds):
	avg = vec3(0.5,0.5,0.5)	# To center each shape triangle around local '1'.
	crds = [crd+avg for crd in crds]
	crd = reduce(lambda x,y:x+y, crds) / len(crds)	# Two shape triangles wide, two high.
	crd *= asc.GRID
	crd.z *= 2
	return crd

def createcuboid(size, crds):
	crd = center(crds)
	size *= asc.GRID*2
	return PhysBox(quat(1,0,0,0), crd, size)

def creatediamond(size, crds):
	crd = center(crds)
	size *= asc.GRID
	size.x *= sq2
	size.y *= sq2
	size.z *= 2
	return PhysBox(rotq, crd, size)

def findcuboid(oshape, shape, crds):
	offsets = (vec3(0,0,0),vec3(1,0,0),vec3(0,1,0),vec3(1,1,0))
	bestcnt = []
	for c in crds:
		c.x = c.x//2*2
		c.y = c.y//2*2
		if all(asc.hastri(oshape,c+o) for o in offsets):
			cnt = len([1 for o in offsets if asc.hastri(shape,c+o)])
			if cnt == 4:
				return c
			bestcnt.append((c,cnt))
	if not bestcnt:
		return None
	return sorted(bestcnt, key=lambda e:e[1])[0][0]

def finddiamond(oshape, shape, crds):
	for c in crds:
		if c.x%2==0 and c.y%2==0 and asc.hasboundstri(oshape,c+vec3(-1,+1,0)):
			return c
		elif c.x%2==1 and c.y%2==0 and asc.hasboundstri(oshape,c+vec3(-1,-1,0)):
			return c
		if c.x%2==0 and c.y%2==1 and asc.hasboundstri(oshape,c+vec3(+1,+1,0)):
			return c
		if c.x%2==1 and c.y%2==1 and asc.hasboundstri(oshape,c+vec3(+1,-1,0)):
			return c
	return None

def cuboid(oshape,shape):
	remains,geom = take(oshape, shape, findcuboid, growcuboid, createcuboid, (vec3(+1,0,0), vec3(0,+1,0), vec3(0,0,+1), vec3(-1,0,0), vec3(0,-1,0), vec3(0,0,-1)))
	return 1*cover_factor(remains,shape), geom, remains

def diamond(oshape,shape):
	remains,geom = take(oshape, shape, finddiamond, growdiamond, creatediamond, (vec3(+1,0,0), vec3(0,+1,0), vec3(0,0,+1), vec3(-1,0,0), vec3(0,-1,0), vec3(0,0,-1)))
	return 0.99*cover_factor(remains,shape), geom, remains

def shape2physgeoms(oshape, shape):
	if not shape or asc.tricnt(shape)==0:
		return []
	factor_geom_remains = [cuboid(oshape,shape), diamond(oshape,shape)]
	factor_geom_remains = [(f,g,r) for f,g,r in factor_geom_remains if f]
	if not factor_geom_remains:
		# Can't box it. Create a physical mesh instead, effectively consuming all remaining geometries.
		remaining_shapes = asc.get_split_shapes(shape)
		geoms = []
		for remains in remaining_shapes:
			gfxmesh = shape2mesh(remains)
			geoms += [PhysMesh(gfxmesh.q, gfxmesh.pos, gfxmesh.vertices, gfxmesh.indices)]
		return geoms
	factor,geom,remains = sorted(factor_geom_remains, key=lambda fgr:fgr[0], reverse=True)[0]
	return [geom] + shape2physgeoms(oshape, remains)

def centerobjs(gfx,physgeoms):
	if not gfx or not physgeoms:
		return
	gfx.q = physgeoms[0].q if physgeoms[0].q != rotq else invrotq
	off = physgeoms[0].pos
	for i,p in enumerate(physgeoms):
		if type(p) == PhysBox:
			p.pos -= off
		else:
			center = reduce(lambda x,y:x+y, p.vertices) / len(p.vertices)
			p.vertices = [v-center for v in p.vertices]
			if i == 0:
				p.pos = vec3(0,0,0)
				off = center
	gfx.vertices = [v-off for v in gfx.vertices]

def shape2obj(shape):
	gfx,physgeoms = shape2mesh(shape),shape2physgeoms(shape, shape)
	centerobjs(gfx,physgeoms)
	return Obj(gfx,physgeoms)

def piece2obj(f):
	shapes = asc.load_shapes_from_file(f, crop=False)
	objs = [shape2obj(s) for s in shapes]
	return objs

if __name__ == '__main__':
	objs = piece2obj(sys.stdin)
	print('\n~~~~~~~~~~~~~~~~~~\n'.join([str(obj) for obj in objs]))
