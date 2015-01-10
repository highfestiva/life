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
	global ship,shipdir,rocks
	ship,shipdir = create_ascii_object(shipasc, vel=(0,0,5)),quat()
	ship.create_engine(push_engine, max_velocity=2, sound=sound_engine_hizz)
	cam(angle=(0,-pi/2,0), distance=100, target=ship)	# Rotated camera yields portrait layout for tablet.
	rocks = [create_rock((rndvec()*60).with_y(0)) for _ in range(5)]

gravity((0,0,0))
init()
while loop():
	shippos,shipvel = ship.pos(),ship.vel()
	if timeout(2):	# Create a new rock every two seconds.
		rocks += [create_rock(shippos+shipvel.normalize()*70+(rndvec()*7).with_y(0))]
	# Steering.
	if taps():
		angle = (closest_tap(shippos).pos3d()-shippos).angle_y(vec3(0,0,1))
		shipdir = quat().rotate_y(angle)
		ship.orientation(shipdir)
	ship.engine[0].force(shipdir*vec3(0,0,30 if taps() else 0))
	# Check if we crashed into something, if so explode.
	if ship in collisions():
		explode(shippos,shipvel)
		ship.release()
		[rock.release() for rock in rocks]
		sleep(3)
		init()
