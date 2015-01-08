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
columns = []

def init():
	global ufo,columns
	ufo = create_ascii_object(ufoasc, vel=(20,0,1))
	ufo.create_engine(push_engine)
	cam(target=ufo)
	columns += make_column_pair(-10,5)
	columns += make_column_pair(+3,5)
	columns += make_column_pair(+16,5)

def make_column_pair(dx, yoff):
	x = ufo.pos().x
	return	[create_ascii_object(column, pos=(x+dx,0,5+yoff), static=True),
		 create_ascii_object(column, pos=(x+dx,0,-5-yoff), static=True)]

cam(distance=30)
gravity((0,0,0))
init()
t = 0.5
while loop():
	ufo.engine[0].force((0, 1, 60 if taps() else -60))
	if timeout(t):
		columns += make_column_pair(20,t*10)
		while len(columns) > 8:
			[c.release() for c in columns[:2]]
			columns = columns[2:]
		t -= 0.01
	for ufo in collisions():
		explode(ufo.pos(),ufo.vel())
		for o in [ufo]+columns:
			o.release()
		sleep(5)
		init()
