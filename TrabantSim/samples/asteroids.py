#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Asteroids-like prototype.

from trabant import *
from trabant.math import *

# ASCII geometries. Ship is created in X and Z; the Y axis is depth.
shipasc = r'''
     /\
    /XX\
   /XXXX\
    X  X
'''.strip('\n')
rockasc = 'X\\\nXX'	# Slightly irregular shape ftw.

create_rock = lambda p: create_ascii_object(rockasc, pos=p, vel=(rndvec()*3).with_y(0), avel=rndvec())

def init():
	global ship
	ship = create_ascii_object(shipasc, vel=(0,0,5))
	ship.create_engine(push_rel_engine, max_velocity=2, sound=sound_engine_hizz)
	cam(distance=100, target=ship)
	[create_rock((rndvec()*60).with_y(0)) for _ in range(10)]

gravity((0,0,0))
init()
while loop():
	shippos,shipvel = ship.pos(),ship.vel()
	if timeout(1):	# Create a new rock every second.
		create_rock(shippos+shipvel.normalize()*80+(rndvec()*15).with_y(0))
	# Steering.
	if taps():
		angle = (closest_tap(shippos).pos3d()-shippos).angle_y(vec3(0,0,1))
		ship.orientation(quat().rotate_y(angle))
	ship.engine[0].force((0,0,30 if taps() else 0))
	# Check if we crashed into something, if so explode.
	if ship in collided_objects():
		ship.engine[0].force((0,0,0))
		explode(shippos,shipvel)
		sleep(3)
		release_all_objects()
		init()
