#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import trabant.asc as asc
from trabant.objects import *
from functools import reduce
from math import sqrt,pi
from trabant.math import *
from trabant.mesh import mesh_optimize,mesh_mergevtxs


sq2 = sqrt(2)
rotq = quat().rotate_z(-pi/4)
_last_centering_offset = vec3()


def shape2mesh(shape):
	v = []
	i = []
	idx = 0
	for crd in asc.get_tri_crds(shape):
		o,topleft = asc.get_tri_pos(crd)
		lx,cx,rx = topleft.x,topleft.x+asc.GRID,topleft.x+asc.GRID*2
		ty,cy,by = topleft.y,topleft.y+asc.GRID,topleft.y+asc.GRID*2
		fz,bz = topleft.z,topleft.z+asc.GRID*2
		hasfront = asc.hasboundstri(shape,crd+vec3(0,0,-1))
		hasback  = asc.hasboundstri(shape,crd+vec3(0,0,+1))
		if o == asc.W2E:
			tv = [vec3(lx,ty,fz),vec3(cx,cy,fz),vec3(lx,by,fz), vec3(lx,ty,bz),vec3(cx,cy,bz),vec3(lx,by,bz)]
			hastop   = asc.hasboundstri(shape,crd+vec3(-1,+1,0))
			hasleft  = asc.hasboundstri(shape,crd+vec3( 0,+1,0))
			hasright = asc.hasboundstri(shape,crd+vec3(+1, 0,0))
		elif o == asc.N2S:
			tv = [vec3(rx,ty,fz),vec3(cx,cy,fz),vec3(lx,ty,fz), vec3(rx,ty,bz),vec3(cx,cy,bz),vec3(lx,ty,bz)]
			hastop   = asc.hasboundstri(shape,crd+vec3(-1,-1,0))
			hasleft  = asc.hasboundstri(shape,crd+vec3(-1, 0,0))
			hasright = asc.hasboundstri(shape,crd+vec3( 0,+1,0))
		elif o == asc.E2W:
			tv = [vec3(rx,by,fz),vec3(cx,cy,fz),vec3(rx,ty,fz), vec3(rx,by,bz),vec3(cx,cy,bz),vec3(rx,ty,bz)]
			hastop   = asc.hasboundstri(shape,crd+vec3(+1,-1,0))
			hasleft  = asc.hasboundstri(shape,crd+vec3( 0,-1,0))
			hasright = asc.hasboundstri(shape,crd+vec3(-1, 0,0))
		elif o == asc.S2N:
			tv = [vec3(lx,by,fz),vec3(cx,cy,fz),vec3(rx,by,fz), vec3(lx,by,bz),vec3(cx,cy,bz),vec3(rx,by,bz)]
			hastop   = asc.hasboundstri(shape,crd+vec3(+1,+1,0))
			hasleft  = asc.hasboundstri(shape,crd+vec3(+1, 0,0))
			hasright = asc.hasboundstri(shape,crd+vec3( 0,-1,0))
		else:
			raise Exception('Internal error!')
		j = []
		if not hasfront:	j += [0,2,1]
		if not hasback:		j += [5,3,4]
		if not hastop:		j += [2,0,5, 5,0,3]
		if not hasleft:		j += [1,2,4, 4,2,5]
		if not hasright:	j += [0,1,3, 3,1,4]
		if not j:
			continue
		v += tv
		i += [k+idx for k in j]
		idx += 6
	mesh_optimize(v,i)
	return GfxMesh(quat(1,0,0,0), vec3(), v, i)

def geoms2mesh(geoms):
	'''Convert physical geometries to a mesh. Count on overlapping triangles.'''
	v = []
	i = []
	for g in geoms:
		b = len(v)
		if type(g) == PhysMesh:
			v += g.vertices
			i += [b+gi for gi in g.indices]
		else:
			sx,sy,sz = g.size.x/2,g.size.y/2,g.size.z/2
			bv = [	vec3(-sx,-sy,-sz), vec3(+sx,-sy,-sz),
				vec3(-sx,+sy,-sz), vec3(+sx,+sy,-sz),
				vec3(-sx,-sy,+sz), vec3(+sx,-sy,+sz),
				vec3(-sx,+sy,+sz), vec3(+sx,+sy,+sz)  ]
			v += [g.pos+g.q*c for c in bv]
			i += [b+c for c in (0,2,1, 1,2,3, 5,7,4, 4,7,6, 1,5,0, 0,5,4, 2,6,3, 3,6,7, 1,3,5, 5,3,7, 4,6,0, 0,6,2)]
	mesh_mergevtxs(v,i)
	return GfxMesh(quat(1,0,0,0), vec3(), v, i)

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
	size = vec3()
	crds = []
	bestsize = None
	growing = True
	while growing:
		growing = False
		for d in directions:
			_crd,_size,_crds = grow(crd,size,d)
			if asc.any_crd_out_of_bound(oshape,_crds):
				#directions.remove(d)	# No way we could ever grow in this direction.
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
						break	# Nope, nothing found in this direction this time, but who knows when we've grown in other directions.
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
	if size==vec3():
		size = vec3(1,1,1)
		off = vec3()
	elif direction.x<0 or direction.y<0 or direction.z<0:
		crd += direction
		size = size-direction
		off = vec3()
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
	offsets = (vec3(),vec3(1,0,0),vec3(0,1,0),vec3(1,1,0))
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
	remains,geom = take(oshape, shape, findcuboid, growcuboid, createcuboid, [vec3(+1,0,0), vec3(0,+1,0), vec3(0,0,+1), vec3(-1,0,0), vec3(0,-1,0), vec3(0,0,-1)])
	return 1*cover_factor(remains,shape), geom, remains

def diamond(oshape,shape):
	remains,geom = take(oshape, shape, finddiamond, growdiamond, creatediamond, [vec3(+1,0,0), vec3(0,+1,0), vec3(0,0,+1), vec3(-1,0,0), vec3(0,-1,0), vec3(0,0,-1)])
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

def setcenterobj(gfx,physgeoms):
	physboxes = [p for p in physgeoms if type(p)==PhysBox]
	if not physboxes:
		return
	avgpos = sum((p.pos for p in physboxes), vec3()) / len(physboxes)
	q = physboxes[0].q
	sameqs = [p for p in physboxes if p.q==q]
	most_centered = min(physboxes, key=lambda p:(p.pos-avgpos).mulvec(vec3(3,1,1)).length())	# Most important to center around Y and Z, X is secondary.
	if gfx:
		gfx.pos -= most_centered.pos-physboxes[0].pos
	imc = physgeoms.index(most_centered)
	physgeoms[0],physgeoms[imc] = most_centered,physgeoms[0]

def centerobjs(gfx,physgeoms,center=vec3()):
	global _last_ascii_top_left_offset
	if not gfx or not physgeoms:
		return
	offset = physgeoms[0].pos
	_last_centering_offset = offset
	#print('_last_centering_offset', _last_centering_offset)
	for i,p in enumerate(physgeoms):
		if type(p) == PhysBox:
			p.pos -= offset
		else:
			off = reduce(lambda x,y:x+y, p.vertices) / len(p.vertices)
			p.vertices = [v-off for v in p.vertices]
			if i == 0:
				p.pos = vec3()
				offset = off
			else:
				p.pos -= offset-off
	gfx.vertices = [v-offset for v in gfx.vertices]
	gfx.pos -= offset
	gfx.q = physgeoms[0].q.inverse()
	physgeoms[0].pos += center

def last_centering_offset():
	return _last_centering_offset

def flipaxis(gfx,physgeoms):
	'''Objects have been generated Y up, Z out; but should appear Z up, Y in.'''
	if not physgeoms:
		return
	rot = quat().rotate_x(-pi/2)
	for geom in physgeoms:
		geom.pos = rot * geom.pos
		geom.q = rot * geom.q
	# def pr(x):
		# print('------------')
		# print(x)
	# pr(gfx)
	# [pr(p) for p in physgeoms]

def shape2obj(shape, fast=True, force_phys_mesh=False):
	'''Fast means redundant and suboptimal meshes. In fast generation we build
	physical geometries, then graphics meshes from "meshified" geometries.'''
	physgeoms = []
	if fast:
		physgeoms = shape2physgeoms(shape, shape)
		gfx = geoms2mesh(physgeoms)
	else:
		gfx = shape2mesh(shape)
	if force_phys_mesh:
		physgeoms = [PhysMesh(quat(),vec3(),gfx.vertices[:],gfx.indices[:])]
	elif not physgeoms:
		physgeoms = shape2physgeoms(shape, shape)
	setcenterobj(gfx,physgeoms)
	centerobjs(gfx,physgeoms)
	flipaxis(gfx,physgeoms)
	#[print(g) for g in [gfx]+physgeoms]
	return Obj(gfx,physgeoms)

def iter2objs(f, fast=True, force_phys_mesh=False):
	'''May also load objects from an opened file.'''
	shapes = asc.load_shapes_from_file(f, crop=False)
	objs = [shape2obj(s,fast,force_phys_mesh) for s in shapes]
	return objs

def str2obj(s, fast=True, force_phys_mesh=False):
	objs = iter2objs(s.split('\n'), fast, force_phys_mesh)
	if len(objs) == 1:
		return objs[0].gfxmesh, objs[0].physgeoms
	return objs
