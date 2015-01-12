#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Flappy birdish.

from trabant import *

# ASCII geometry.
ufoasc = r'''
  X
XXXXX
 ´X`
'''
column = 'X\n'*5

def init():
	global ufo,columns,t
	ufo = create_ascii_object(ufoasc, vel=(20,0,1), col='#a33')
	ufo.create_engine(push_abs_engine, sound=sound_engine_hizz).force((1,0,0))
	cam(target=ufo)
	columns = []
	columns += make_column_pair(-10,8)
	columns += make_column_pair(-3,8)
	columns += make_column_pair(+4,8)
	columns += make_column_pair(+11,8)
	t = 0.3

def make_column_pair(dx, yoff):
	x = ufo.pos().x
	return	[create_ascii_object(column, pos=(x+dx,0,5+yoff), col='#ba0', static=True),
		 create_ascii_object(column, pos=(x+dx,0,-5-yoff), col='#ba0', static=True)]

cam(distance=30)
gravity((0,0,-40))
init()
while loop(delay=0.03):
	for tap in taps():
		v = ufo.vel()
		v.z = min(v.z+15, 20)
		v.z = max(v.z, 5)
		ufo.vel(v)
	if timeout(t):
		columns += make_column_pair(18,t*26)
		if len(columns) > 14:
			[c.release() for c in columns[:2]]
			columns = columns[2:]
		t -= 0.002
	if collided_objects():
		ufo.engine[0].force((0,0,0))
		explode(ufo.pos(),ufo.vel())
		sleep(1)
		release_all_objects()
		init()
