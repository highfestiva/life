#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# This prototype tests if right/left tapping of a screen-centered space ship is a good way to steer.

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
'''.strip('\n').replace('\n', '\n---\n')	# Transpose so Y becomes Z.
rockasc = 'X\\\nXX'	# Slightly irregular shape ftw.

def init():
	global ship,rocks
	ship = create_ascii_object(shipasc)
	ship.create_engine(push_engine, sound=sound_hizz)
	cam(angle=(-pi/2,0,-pi/2), dist=50, target=ship)	# Camera pointing down, initial "up" is to the left (i.e. iPad in portrait layout).
	rocks = []

shipdir = quat()
while loop():
	shippos,shipvel = ship.pos(),ship.vel()
	if timeout(2):	# Create a new rock every two seconds.
		rocks += [create_ascii_object(rockasc, pos=shippos+shipvel.normal()*70, angular_velocity=(0.3,0.5,0.7))]
	# Steering.
	engineforce = 0.1
	if tapping_left_of(shippos, shipdir*vec3(0,0,-1)):
		shipdir,engineforce = shipdir.rotate_z(-0.1),1
	if tapping_right_of(shippos, shipdir*vec3(0,0,-1)):
		shipdir,engineforce = shipdir.rotate_z(+0.1),1
	ship.pos(orientation=shipdir)
	ship.engine[0].force(shipdir*vec3(0,engineforce,0))
	# Check if we crashed into something, if so explode.
	if ship in collisions():
		explode(shippos,shipvel)
		ship.release()
		[rock.release() for rock in rocks]
		sleep(3)
		init()
