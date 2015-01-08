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
'''.strip('\n').replace('\n', '\n---\n')	# Transpose so Z becomes Y.

ship,orientation = create_ascii_object(shipasc),quat()
ship.create_engine(push_engine, sound=sound_engine_hizz)
terrain_meshes,terrain_patch_size = {},100
zangle = 0
cam(angle=(-pi*2/5,0,0), distance=50, target=ship)
fog(200)

def create_terrain_patch(patch_x,patch_y):
	x,y = patch_x*terrain_patch_size,patch_y*terrain_patch_size
	s = terrain_patch_size//10
	vertices = []
	for vy in range(0,11):
		for vx in range(0,11):
			lpos = vec3((vx-5)*s, (vy-5)*s, 0)
			wpos = vec3(x+vx*s, y+vy*s, 0)
			rnd = int(wpos.y*29+wpos.x) * 214013 + 2531011	# Pseudo random to give terrain some fluctuation.
			lpos.z = 36 - (11*math.sin(wpos.x/17) + 19*math.sin(wpos.y/3) + rnd%1024/300);	# Terrain well below Z=0.
			vertices.append(lpos)
	triangles = []
	for ty in range(0,10):
		for tx in range(0,10):
			triangles += [ty*10+tx+1, ty*10+tx, (ty+1)*10+tx+1, (ty+1)*10+tx+1, ty*10+tx, (ty+1)*10+tx]
	return create_mesh_object(vertices, triangles, pos=(x+s*5,y+s*5,0), static=True)

def update_terrain(pos):
	'''Add new visible patches, drop no longer visible ones.'''
	global terrain_meshes
	meshes = {}
	top,left = int(pos.y)//terrain_patch_size-2,int(pos.x)//terrain_patch_size-2
	for y in range(top,top+5):
		for x in range(left,left+5):
			key = y*3301919+x	# Anything large prime is good.
			if key in terrain_meshes:
				meshes[key] = terrain_meshes[key]
				del terrain_meshes[key]
			else:
				meshes[key] = create_terrain_patch(x,y)
	# Drop out of range terrain patches.
	for mesh in terrain_meshes.values():
		mesh.release()
	terrain_meshes = meshes

while loop():
	update_terrain(ship.pos())
	zangle += -accelerometer().x / 10
	orientation = orientation.rotate_z(zangle)
	ship.pos(orientation=orientation)
	ship.vel(orientation*vec3(0,20,0))
	cam(angle=(-pi*2/5,0,zangle))
