#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Flappy birdish.

from trabant import *
from math import sin

# ASCII geometry.
ufoasc = r'''
  X
XXXXX
 ´X`
'''
column = 'X\n'*5

def init():
	gravity((0,0,0))	# Suspend objects while initializing.
	global ufo,columns,t
	ufo = create_ascii_object(ufoasc, vel=(8,0,1), col='#a33')
	engine = ufo.create_engine(push_abs_engine, strength=0.8)
	engine.addsound(sound_engine_wobble, intensity=0.3, volume=1)
	engine.force((1,0,0))
	cam(target=ufo)
	columns = [y for x in range(-20,20,2) for y in make_column_pair(x,8)]
	t = 0.3
	gravity((0,0,-35))	# This is what I call gravity!

def make_column_pair(dx, yoff):
	x = ufo.pos().x
	y = sin(x/20)*8
	return	[create_ascii_object(column, pos=(x+dx,0,y+5+yoff), col='#ba0', static=True),
		 create_ascii_object(column, pos=(x+dx,0,y-5-yoff), col='#ba0', static=True)]

cam(distance=30)
init()
while loop():
	if taps() or keys():
		ufo.vel(ufo.vel().with_z(11))
		ufo.avel(rndvec()*0.1)
	if timeout(t):
		columns += make_column_pair(20,t*26)
		while columns[0].pos().x < ufo.pos().x-20:
			[c.release() for c in columns[:2]]
			columns = columns[2:]
		t -= 0.002
	if collided_objects():
		ufo.engine[0].force((0,0,0))
		explode(ufo.pos(),ufo.vel())
		sleep(1)
		release_all_objects()
		init()
