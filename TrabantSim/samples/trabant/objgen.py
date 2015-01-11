#!/usr/bin/env python3


from math import pi,cos,sin
from trabant.asc2obj import mesh_mergevtxs
from trabant.objects import *
from trabant.math import quat,vec3,tovec3


def createmesh(vertices,triangles):
	return GfxMesh(quat(),vec3(),vertices,triangles),[PhysMesh(quat(),vec3(),vertices[:],triangles[:])]

def createcube(side):
	s = side/2
	v = [(+s,-s,+s), (-s,-s,+s), (+s,-s,-s), (-s,-s,-s), (+s,+s,+s), (-s,+s,+s), (+s,+s,-s), (-s,+s,-s)]
	i = [0,1,2, 1,3,2, 1,5,3, 5,7,3, 4,0,6, 0,2,6, 5,4,7, 4,6,7, 4,5,0, 5,1,0, 2,3,6, 3,7,6]
	return GfxMesh(quat(),vec3(),[tovec3(c) for c in v],i),[PhysBox(quat(),vec3(),[side]*3)]

def createsphere(radius, latitude=8, longitude=12):
	v,i = [],[]
	def st(t,p):
		return p*longitude + t%longitude
	def sphere_triangles(t,p):
		if p >= latitude:
			return []
		return [st(t,p), st(t+1,p), st(t,p+1), st(t+1,p), st(t+1,p+1), st(t,p+1)]
	for phi_i in range(latitude+1):
		for theta_i in range(longitude):
			theta,phi = theta_i*2*pi/longitude,phi_i*pi/latitude
			waist = sin(phi)
			r = vec3(waist*cos(theta), -waist*sin(theta), cos(phi)) * radius
			v.append(r)
			i += sphere_triangles(theta_i,phi_i)
	return GfxMesh(quat(),vec3(),v,i),[PhysSphere(quat(),vec3(),radius)]

# def flatten_mesh(gfx):
	# i = flatten_triangles(gfx.vertices, gfx.indices)
	# return GfxMesh(gfx.q, gfx.pos, gfx.vertices, i)

# def flatten_triangles(vtxs,idxs):
	# '''Uniquify all the vertices which have different '''
	# Store all the unique surface normals per vertex, along with a list of referencing indices.
	# vnormals = [[] for _ in vtxs]
	# def append_normal(ii, normal):
		# for refs,n in vnormals[idxs[i]]:
			# if normal == n:
				# refs.append(ii)
				# return
		# vnormals.append(([ii],normal))
	# oidxs = idxs[:]
	# for ii in range(0,len(idxs),3):
		# v0,v1,v2 = vtxs[idxs[ii]],vtxs[idxs[ii+1]],vtxs[idxs[ii+2]]
		# n = (v1-v0).cross(v2-v1).normalize()
		# append_normal(ii,n)
		# append_normal(n,ii+1)
		# append_normal(n,ii+2)
	
		# for refs,n in vnormals[idxs[i]]:
			# if normal == n:
	# Index the normals.
	# c = 0
	# for i,nl in enumerate(vnormals):
		# for k,n in nl:
			# nl[k] = (c,n)
			# c += 1
	# Create new indices using the normal
	# oidxs
	# return oidxs
