#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Asteroids-like prototype.

from trabant import *
from trabant.math import *

# ASCII geometries. Ship is created in X and Z; the Y axis is height.
shipasc = r'''
     /\
    /XX\
   /XXXX\
  /XXXXXX\
 /XXXXXXXX\
   XX  XX
'''.strip('\n').replace('\n', '\n---\n')	# Transpose so Z becomes Y.
rockasc = 'X\\\nXX'	# Slightly irregular shape ftw.

def init():
	global ship,rocks
	ship = create_ascii_object(shipasc)
	ship.create_engine(push_engine, sound=sound_engine_hizz)
	cam(angle=(-pi/2,0,-pi/2), distance=50, target=ship)	# Camera pointing down, initial "up" is to the left (i.e. iPad in portrait layout).
	rocks = []

init()
shipdir = quat()
while loop() or True:
	shippos,shipvel = ship.pos(),ship.vel()
	if timeout(2):	# Create a new rock every two seconds.
		rocks += [create_ascii_object(rockasc, pos=shippos+shipvel.normal()*70, angular_velocity=(0.3,0.5,0.7))]
	# Steering.
	if taps():
		angle = (shippos-closest_tap(shippos).pos3d(50)).angle_z(vec3(0,1,0))
		shipdir = quat().rotate_z(angle)
	ship.pos(orientation=shipdir)
	ship.engine[0].force(shipdir*vec3(0,1 if taps() else 0.1,0))
	# Check if we crashed into something, if so explode.
	if ship in collisions():
		explode(shippos,shipvel)
		ship.release()
		[rock.release() for rock in rocks]
		sleep(3)
		init()
