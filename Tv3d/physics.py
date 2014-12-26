#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Just some pieces to play with.

from trabant import *
from trabant.math import *

gravity((0,0,-9))
create_static_cube(pos=(0,0,-105), size=200)
geometry = 'XX'	# ASCII geometry.
things = [create_ascii_object(geometry, pos=randvec()*5+vec3(0,0,5), col=absrandvec()) for _ in range(10)]

while loop():
	for thing in things:
		tap = closest_tap(thing.pos())
		if tap and (tap.pos3d(10)-thing.pos()).length() <= 3:
			thing.vel(tap.vel3d(10))
			break
