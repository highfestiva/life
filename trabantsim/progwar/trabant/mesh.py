#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import trabant.asc as asc
from functools import reduce
from trabant.math import *


class _Triangle:
	'''Internal representation of triangle face in a mesh.'''
	def __init__(self, v1,v2,v3, vidxs, baseindex):
		self.v = [v1,v2,v3]
		self.center = (self.v[0]+self.v[1]+self.v[2])/3
		self.normal = (self.v[1]-self.v[0]).cross(self.v[1]-self.v[2]).normalize()
		self.vidxs = vidxs
		self.baseindex = baseindex
		self.inwards = None
	def replacevertex(self,vold,vnew):
		if vold in self.v:
			self.v[self.v.index(vold)] = vnew
			# Only update normal if we have area, otherse we're considered having unchanged normal.
			a,b,c = (self.v[1]-self.v[0]).length(),(self.v[2]-self.v[1]).length(),(self.v[0]-self.v[2]).length()
			p = (a+b+c)*0.5
			if p*(p-a)*(p-b)*(p-c) > 0.1:
				self.normal = (self.v[1]-self.v[0]).cross(self.v[1]-self.v[2]).normalize()
	# def getcenter(self):
		# if not self._center:
			# self._center = (self.v[0]+self.v[1]+self.v[2])/3
		# return self._center
	# def getnormal(self):
		# if not self._normal:
			# self._normal = (self.v[1]-self.v[0]).cross(self.v[1]-self.v[2]).normalize()
		# return self._normal
	# center = property(getcenter)
	# normal = property(getnormal)
	def sides(self, vtxidx):
		sx = [k for k,ia in enumerate(self.vidxs) if ia==vtxidx][0]
		osx = [k for k,ia in enumerate(self.vidxs) if ia!=vtxidx]
		return [self.v[k]-self.v[sx] for k in osx]
	def contains(self, point):
		if not self.inwards:
			self.inwards = [(w-q).normalize().cross(self.normal) for w,q in zip(self.v[1:]+self.v[0:1],self.v)]
		return len([1 for inw,w in zip(self.inwards,self.v) if (point-w)*inw>=-0.1]) == 3
	def prepare_join(self, t):
		# Two things: check the opposing end lines up with either endpoint. I.e.:
		#   |\           /\
		#   +->   NOT   <-->
		#   |/           \/
		s = [k for k,ia in enumerate(self.vidxs) if ia not in t.vidxs]
		if len(s) > 1:	# Only sharing a single vertex, can't join.
			return
		sx = s[0]
		tx = [k for k,ia in enumerate(t.vidxs) if ia not in self.vidxs][0]
		osx = [k for k,ia in enumerate(self.vidxs) if ia in t.vidxs]
		#if len(osx) < 2:
		#	return
		dn = t.v[tx]-self.v[sx]
		dn /= dn.length()
		v1,v2 = self.v[osx[0]]-self.v[sx], self.v[osx[1]]-self.v[sx]
		l1,l2 = v1.length(),v2.length()
		joinable1,joinable2 = almosteq(v1*dn,l1), almosteq(v2*dn,l2)
		if joinable1 or joinable2:
			self.join_from_idx = self.baseindex + osx[0 if joinable1 else 1]
			self.join_to_idx = t.baseindex + tx
			return True
		return False
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
	close0 = 0.0001
	close1p,close1m = 1+close0,1-close0
	# Check for corners. Corner vertices may never be moved.
	if len(faces) >= 3:
		n0 = faces[0].normal
		n1 = None
		for f in faces:
			if n0*f.normal < close1m:
				n1 = f.normal
				break
		if n1:
			c = n0.cross(n1).normalize()
			for f in faces:
				dot = c*f.normal
				if dot<-close0 or dot>close0:
					return
	# Check for adjacent sides. They must be equally long, or only reversed direction joins are allowed.
	req_direction = set()
	sides = [(s,s.length(),s.normalize()) for f in faces for s in f.sides(vtxidx)]
	for si,sln in enumerate(sides):
		sa,la,na = sln
		for sj in range(si+1,len(sides)):
			sb,lb,nb = sides[sj]
			if na*nb > close1m:
				q = la/lb
				if q < close1m or q > close1p:
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
			if dot>-close0 and dot<close0:
				for rd in req_direction:
					if sn*rd<close1m:
						sn = None
						break
				if not sn:
					continue
				for gs,gsn in goodsides:
					if sn*gsn < -close1m:
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
	if not drop_v_indices:
		return
	lv = len(v)
	movcnt = [0]*lv
	ordered = list(sorted(drop_v_indices, key=lambda e:e[0])) + [(lv,None)]
	idx = 0	# Always coincides with offset.
	for z in range(lv):
		if z > ordered[idx][0]:
			idx += 1
		movcnt[z] -= idx
	for vj,vi in drop_v_indices:
		movcnt[vj] = vi-vj + movcnt[vi]
	for j,ia in enumerate(i):
		i[j] += movcnt[ia]
	for vj,_ in sorted(drop_v_indices, key=lambda e:e[0], reverse=True):
		del v[vj]	# Drop vertex.

def mesh_getfaces(v, wanted_indices, i, facecache):
	faces = []
	for wi in wanted_indices:
		t = facecache.get(wi)
		if not t:
			idxs = i[wi],i[wi+1],i[wi+2]
			t = _Triangle(v[idxs[0]],v[idxs[1]],v[idxs[2]], idxs, wi)
			facecache[wi] = t
		faces.append(t)
	return faces

def mesh_dropface(baseindex, i):
	del i[baseindex]; del i[baseindex]; del i[baseindex]	# Drop all three indices.

def mesh_dropopposingfaces(v,i):
	usedvtxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): usedvtxs[j].append(ia//3*3)
	facecache = {}
	opposingfaces = {}
	ascgrid2 = asc.GRID*asc.GRID
	for c in usedvtxs:
		faces = mesh_getfaces(v, c, i, facecache)
		for fi,f in enumerate(faces):
			for ti in range(fi+1,len(faces)):
				t = faces[ti]
				if f.normal*t.normal<-0.99 and (f.center-t.center).length2() < ascgrid2:
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

def mesh_extendfaces(v,i,t):
	vtxidxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxidxs[j].append(ia//3*3)
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
	'''Crush face by moving shared vertex to opposing corner:
	   |\         |\
	   +->   TO   | >
	   |/         |/   '''
	vtxidxs = [[] for _ in range(len(v))]
	for ia,j in enumerate(i): vtxidxs[j].append(ia//3*3)
	facecache = {}
	usedfaces = set()
	for vi,c in enumerate(vtxidxs):
		faces = mesh_getfaces(v, c, i, facecache)
		if not faces or [1 for f in faces if f in usedfaces]:
			continue
		target = _get_vertex_crush_pos(v, vi, faces, i)
		if target:
			ns = [f.normal for f in faces]
			# Check if any of the polys flipped - if so go back!
			for f in faces:
				f.replacevertex(v[vi],target)
			if [1 for f,n in zip(faces,ns) if f.normal*n < 0.7]:
				for f in faces:
					f.replacevertex(target,v[vi])
			else:
				v[vi] = target
				usedfaces.update(faces)
			
	if usedfaces:
		mesh_mergevtxs(v,i)
		mesh_dropsquashedfaces(v,i)
		mesh_dropequalfaces(v,i)
		mesh_dropunusedvtxs(v,i)
	return len(usedfaces)

def mesh_joinfaces(v,i):
	while True:
		while mesh_extendfaces(v,i): pass
		if not mesh_crushfaces(v,i): break

def mesh_dropunusedvtxs(v,i):
	usedindices = set(i)
	lv = len(v)
	unusedindices = set(range(lv)) - usedindices
	movcnt = [0]*lv
	ordered = list(sorted(unusedindices))
	ordered_end = ordered+[lv]
	idx = 0	# Always coincides with offset.
	for z in range(lv):
		if z > ordered_end[idx]:
			idx += 1
		movcnt[z] -= idx
	for j,ia in enumerate(i):
		i[j] += movcnt[ia]
	for vi in reversed(ordered):
		del v[vi]	# Drop vertex.

def mesh_reduce_redundant_vtxs(v,i,stop_at_edge_cost=0):
	if len(i) <= 8*3:
		return 0
	class tri:
		def __init__(self,v0,v1,v2):
			self.vertex = [v0,v1,v2]
			self.vertexset = set(self.vertex)
			self.normal = (self.vertex[1].v-self.vertex[0].v).cross(self.vertex[1].v-self.vertex[2].v).normalize()
		def replacevertex(self,vold,vnew):
			self.vertex[self.vertex.index(vold)] = vnew
			self.vertexset = set(self.vertex)
			vnew.face.add(self)
			#if vold.cost != 0:
			self.normal = (self.vertex[1].v-self.vertex[0].v).cross(self.vertex[1].v-self.vertex[2].v).normalize()
	class vtx:
		def __init__(self,_v,idx):
			self.v = _v
			self.idx = idx
			self.neighbor = set()
			self.face = set()
			self.cost = 0.0
			self.collapse_vtx = None
	vertices = [vtx(_v,_i) for _i,_v in enumerate(v)]
	triangles = [tri(vertices[i[bi]],vertices[i[bi+1]],vertices[i[bi+2]]) for bi in range(0,len(i),3)]
	vertices = set(vertices)
	dropped_triangles = set()
	for t in triangles:
		t.vertex[0].face.add(t)
		t.vertex[1].face.add(t)
		t.vertex[2].face.add(t)
		t.vertex[0].neighbor.add(t.vertex[1])
		t.vertex[0].neighbor.add(t.vertex[2])
		t.vertex[1].neighbor.add(t.vertex[0])
		t.vertex[1].neighbor.add(t.vertex[2])
		t.vertex[2].neighbor.add(t.vertex[0])
		t.vertex[2].neighbor.add(t.vertex[1])
	mincurv_cache = {}
	def compute_edge_collapse_cost(u,v):
		# if we collapse edge uv by moving u to v then how much different will the model change, i.e. the “error”.
		edgelength = (v.v-u.v).length2();
		curvature = 0.0
		# find the "sides" triangles that are on the edge uv
		sides = [face for face in u.face if v in face.vertexset]
		# use the triangle facing most away from the sides to determine our curvature term
		for face in u.face:
			mincurv = 1.0
			for side in sides:
				# use dot product of face normals.
				mincurv = min(mincurv,(1-face.normal*side.normal)*0.5);
			curvature = max(curvature,mincurv);
		return edgelength * curvature
	def compute_edge_cost_at_vertex(v):
		v.collapse = None
		if not v.neighbor:
			v.cost = -0.01
			return
		v.cost = 1e6
		# search all neighboring edges for "least cost" edge
		for neighbor in v.neighbor:
			c = compute_edge_collapse_cost(v,neighbor)
			if c < v.cost:
				v.collapse = neighbor
				v.cost = c
	def collapse(u,v):
		# collapse the edge uv by moving vertex u onto v
		if not v:
			# u is a vertex all by itself so just delete it
			vertices.remove(u)
			return
		# delete triangles on edge uv
		for face in list(u.face):
			if v in face.vertexset:
				dropped_triangles.add(face)
				for q in face.vertex:
					q.face.remove(face)
			else:
				face.replacevertex(u,v)
		# correct neighbors' new neighbors
		recalc_neighborhood = list(u.neighbor)
		v.neighbor.remove(u)
		u.neighbor.remove(v)
		v.neighbor.update(u.neighbor)
		for un in u.neighbor:
			un.neighbor.remove(u)
			un.neighbor.add(v)
		vertices.remove(u)
		# recompute the edge collapse costs in neighborhood
		for nvtx in recalc_neighborhood:
			compute_edge_cost_at_vertex(nvtx)
	for v in vertices:
		compute_edge_cost_at_vertex(v)
	while vertices:
		mn = min(vertices, key=lambda _v:_v.cost)
		if mn.cost > stop_at_edge_cost:
			break
		collapse(mn,mn.collapse)
	j = 0
	for t in triangles:
		if t in dropped_triangles:
			continue
		i[j+0] = t.vertex[0].idx
		i[j+1] = t.vertex[1].idx
		i[j+2] = t.vertex[2].idx
		j += 3
	for k in reversed(range(j,len(i))):
		del i[k]
	return len(dropped_triangles)


def mesh_optimize(v,i,stop_at_edge_cost=0.02):
	mesh_dropunusedvtxs(v,i)
	mesh_mergevtxs(v,i)
	while True:
		mesh_reduce_redundant_vtxs(v,i,stop_at_edge_cost)
		if not mesh_crushfaces(v,i): break
	mesh_dropunusedvtxs(v,i)


def mesh_union(v,i,v2,i2,o2,p2):
    i += [ii+len(v) for ii in i2]
    v += [o2*vi+p2 for vi in v2]
    return v,i
