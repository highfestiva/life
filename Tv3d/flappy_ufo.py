#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Flappy birdish.

from trabant import *

# ASCII geometry.
ufoasc = r'''
 /X\
/XXX\
XXXXX
´XXX`
 ´X`
'''
column = 'X\n'*5

def init():
	global ufo,columns
	ufo = create_ascii_object(ufoasc, vel=(2,0,0))
	ufo.create_engine(push_engine)
	columns = []
	cam(target=ufo)

init()
while loop():
	ufo.engine[0].force((0, 0, 2 if taps() else -1))
	if timeout(3):
		columns += [create_ascii_object(column, pos=ufo.pos()+vec3(10,0,5))]
		columns += [create_ascii_object(column, pos=ufo.pos()+vec3(10,0,-5))]
		if len(columns) > 8:
			[c.release() for c in columns[:2]]
			columns = columns[2:]
	for ufo in collisions():
		explode(ufo.pos(),ufo.vel())
		for o in [ufo]+columns:
			o.release()
		sleep(5)
		init()
