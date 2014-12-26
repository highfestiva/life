#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Snake prototype.

from trabant import *

steps = [vec3(1,0,0),vec3(0,0,1),vec3(-1,0,0),vec3(0,0,-1)]
pos,step = vec3(0,0,0),steps[0]
block = lambda p: create_ascii_object('X', pos=p)
blocks,worm = [block(pos)],[pos]
length = 5

cam(distance=25)

while loop():
	sleep(1)
	if len(worm) >= length:	# Remove last block when moving forward.
		blocks[-1].release()
		blocks,worm = blocks[:-1],worm[:-1]
	if taps():
		step = min(steps, key=lambda s:(closest_tap(pos).pos3d(25)+s-pos).length())
	pos += step
	if pos in worm:	# Collide with block?
		explosion(pos)
		while blocks:	# Eat up worm from back to front.
			blocks[-1].release()
			blocks,worm = blocks[:-1],worm[:-1]
			sleep(0.4)
	blocks += [block(pos)]
	worm += [pos]
	if timeout(3):	# Every few seconds the worm gets longer.
		length += 1
