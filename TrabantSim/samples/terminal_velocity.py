#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Terminal Velocity prototype, accelerometer controlled.

from trabant import *
from trabant.math import *
import math
from time import time

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
ship.orientation(quat().rotate_x(-pi/2))
pusher = ship.create_engine(push_engine, max_velocity=(1,0), sound=sound_engine_hizz)
yawer = ship.create_engine(push_turn_engine)
terrain_meshes,terrain_patch_size = {},100
cam(angle=(-pi/9,0,0), distance=50, target=ship, use_relative_angle=True)
fog(300)

def create_terrain_patch(patch_x,patch_y):
	x,y = patch_x*terrain_patch_size,patch_y*terrain_patch_size
	grid = 10
	g1,g2 = grid+1,grid//2
	s = terrain_patch_size//grid
	vertices = []
	for vy in range(0,grid+1):
		for vx in range(0,grid+1):
			lpos = vec3((vx-g2)*s, (vy-g2)*s, 0)
			wpos = vec3(x,y,0)+lpos
			rnd = wpos.y*27+wpos.x*13	# Pseudo random to give terrain some fluctuation.
			lpos.z = -65 + 25*sin2(wpos.x/73) + 21*sin2(wpos.y/123) + rnd//20%11	# Terrain well below Z=0.
			vertices.append(lpos)
	triangles = []
	for ty in range(0,grid):
		for tx in range(0,grid):
			triangles += [ty*g1+tx, ty*g1+tx+1, (ty+1)*g1+tx, ty*g1+tx+1, (ty+1)*g1+tx+1, (ty+1)*g1+tx]
	c = int(-cos(patch_y/9)*7+7)*16 + int(sin(patch_x/8)*7+7)*256
	c = '#%3.3x'%c
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
	update_terrain(ship.pos() + ship.orientation()*vec3(0,0,100))

	pusher.force(ship.orientation()*vec3(0,0,20))
	turnforce = -accelerometer().x
	yawer.force((0,0,turnforce))
