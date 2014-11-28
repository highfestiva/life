#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import asc
import sys
from functools import reduce
from quat import quat
from vec3 import vec3,almosteq


class Mesh:
	def __init__(self, quat, pos, vertices, indices):
		self.quat = quat
		self.pos = pos
		self.vertices = vertices
		self.indices = indices
	def __repr__(self):
		return 'gfx-mesh (%g,%g,%g,%g,%g,%g,%g) %s %s' % tuple(list(self.quat)+list(self.pos)+['('+', '.join(str(v) for v in self.vertices)+')']+[tuple(self.indices)])

class MeshTri:
	def __init__(self, v1,v2,v3, vidxs, baseindex):
		self.v = (v1,v2,v3)
		self.center = (v1+v2+v3)/3
		self.normal = (v2-v1).cross(v2-v3).normalize()
		self.vidxs = list(vidxs)
		self.baseindex = baseindex
	def sides(self, vtxidx):
		sx = [k for k,ia in enumerate(self.vidxs) if ia==vtxidx][0]
		osx = [k for k,ia in enumerate(self.vidxs) if ia!=vtxidx]
		return [self.v[k]-self.v[sx] for k in osx]
	def contains(self, point):
		inwards = [(w-q).normalize().cross(self.normal) for w,q in zip(self.v[1:]+self.v[0:1],self.v)]
		return len([1 for inw,w in zip(inwards,self.v) if (point-w)*inw>=-0.1]) == 3
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
		#prt('vidxs =', self.vidxs, 't.vidxs =', t.vidxs, 'osx =',osx, 'v =', self.v, 'sx =', sx)
		dn = (t.v[tx]-self.v[sx]).normalize()
		v1,v2 = self.v[osx[0]]-self.v[sx], self.v[osx[1]]-self.v[sx]
		l1,l2 = v1.length(),v2.length()
		joinable1,joinable2 = almosteq((v1/l1)*dn,1), almosteq((v2/l2)*dn,1)
		if joinable1 or joinable2:
			prt('Joining faces: vidxs =', self.vidxs, 't.vidxs =', t.vidxs, 'osx =',osx, 'v =', self.v, 't.v =', t.v, 'sx =', sx)
			self.join_from_idx = self.baseindex + osx[0 if joinable1 else 1]
			self.join_to_idx = t.baseindex + tx
		return joinable1 or joinable2
	def extend(self, t, i):
		prt('extending base index %i with %i, joinfrom=%i, jointo=%i' % (self.baseindex, t.baseindex, self.join_from_idx, self.join_to_idx))
		#n = [k for k,ix in enumerate(i[self.baseindex:self.baseindex+3]) if ix not in t.vidxs][0]
		#vi = [k for k in t.vidxs if k not in self.vidxs][0]
		#prt('extending base index %i with %i n=%i, vi=%i, self=%s, t=%s, i[b+n]=%i->%i)' % (self.baseindex, t.baseindex, n, vi, str(self.vidxs), str(t.vidxs), i[self.baseindex+n], vi))
		i[self.join_from_idx] = i[self.join_to_idx]
		i[t.baseindex] = i[t.baseindex+1] = i[t.baseindex+2] = 0
	def __eq__(self, t):
		return self.baseindex==t.baseindex
	def __hash__(self):
		return hash(self.baseindex)
	def __repr__(self):
		return str(self.v)


def prt(*args):
	#print(*args)
	pass

def printobj(o):
	printvi(o.vertices,o.indices)

def printvi(v,i):
	print('Object has %i vertices, %i triangles=%i indices' % (len(v),len(i)//3,len(i)))
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
		print('%2i: %s %s %s (%s)' % (j, str(v[i[j]]), str(v[i[j+1]]), str(v[i[j+2]]), p))

def _get_vertex_crush_pos(v, vtxidx, faces, i):
	# Check for corners. Corner vertices may never be moved.
	if len(faces) >= 3:
		n0 = faces[0].normal
		n1 = None
		for f in faces:
			if n0*f.normal < 0.98:
				n1 = f.normal
				break
		if n1:
			c = n0.cross(n1).normalize()
			for f in faces:
				dot = c*f.normal
				if dot<-0.02 or dot>0.02:
					return
	# Check for adjacent sides. They must be equally long, or only reversed direction joins are allowed.
	req_direction = set()
	sides = [(s,s.length(),s.normalize()) for f in faces for s in f.sides(vtxidx)]
	for si,sln in enumerate(sides):
		sa,la,na = sln
		for sj in range(si+1,len(sides)):
			sb,lb,nb = sides[sj]
			if na*nb > 0.98:
				q = la/lb
				if q < 0.98 or q > 1.02:
					req_direction.add(-na)
					break
	if len(req_direction) > 1:	# Can only have one required direction.
		prt('skipping %s due to req_direction' % str(v[vtxidx]))
		return
	normal = reduce(lambda x,y:x+y, [f.normal for f in faces])
	normal = normal.normalize()
	goodsides = []
	for f in faces:
		for s in f.sides(vtxidx):
			sn = s.normalize()
			dot = sn*normal
			if dot>-0.02 and dot<0.02:
				for rd in req_direction:
					if sn*rd<0.98:
						sn = None
						break
				if not sn:
					continue
				for gs,gsn in goodsides:
					if sn*gsn < -0.98:
						prt('found crushable: v=%s, s=%s, normal=%s, gs=%s' % (str(v[vtxidx]),str(s),str(normal),str(gs)))
						return v[vtxidx]+gs
				prt('found plausible crushable: v=%s, s=%s, normal=%s, faces=%s' % (str(v[vtxidx]),str(s),str(normal),str(faces)))
				goodsides.append((s,sn))
	if goodsides:
		for gs,gsn in goodsides:
			if gs.z == 0:
				return v[vtxidx]+gs	# A flat crush is always better, if available.
		return v[vtxidx]+goodsides[0][0]	# Last pick, some skewed stuff.

def mesh_mergevtxs(v,i):
	drop_v_indices = []
	skip_v_indices = set()
	for vi,va in enumerate(v):
		if va in skip_v_indices:
			continue
		for vj in (vjj for vjj in range(vi+1,len(v)) if vjj not in skip_v_indices):
			if va==v[vj]:
				drop_v_indices.append((vj,vi))
				skip_v_indices.add(vj)
				#prt('dropping v index %i' % vj)
	for vj,vi in sorted(drop_v_indices, key=lambda t:t[0], reverse=True):
		for j,ia in enumerate(i):	# Replace indices.
			if ia == vj:
				#prt('replacing i[%i]==%i with %i' % (j,ia,vi))
				i[j] = vi
		for j,ia in enumerate(i):	# Lower all indices above dropped one.
			if ia > vj:
				i[j] -= 1
		del v[vj]	# Drop vertex.
	if drop_v_indices:
		mesh_dropunusedvtxs(v,i)	# TODO: possibly remove?
	#prt('merged %i vertices' % len(drop_v_indices))

def mesh_getfaces(v, wanted_indices, i):
	faces = []
	for wi in wanted_indices:
		bi = wi//3*3
		idxs = i[bi],i[bi+1],i[bi+2]
		faces.append(MeshTri(v[idxs[0]],v[idxs[1]],v[idxs[2]], idxs, bi))
	return faces

def mesh_dropface(baseindex, i):
	#prt('mesh_dropface', baseindex, len(i))
	del i[baseindex]; del i[baseindex]; del i[baseindex]	# Drop all three indices.

def mesh_dropopposingfaces(v,i):
	usedvtxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): usedvtxs[j].append(ia)
	sorteduvs = sorted([(len(c),i) for i,c in enumerate(usedvtxs)], key=lambda e:e[0], reverse=True)
	opposingfaces = {}
	for c in (usedvtxs[i] for l,i in sorteduvs):
		faces = mesh_getfaces(v, c, i)
		opposingfaces.update([(f.baseindex,f) for f in faces if [t for t in faces if f.normal*t.normal<-0.98 and f.contains(t.center) and (f.center-t.center).length() < asc.GRID]])
	#prt('dropping opposing faces:', [o.baseindex for o in opposingfaces.values()])
	for of in sorted(opposingfaces.values(), key=lambda o:o.baseindex, reverse=True):
		#prt('dropping opposing face %i' % of.baseindex)
		mesh_dropface(of.baseindex, i)

def mesh_dropsquashedfaces(v,i):
	drops = set()
	for bi in range(0,len(i),3):
		b = i[bi:bi+3]
		if b[0]==b[1] or b[0]==b[2] or b[1]==b[2]:
			#prt('dropping squashed face %s,%s,%s' % tuple([str(s) for s in b]))
			drops.add(bi)
	[mesh_dropface(bi,i) for bi in sorted(drops,reverse=True)]
	#prt('dropped %i squashed faces' % len(drops))

def mesh_dropequalfaces(v,i):
	drops = set()
	for bi in range(0,len(i),3):
		bs = i[bi:bi+3]
		for ci in range(bi+3,len(i),3):
			cs = i[ci:ci+3]
			if len([1 for b in bs if b in cs]) == 3:
				#prt('dropping equal faces %s %s,%s,%s == %s %s,%s,%s' % tuple([str(s) for s in [bi]+bs+[ci]+cs]))
				drops.add(ci)
	[mesh_dropface(bi,i) for bi in sorted(drops,reverse=True)]
	#prt('dropped %i equal faces' % len(drops))

def mesh_extendfaces(v,i):
	prt('before extending faces, now %i indices' % len(i))
	vtxidxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxidxs[j].append(ia)
	#joins = []
	usedfaces = set()
	for c in vtxidxs:
		faces = mesh_getfaces(v, c, i)
		for fi,f in enumerate(faces):
			if f.baseindex in usedfaces: continue
			for ti in range(fi+1,len(faces)):
				t = faces[ti]
				#prt('testing faces %i against faces %i' % (f.baseindex,t.baseindex))
				if t.baseindex in usedfaces: continue
				if f.normal*t.normal > 0.9 and f.prepare_join(t):
					#joins.append((f,t))
					usedfaces.update([f.baseindex,t.baseindex])
					f.extend(t,i)
					break
	# def adjust_tri_index(bi,ft):
		# f,t = ft
		# if bi>f.baseindex and bi>t.baseindex:
			# return ft
		# return (f.baseindex - (1 if bi<f.baseindex else 0), t.baseindex - (1 if bi<t.baseindex else 0))
	#for j in sorted(joins, key=lambda fs:fs[1].baseindex, reverse=True):
	#	f,t = joins[j]
		#prt('extending face (%i) %s with face (%i) %s' % (f.baseindex, str(f.v), t.baseindex, str(t.v)))
	#	f.extend(t,i)
		#mesh_dropface(t.baseindex, i)
		#for k in range(j+1,len(joins)):
		#	joins[k] = adjust_tri_index(t.baseindex,joins[k])
	if usedfaces:
		mesh_dropsquashedfaces(v,i)
		mesh_dropequalfaces(v,i)
	prt('after extending faces, now %i indices' % len(i))
	return len(usedfaces)

def mesh_crushfaces(v,i):
	#prt('before crushing faces, now %i indices' % len(i))
	vtxidxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxidxs[j].append(ia)
	usedfaces = set()
	for vi,c in enumerate(vtxidxs):
		faces = mesh_getfaces(v, c, i)
		if not faces or [f for f in faces if f in usedfaces]:
			continue
		target = _get_vertex_crush_pos(v, vi, faces, i)
		if target:
			prt('CRUSHING!!! %s -> %s' % (str(v[vi]),str(target)))
			usedfaces.update(faces)
			v[vi] = target
	if usedfaces:
		mesh_mergevtxs(v,i)
		mesh_dropsquashedfaces(v,i)
		mesh_dropequalfaces(v,i)
	prt('after crushing %i faces, %i indices left' % (len(usedfaces),len(i)))
	return len(usedfaces)

def mesh_joinfaces(v,i):
	while True:
		while mesh_extendfaces(v,i): pass
		if not mesh_crushfaces(v,i): break
	#while mesh_crushfaces(v,i): pass

def mesh_dropunusedvtxs(v,i):
	vtxusecnt = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxusecnt[j].append(ia)
	unused = [vi for vi,c in enumerate(vtxusecnt) if not c]
	for u in sorted(unused,reverse=True):
		for j,ia in enumerate(i):
			if ia > u:
				i[j] -= 1
		del v[u]
	prt('dropped %i unused vertices' % len(unused))

def mesh_optimize(v,i):
	# Merge vertices on same position.
	mesh_mergevtxs(v,i)
	mesh_dropopposingfaces(v,i)
	mesh_joinfaces(v,i)
	mesh_dropunusedvtxs(v,i)


def shape2obj(shape):
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
	#prt('before optimization len(i)=%i' % len(i))
	mesh_optimize(v,i)
	return Mesh(quat(1,0,0,0), vec3(0,0,0), v, i)

def piece2obj(f):
	shapes = asc.load_shapes_from_file(f, crop=False)
	objs = [shape2obj(s) for s in shapes]
	return objs

if __name__ == '__main__':
	objs = piece2obj(sys.stdin)
	print('\n~~~~~~~~~~~~~~~~~~\n'.join([str(obj) for obj in objs]))
