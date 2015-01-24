#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Snake prototype.

from trabant import *

cam(distance=60)

steps = [vec3(1,0,0),vec3(0,0,1),vec3(-1,0,0),vec3(0,0,-1)]
pos,step = vec3(),steps[0]
block = lambda p: create_ascii_object('X', pos=p, static=True)
blocks,snake_coords = [block(pos)],[pos]
length = 5

while loop():
	if taps():
		step = min(steps, key=lambda s:(closest_tap(pos).pos3d()-s-pos).length())
	elif keydir().x or keydir().y:
		step = vec3(keydir().x,0,keydir().y)
	pos += step
	if pos in snake_coords:	# Collide with block?
		explode(pos)
		while blocks:	# Eat up snake from back to front.
			blocks[-1].release()
			blocks,snake_coords = blocks[:-1],snake_coords[:-1]
			sleep(0.05)
		length = 5
	blocks = [block(pos)] + blocks
	snake_coords = [pos] + snake_coords
	if len(snake_coords) >= length:	# Remove last block when moving forward.
		blocks[-1].release()
		blocks,snake_coords = blocks[:-1],snake_coords[:-1]
	if timeout(1):	# Every second the snake gets longer.
		length += 1
