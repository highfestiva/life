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
'''

ship = create_ascii_object(shipasc)
ship.orientation(quat().rotate_x(-pi/2))	# Point ship forward instead of upward. Ship-relative coordinate system hereafter is +Y down, +Z in.
pusher = ship.create_engine(push_rel_engine, strength=30, friction=0.7)
pusher.addsound(sound_engine_hizz, intensity=0.5, volume=0.5)
pusher.force((0,0,1))	# Always full throttle.
yawer = ship.create_engine(push_turn_abs_engine, friction=0.5)
roller = ship.create_engine(push_turn_rel_engine, friction=0.5)	# Handles both banking and levelling ship.

terrain_meshes,patch_size = {},120

bgcol('#aaa')
gravity((0,0,0))
cam(angle=(pi/2.5,0,0), distance=80, target=ship, target_relative_angle=True)
fog(350,430)

def create_terrain_patch(px,py):
	x,y,grid = px*patch_size,py*patch_size,6
	g1,g2,s = grid+1,grid//2,patch_size//grid
	vertices,triangles = [],[]
	for v in range(0,grid+1):
		for u in range(0,grid+1):
			lpos = vec3((u-g2)*s, (v-g2)*s, 0)
			wpos = vec3(x,y,0)+lpos
			rnd = wpos.y*29+wpos.x*13	# Pseudo random to give terrain some fluctuation.
			lpos.z = -54 + 25*sin2(wpos.x/73) + 21*sin2(wpos.y/123) + rnd//20%11	# Terrain mostly below Z=0.
			vertices.append(lpos)
			if v < grid and u < grid:
				triangles += [v*g1+u, v*g1+u+1, (v+1)*g1+u, v*g1+u+1, (v+1)*g1+u+1, (v+1)*g1+u]
	col = int(-cos(py/9)*80+160)*256 + int(sin(px/8)*80+160)*65536	# Vary the patch color a little.
	return create_mesh_object(vertices, triangles, pos=(x,y,0), col='#%6.6x'%col, static=True)

def update_terrain(pos):
	'''Add new visible patches, drop no longer visible ones.'''
	global terrain_meshes
	old_meshes,created = dict(terrain_meshes),False
	top,left,s = int(pos.y)//patch_size-3,int(pos.x)//patch_size-3,7
	for y in reversed(range(top,top+s)):
		for x in range(left,left+s):
			key = y*3301919+x	# Anything large prime ok for hashing.
			if key in terrain_meshes:
				del old_meshes[key]
			elif not created:
				terrain_meshes[key],created = create_terrain_patch(x,y),True
	# Drop patches that have gone out of range.
	for key,mesh in old_meshes.items():
		del terrain_meshes[key]
		mesh.release()

while loop():
	acc,orientation = accelerometer(relative=True),ship.orientation()

	update_terrain(ship.pos() + orientation*vec3(0,0,100))

	# Turn ship.
	yaw_force = acc.roll*6 - sum(t.x*10-5 for t in taps()) - 5*keydir().x	# Control by either accelerometer, tapping or keyboard.
	yawer.force((0,0,yaw_force))

	# Banking and nose.
	roll_force = -yaw_force*1.3 + (orientation*vec3(1,0,0)).z*10	# Banking.
	pitch_factor = (orientation*vec3(0,0,1)).z*10 + ship.pos().z/10	# Level ship at around Z=0.
	pitch_force = acc.pitch*6 - sum(t.y*20-10 for t in taps()) - 3*keydir().y - pitch_factor	# Control nose by either accelerometer, tapping or keyboard.
	roller.force((pitch_force,0,roll_force))
