#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Just some pieces to play with.

from trabant import *
from trabant.math import *

cam(distance=25)
gravity((0,0,-9), bounce=0.3)
create_cube_object(pos=(0,0,-105), side=200, static=True)
geometry = 'XX'	# ASCII geometry.
things = [create_ascii_object(geometry, pos=rndvec()*5+vec3(0,0,5), vel=rndvec()*3, col=absrndvec()) for _ in range(10)]

while loop():
	if not taps():
		continue
	tap = taps()[0]
	thing = min(things, key=lambda t:(tap.pos3d(t.pos().y+25)-t.pos()).length())
	#thing.vel(tap.vel3d())
	thing.vel(rndvec()*2+vec3(0,0,12), avel=rndvec())
