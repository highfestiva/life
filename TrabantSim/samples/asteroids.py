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
  /XXXXXX\
 /XXXXXXXX\
   XX  XX
'''.strip('\n')
rockasc = 'X\\\nXX'	# Slightly irregular shape ftw.

def init():
	global ship,rocks
	ship = create_ascii_object(shipasc, vel=(0,0,10))
	ship.create_engine(push_engine, sound=sound_engine_hizz)
	cam(angle=(0,-pi/2,0), distance=200, target=ship)	# Camera pointing down, initial "up" is to the left (i.e. iPad in portrait layout).
	rocks = []

gravity((0,0,0))
init()
shipdir = quat()
while loop():
	shippos,shipvel = ship.pos(),ship.vel()
	if timeout(2):	# Create a new rock every two seconds.
		print('shipvel:', shipvel)
		rocks += [create_ascii_object(rockasc, pos=shippos+shipvel.normalize()*200, angular_velocity=(0.3,0.5,0.7))]
	# Steering.
	if taps():
		angle = (shippos-closest_tap(shippos).pos3d()).angle_y(vec3(0,0,1))
		shipdir = quat().rotate_y(angle)
	ship.orientation(shipdir)
	ship.engine[0].force(shipdir*vec3(0,0,1 if taps() else 0.1))
	# Check if we crashed into something, if so explode.
	if ship in collisions():
		explode(shippos,shipvel)
		ship.release()
		[rock.release() for rock in rocks]
		sleep(3)
		init()
