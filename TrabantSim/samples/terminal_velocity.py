#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Terminal Velocity prototype, accelerometer controlled.

from trabant import *
from trabant.math import *
import math

# ASCII geometries.
shipasc = r'''
    /\
    XX
    XX
XX  XX  XX
´XXXXXXXX`
 ´XXXXXX`
'''.strip('\n')

gravity((0,0,0))
ship = create_ascii_object(shipasc)
ship.create_engine(push_engine, sound=sound_engine_hizz)
terrain_meshes,terrain_patch_size = {},100
zangle = 0
cam(angle=(-pi/6,0,0), distance=50, target=ship)
fog(350)

def create_terrain_patch(patch_x,patch_y):
	x,y = patch_x*terrain_patch_size,patch_y*terrain_patch_size
	s = terrain_patch_size//10
	vertices = []
	for vy in range(0,11):
		for vx in range(0,11):
			lpos = vec3((vx-5)*s, (vy-5)*s, 0)
			wpos = vec3(x,y,0)+lpos
			rnd = wpos.y*27+wpos.x*13	# Pseudo random to give terrain some fluctuation.
			lpos.z = -65 + 25*sin2(wpos.x/73) + 21*sin2(wpos.y/123) + rnd//20%11	# Terrain well below Z=0.
			vertices.append(lpos)
	triangles = []
	for ty in range(0,10):
		for tx in range(0,10):
			triangles += [ty*11+tx, ty*11+tx+1, (ty+1)*11+tx, ty*11+tx+1, (ty+1)*11+tx+1, (ty+1)*11+tx]
	c = int(-cos(patch_y/9)*7+7)*16 + int(sin(patch_x/8)*7+7)*256
	c = '#%3.3x'%c
	#print(c)
	return create_mesh_object(vertices, triangles, pos=(x,y,0), col=c, static=True)

def update_terrain(pos):
	'''Add new visible patches, drop no longer visible ones.'''
	global terrain_meshes
	meshes,created_count = {},0
	top,left = int(pos.y)//terrain_patch_size-2,int(pos.x)//terrain_patch_size-2
	for y in range(top,top+5):
		for x in range(left,left+5):
			key = y*3301919+x	# Anything large prime is good enough.
			if key in terrain_meshes:
				meshes[key] = terrain_meshes[key]
				del terrain_meshes[key]
			elif created_count < 1:
				meshes[key] = create_terrain_patch(x,y)
				created_count += 1
				#print('creating terrain patch at %g,%g. Ship pos=%s, vel=%s.' % (x,y,str(ship.pos()),str(ship.vel())))
	# Drop out of range terrain patches.
	for mesh in terrain_meshes.values():
		mesh.release()
	terrain_meshes = meshes

while loop():
	zangle += -accelerometer().x / 50
	orientation = quat().rotate_z(zangle)
	update_terrain(ship.pos() + orientation*vec3(0,100,0))
	z2y = quat().rotate_x(-pi/2)	# Point ship forward instead of upward
	ship.orientation(z2y * orientation)
	ship.vel(orientation*vec3(0,20,0))
	#cam(angle=(-pi/6,0,zangle))
