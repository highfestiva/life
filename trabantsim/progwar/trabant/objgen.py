#!/usr/bin/env python3


from math import pi,cos,sin
from trabant.asc2obj import mesh_mergevtxs
from trabant.objects import *
from trabant.math import quat,vec3,tovec3


def createmesh(vertices,triangles):
	return GfxMesh(quat(),vec3(),vertices,triangles),[PhysMesh(quat(),vec3(),vertices[:],triangles[:])]

def createcube(side):
	s = side*0.5
	v = [(+s.x,-s.y,+s.z), (-s.x,-s.y,+s.z), (+s.x,-s.y,-s.z), (-s.x,-s.y,-s.z), (+s.x,+s.y,+s.z), (-s.x,+s.y,+s.z), (+s.x,+s.y,-s.z), (-s.x,+s.y,-s.z)]
	i = [0,1,2, 1,3,2, 1,5,3, 5,7,3, 4,0,6, 0,2,6, 5,4,7, 4,6,7, 4,5,0, 5,1,0, 2,3,6, 3,7,6]
	return GfxMesh(quat(),vec3(),[tovec3(c) for c in v],i),[PhysBox(quat(),vec3(),side)]

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

def createcapsule(radius, length, latitude=8, longitude=12):
	latitude_odd = latitude if latitude&1 else latitude+1
	latitude = latitude_odd-1
	latitude_half = latitude/2
	v,i,l2 = [],[],length/2
	def st(t,p):
		return p*longitude + t%longitude
	def capsule_triangles(t,p):
		if p >= latitude_odd:
			return []
		return [st(t,p), st(t+1,p), st(t,p+1), st(t+1,p), st(t+1,p+1), st(t,p+1)]
	for phi_i in range(latitude_odd+1):
		phi_j,offz = (phi_i-1,-l2) if phi_i > latitude_half else (phi_i,l2)
		for theta_i in range(longitude):
			theta,phi = theta_i*2*pi/longitude,phi_j*pi/latitude
			waist = sin(phi)
			r = vec3(waist*cos(theta), -waist*sin(theta), cos(phi)) * radius
			r.z += offz
			v.append(r)
			i += capsule_triangles(theta_i,phi_i)
	return GfxMesh(quat(),vec3(),v,i),[PhysCapsule(quat(),vec3(),radius,length)]
