#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Minecraft prototype, mainly intended for computers.

from trabant import *

bg(col='#9df')
fg(col='#975', outline=False)
gravity((0,0,-15), friction=0, bounce=0.1)

cube = create_cube_object(pos=(1e5,1e5,1e5), side=1, mat='smooth', static=True)	# Template.
flooring_coords = [(vec3(x,y,0),quat()) for y in range(-8,8+1) for x in range(-8,8+1)]
create_clones(cube, flooring_coords, static=True)

player = create_sphere_object((0,0,2), col='#00f0')	# Alpha=0 means invisible object.
player.create_engine(walk_abs_engine, strength=30, max_velocity=70, friction=30)
cam(distance=0, fov=60, target=player, target_relative_angle=True)
fg(col='#3a4')

yaw,pitch = 0,0
while loop():
	# Update mouse look angles.
	yaw,pitch = yaw-mousemove().x*0.09, pitch-mousemove().y*0.05
	pitch = max(min(pitch,pi/2),-pi/2)	# Allowed to look straight up and straight down, but no further.

	# XY movement relative to the current yaw angle, jumps are controlled with Z velocity.
	xyrot = quat().rotate_z(yaw)
	player.engine[0].force(xyrot * keydir().with_z(0))
	if keydir().z>0 and timeout(1, first_hit=True):
		player.vel(player.vel()+vec3(0,0,6))

	# Look around.
	orientation = xyrot.rotate_x(pitch)
	player.orientation(orientation)
	player.avel((0,0,0))	# Angular velocity. Makes sure the player object doesn't start rotating for some reason.

	# Build/destroy blocks.
	if (rightclick() or clicks()) and timeout(0.3, timer=2, first_hit=True):
		os = [(o,p) for o,p in pick_objects(player.pos(), orientation*vec3(0,1,0), 0,6) if o!=player]
		if os:
			obj,pos = os[0]	# Pick nearest cube.
			if rightclick():
				cubepos = obj.pos()
				create_clones(cube, [(toaxis(pos-cubepos)+cubepos,quat())], static=True)
			else:
				obj.release()

	# Respawn if fell down.
	if player.pos().z < -30:
		player.vel((0,0,0))
		player.pos((0,0,30))
		yaw,pitch = 0,0
