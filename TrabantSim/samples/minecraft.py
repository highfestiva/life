#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Minecraft prototype, mainly intended for computers.

from trabant import *

bg(col='#9df')
gravity((0,0,-15), friction=0, bounce=0.1)

player = create_capsule((0,0,15), col='#00f0')	# Alpha=0 means invisible object.
player.create_engine(walk_abs_engine, strength=20, max_velocity=2)
cam_offset = vec3(0,0,0.5)	# Keep the camera in eye level, same as in the original Minecraft.
cam(distance=0, fov=60, pos=cam_offset, target=player, target_relative_angle=True)

fg(col='#975', outline=False)	# Brownish floor.
cube = create_cube(pos=(1e5,1e5,1e5), side=1, mat='smooth', static=True)	# Template.
flooring_coords = [(vec3(x,y,0),quat()) for y in range(-8,8+1) for x in range(-8,8+1)]
create_clones(cube, flooring_coords, static=True)
fg(col='#3a4')	# Build in green color.

yaw,pitch = 0,-pi*0.4
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
	cam(angle=(pitch,0,yaw))
	player.avel((0,0,0))	# Angular velocity. Makes sure the player object doesn't start rotating for some reason.
	player.orientation(quat())	# Keep player object straight at all times.

	# Build/destroy blocks or pick color.
	if click() and timeout(0.2, timer=2, first_hit=True):
		orientation = xyrot.rotate_x(pitch)
		os = [(o,p) for o,p in pick_objects(player.pos()+cam_offset, orientation*vec3(0,1,0), 0,6) if o!=player]
		if os:
			obj,pos = os[0]	# Pick nearest cube.
			if click(left=True):
				obj.release()
			elif click(right=True):
				cubepos = obj.pos()
				create_clones(cube, [(toaxis(pos-cubepos)+cubepos,quat())], static=True)
			elif click(middle=True):
				fg(col=obj.col())

	# Change color.
	if mousewheel():
		fg(col=rndvec().abs())

	# Respawn if fell down.
	if player.pos().z < -30:
		player.vel((0,0,0))
		player.pos((0,0,30))
		yaw,pitch = 0,-pi*0.4
