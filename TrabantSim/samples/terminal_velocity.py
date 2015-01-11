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
ship.orientation(quat().rotate_x(-pi/2))	# Point ship forward instead of upward. Ship-relative coordinate system hereafter is +Y down, +Z in.
pusher = ship.create_engine(push_rel_engine, max_velocity=1, friction=0.5, sound=sound_engine_hizz)
pusher.force((0,0,30))	# Always full throttle.
yawer = ship.create_engine(push_abs_turn_engine, friction=0.5)
roller = ship.create_engine(push_rel_turn_engine, friction=0.5)	# Handles both banking and levelling ship.

terrain_meshes,terrain_patch_size = {},120

cam(angle=(pi/2.5,0,0), distance=80, target=ship, use_relative_angle=True)
fog(80)

def create_terrain_patch(patch_x,patch_y):
	x,y = patch_x*terrain_patch_size,patch_y*terrain_patch_size
	grid = 6
	g1,g2 = grid+1,grid//2
	s = terrain_patch_size//grid
	vertices = []
	for vy in range(0,grid+1):
		for vx in range(0,grid+1):
			lpos = vec3((vx-g2)*s, (vy-g2)*s, 0)
			wpos = vec3(x,y,0)+lpos
			rnd = wpos.y*29+wpos.x*13	# Pseudo random to give terrain some fluctuation.
			lpos.z = -60 + 25*sin2(wpos.x/73) + 21*sin2(wpos.y/123) + rnd//20%11	# Terrain below Z=0.
			vertices.append(lpos)
	triangles = []
	for ty in range(0,grid):
		for tx in range(0,grid):
			triangles += [ty*g1+tx, ty*g1+tx+1, (ty+1)*g1+tx, ty*g1+tx+1, (ty+1)*g1+tx+1, (ty+1)*g1+tx]
	col = int(-cos(patch_y/9)*80+160)*256 + int(sin(patch_x/8)*80+160)*65536	# Vary the patch color a little.
	return create_mesh_object(vertices, triangles, pos=(x,y,0), col='#%6.6x'%col, static=True)

def update_terrain(pos):
	'''Add new visible patches, drop no longer visible ones.'''
	global terrain_meshes
	meshes,created_count = {},0
	top,left = int(pos.y)//terrain_patch_size-3,int(pos.x)//terrain_patch_size-3
	for y in reversed(range(top,top+7)):
		for x in range(left,left+7):
			key = y*3301919+x	# Anything large prime is good enough.
			if key in terrain_meshes:
				meshes[key] = terrain_meshes[key]
				del terrain_meshes[key]
			elif created_count < 1:
				meshes[key] = create_terrain_patch(x,y)
				created_count += 1
	# Drop out of range terrain patches.
	for mesh in terrain_meshes.values():
		mesh.release()
	terrain_meshes = meshes

while loop():
	p,o = ship.pos(),ship.orientation()

	update_terrain(p + o*vec3(0,0,100))

	# Turn ship.
	yaw_force = -accelerometer().x - sum(t.x*6-3 for t in taps())
	yawer.force((0,0,yaw_force))

	# Bank ship, and adjust nose up/down levelling.
	roll_factor = -(o*vec3(1,0,0)).z*10
	roll_force = -yaw_force*1.3 - roll_factor	# Banking.
	pitch_force = -(o*vec3(0,0,1)).z*10 - ship.pos().z/10	# Try to level ship at Z=0.
	roller.force((pitch_force,0,roll_force))
