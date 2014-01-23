#!/usr/bin/env python3
# -*- coding: utf-8 -*-


from vec3 import *
from vec4 import *
from mat4 import *
from quat import *
import sys


def _getvertex(vs, idx):
	return vec3(vs[idx*3+0], vs[idx*3+1], vs[idx*3+2])


def _textureuv(idx, uvs):
	if uvs:
		idx *= 2
		return (uvs[idx], uvs[idx+1])
	return (0.0, 0.0)


def _normal(idx, ns):
	idx *= 3
	return vec3(ns[idx], ns[idx+1], ns[idx+2])


def _angle(n1, n2):
	m1, m2 = n1.max(), n2.max()
	if m1 == 0 or m2 == 0:
		if m1 == 0 and m2 == 0:
			return 0
		return 180
	return math.degrees(math.fabs(vec3.acos(n1*n2)))


def _angle_fast(n1, n2x, n2y, n2z):
	x = n1.x*n2x + n1.y*n2y + n1.z*n2z
	# Ugly, hard-coded optimization. Note that negative values mean opposite direction, so no need for math.fabs().
	if x < 0.65:
		return 0.7
	return vec3.acos(x)


def _uvdiff_sqr(t1, t2):
	dx = t1[0]-t2[0]
	dy = t1[1]-t2[1]
	return dx*dx + dy*dy


def scaleverts(group):
	for node in group:
		vtx = node.get_fixed_attribute("rgvtx", optional=True)
		if vtx:
			for p in node.getparents():
				if not p.shape:
					p.shape = node
			mnode = node.getParent()
			meshroot = None
			m_tr = mnode.gettransformto(meshroot, "original", getparent=lambda n: n.getParent())
			if not m_tr:
				print("Mesh crash!")
			m_s = m_tr.decompose()[2]
			transform = mat4.scaling(m_s)
			vp = vec4(0,0,0,1)
			for idx in range(0, len(vtx), 3):
				vp[:3] = vtx[idx:idx+3]
				vp = transform*vp
				vtx[idx:idx+3] = vp[:3]


def splitverts_group(group, verbose=False):
	"""Split mesh vertices that have different normals or UVs (=hard edges).
	   But to complicate things, I keep vertices together that share a similar normal/UV."""
	for node in group:
		splitverts_node(node, verbose)


def splitverts_node(node, verbose=False):
	vs = node.get_fixed_attribute("rgvtx", optional=True)
	ts = node.get_fixed_attribute("rgtri", optional=True)
	ns = node.get_fixed_attribute("rgn", optional=True)
	uvs = node.get_fixed_attribute("rguv", optional=True)
	if not ns:
		return

	for parent in node.getparents():
		nosplit = parent.get_fixed_attribute("nosplit", optional=True)
		if nosplit:
			if verbose: print("Not splitting mesh", node)
			return

	original_vsc = len(vs)
	original_nsc = len(ns)
	# print(node.getName()+":")
	# print(len(vs))
	# print(len(ts))
	# print(len(ns))
	# print(len(uvs))
	vs, ts, ns, uvs = splitverts(vs, ts, ns, uvs)
	node.fix_attribute("rgvtx", vs)
	node.fix_attribute("rgtri", ts)
	node.fix_attribute("rgn", ns)
	if uvs:
		node.fix_attribute("rguv", uvs)
	if verbose:
		print("Mesh %s was made %.1f times larger due to (hard?) edges, and %.1f %% of worst-case size." %
				(node.getName(), len(ns)/original_vsc-1, len(ns)*100/original_nsc))


def splitverts(vs, ts, ns, uvs):
	if not ns:
		return vs, ts, ns, uvs

	if len(vs) >= len(ts)*3:
		print("Internal error: too few triangle indices.")
		sys.exit(4)
	if len(ns) != len(ts)*3:
		print("Internal error: too few normals.")
		sys.exit(4)
	if uvs and len(uvs) != len(ts)*2:
		print("Internal error: too few UVs.")
		sys.exit(4)

	vs = vs[:]
	ts = ts[:]
	ns = ns[:]
	if uvs:
		uvs = uvs[:]
	#	print("UVs before split:")
	#	print(uvs)

	# Create a number of UNIQUE empty lists. Hence the for loop.
	shared_indices = []
	for u in range(len(vs)//3):
		shared_indices.append([])

	end = len(ts)
	x = 0
	while x < end:
		shared_indices[ts[x]] += [x]
		x += 1

	# Normalize normal vectors (length=1) for optimization purposes.
	ncnt = len(ns)
	x = 0
	while x < ncnt:
		l = ns[x]*ns[x] + ns[x+1]*ns[x+1] + ns[x+2]*ns[x+2]
		if l:
			f = 1 / math.sqrt(l)
			ns[x]   *= f
			ns[x+1] *= f
			ns[x+2] *= f
		x += 3

	x = 0
	ang_cmp = 40*math.pi/180
	while x < end:
		c = _normal(shared_indices[ts[x]][0], ns)
		d = _textureuv(shared_indices[ts[x]][0], uvs)
		split = []
		for s in shared_indices[ts[x]][1:]:
			#if _angle(c, _normal(s, ns)) > 40 or _uvdiff_sqr(d, _textureuv(s, uvs)) > 0.0001:
			i = s*3
			if _angle_fast(c, ns[i],ns[i+1],ns[i+2]) > ang_cmp or _uvdiff_sqr(d, _textureuv(s, uvs)) > 0.0001:
				split += [s]
		# Push all the once that we don't join together at the end.
		if split:
			new_index = len(vs)//3
			v = _getvertex(vs, ts[x])
			vs += v[:]
			shared_indices += [[]]
			for s in split:
				shared_indices[ts[s]].remove(s)
				ts[s] = new_index
				shared_indices[ts[s]] += [s]
			if len(shared_indices) != len(vs)/3:
				print("Internal error: normals/UVs no longer corresponds to vertices!")
				sys.exit(4)
		x += 1

	normals = [0.0]*len(vs)
	textureuvs = None if not uvs else [0.0]*(len(vs)*2//3)
	for join_indices in shared_indices:
		# Join 'em by simply adding normals together and normalizing.
		n = vec3(0,0,0)
		uv = [0.0, 0.0]
		cnt = 0;
		for j in join_indices:
			n += _normal(j, ns)
			if uvs:
				uv2 = _textureuv(j, uvs)
				uv[0] += uv2[0]
				uv[1] += uv2[1]
			cnt += 1
		idx = ts[join_indices[0]]
		n_idx = idx*3+0
		uv_idx = idx*2+0
		try:
			normals[n_idx:n_idx+3] = n.normalize()[:]
		except ZeroDivisionError:
			pass
		if uvs:
			try:
				textureuvs[uv_idx:uv_idx+2] = [uv[0]/cnt, uv[1]/cnt]
			except ZeroDivisionError:
				pass
	return vs, ts, normals, textureuvs


def centerverts(group, bodies, verbose):
	for shape in group:
		vtx = shape.get_fixed_attribute("rgvtx", optional=True)
		if not vtx:
			continue
		mesh = shape.getParent();
		if not mesh.get_fixed_attribute("center_vertices", optional=True):
			continue
		# Center around physics position, by removing translational offset for each vertex.
		phys,physidx,q,p,s = mesh.get_final_mesh_transform(None, bodies, verbose)
		p = quat(q).rotateVec(p)
		if verbose:
			print("Offsetting %s's %i vertices around %s, moving them %s." % (mesh.getName(), len(vtx)/3, phys.getName(), str(p)))
		for idx in range(0, len(vtx), 3):
			v = vec3(vtx[idx:idx+3]) + p
			vtx[idx:idx+3] = v[:3]
