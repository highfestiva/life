#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Descent-like prototype, tap/keyboard controlled.

from trabant import *

# ASCII geometries.
shipasc = \
r'''

   X  X
----------


  XXXXXX
 /`    ´\
----------
  /` ´\
  ´\ /`
  XXXXXX
 /`    ´\
/`      ´\
----------


    XX    

/`      ´\
----------


   XXXX
----------


    XX
'''

bg(col='#aaa')
gravity((0,0,0), bounce=1, friction=0)

ship = create_ascii_object(shipasc)
pusher = ship.create_engine(push_rel_engine, strength=20, friction=0.8)	# For forward acceleration.
pusher.addsound(sound_engine_hizz, intensity=0.2, volume=0.2)
steer = ship.create_engine(push_turn_rel_engine, friction=1)	# Handles pitch and roll.

cam(angle=(-0.2,0,0), distance=20, target=ship, target_relative_angle=True, smooth=0.9)
fog(230,350)

cube_side,grid_scale = 140,280
room_prio = [1,0,2,-1]		# Create center room first (1=center).
cube = create_cube(pos=(-1e5,-1e5,-1e5), side=cube_side, static=True)	# Create a cube far away to have something to clone.
# Rooms are lists of cubes. Cubes actualy constitutes half rooms. The remainder is made up by adjoining rooms.
# Rooms are hashed by their grid positions.
rooms = {}
# Helper functions.
rotx = lambda a: quat().rotate_x(a)
roty = lambda a: quat().rotate_y(a)
rotz = lambda a: quat().rotate_z(a)

def create_room(pos):
	'''Returns cube positions+sizes for half a room, i.e. three walls and three "gateways".'''
	wpos,cubepos,nozfight = pos*grid_scale,vec3(0,cube_side,cube_side*0.4),vec3(0.1,0.1,0.1)
	wall_orientations = [quat(), rotz(-pi/2), rotx(-pi/2)]
	side_orientations = [(roty(a),zfoff) for a,zfoff in [(0,vec3()),(pi/2,nozfight)]]
	wall_pieces = []
	for wo in wall_orientations:
		for so,zfoff in side_orientations:
			cpos = wo*so*cubepos+zfoff+wpos
			wall_pieces += [(cpos, quat())]
	return create_clones(cube, wall_pieces, static=True)

def update_level(pos, all_rooms=False):
	global rooms
	created,old_rooms = False,dict(rooms)
	# Create the grid (of coordinates and sizes for the wall pieces/cubes).
	left,front,top = [int(i)//grid_scale for i in pos]
	for z in [top+i for i in room_prio]:
		for y in [front+i for i in room_prio]:
			for x in [left+i for i in room_prio]:
				key = z*3301919+y*1783+x	# Anything large prime ok for hashing.
				if key in rooms:
					del old_rooms[key]
				elif not created:
					rooms[key],created = create_room(vec3(x,y,z)),True
	# Drop rooms that have gone out of range.
	for key,room in old_rooms.items():
		[wallpiece.release() for wallpiece in room]
		del rooms[key]

while loop():
	update_level(ship.pos())

	pusher.force((0,1,0) if taps() or 'LCTRL' in keys() else (0,0.1,0))

	# Turn ship.
	roll_force = sum(t.x*40-20 for t in taps()) + 20*keydir().x	# Control roll by either tapping or keyboard.
	pitch_force = -sum(t.y*80-40 for t in taps()) - 12*keydir().y	# Control pitch by either tapping or keyboard.
	steer.force((pitch_force,roll_force,0))
