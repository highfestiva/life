#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Quake prototype, mainly intended for computers.

from trabant import *
from trabant.objects import orthoscale

# ASCII geometries.
floorasc = r'''

XXXX      XXXX
XXXX      XXXX
XXXX      XXXX
XXXX      XXXX


--------------
    X    X
XXXXX    XXXXX




XXXXX    XXXXX
    X    X
--------------
     XXXX
     XXXX

      XX
      XX

     XXXX
     XXXX
      XX
      XX
      XX
      XX
      ´`
'''

# Helper functions.
rotx = lambda a: quat().rotate_x(a)
roty = lambda a: quat().rotate_y(a)
rotz = lambda a: quat().rotate_z(a)

bg = vec3(0.4,0.8,1)
bgcol(bg)
gravity((0,0,-9), friction=0, bounce=0.1)
floor = create_ascii_object(floorasc, orientation=rotx(-pi/2), mat='checker', static=True, process=orthoscale((3,3,1)))
player = create_sphere_object((0,0,1), radius=0.5, col='#00f')
player.create_engine(walk_abs_engine, strength=30, max_velocity=150, friction=30)
cam(distance=0, fov=60, target=player, target_relative_angle=True, light_angle=(-pi/2,0,0))
bot = create_sphere_object((30,0,1), radius=0.5, col='#f00')
bot.create_engine(walk_abs_engine, strength=30, max_velocity=150, friction=30)

# Create walls and pillars.
levelcenter = vec3(15,0,0)
symmetry = [(rotz(0.2),vec3(1,1,1)), (rotz(-0.2),vec3(-1,1,1)), (rotz(-0.2),vec3(1,-1,1)), (rotz(0.2),vec3(-1,-1,1))]
longwalls = [vec3(-10.5,10.5,0),(18,1,4)]
[create_cube_object(longwalls[0].mulvec(sym)+levelcenter, side=longwalls[1], orientation=orientation, mat='flat', col='#943', static=True) for orientation,sym in symmetry]
symmetry = [vec3(1,1,1), vec3(-1,1,1), vec3(1,-1,1), vec3(-1,-1,1)]
[create_cube_object(vec3(-2.7,2.7,0).mulvec(sym)+levelcenter, side=(0.3,0.3,3), mat='flat', col=rndvec().abs(), static=True) for sym in symmetry]

yaw,pitch = -pi/2,0	# Start looking to the right, towards the center of the map.
grenades = []

while loop():
	# Update mouse look angles.
	yaw,pitch = yaw-mousemove().x*0.09, pitch-mousemove().y*0.05
	pitch = max(min(pitch,pi/2),-pi/2)	# Allowed to look straight up and straight down, but no further.

	# XY movement relative to the current yaw angle, jumps are controlled with Z velocity.
	xyrot = rotz(yaw)
	player.engine[0].force(xyrot * keydir().with_z(0))
	if keydir().z>0 and timeout(0.3, first_hit=True):
		player.vel(player.vel()+vec3(0,0,5))

	# Look around.
	orientation = xyrot.rotate_x(pitch)
	player.orientation(orientation)
	player.avel((0,0,0))	# Angular velocity. Makes sure the player object doesn't start rotating for some reason.

	# Throw grenades.
	if clicks() and timeout(1, timer=2, first_hit=True):
		vel = player.vel()
		pos = player.pos() + vel*0.05 + orientation*vec3(0,1,0)
		vel = vel + orientation*vec3(0,8,0)
		grenades += [create_sphere_object(pos=pos, vel=vel, radius=0.03, col='#3a3')]

	# Check if rocket exploded or if a player touched ground.
	for obj,_,_,pos in collisions():
		if obj in grenades:
			explode(pos, vel=(0,0,7), strength=0.3)
			obj.release()
			grenades.remove(obj)

	# Remove missed grenades.
	for g in list(grenades):
		if g.pos().z < -100:
			g.release()
			grenades.remove(g)

	# Respawn if fell down.
	pos = player.pos()
	if pos.z < -5:	# Fade to black.
		bgcol(bg*-5/pos.z)
		pitch += 0.1	# Look up towards the level you're falling down from.
	if pos.z < -50:
		sound(sound_clank, pos)
		player.vel((0,0,0))
		player.pos((0,0,1))
		bgcol(bg)
		yaw,pitch = -pi/2,0
	if bot.pos().z < -50:
		bot.vel((0,0,0))
		bot.pos((30,0,1))
