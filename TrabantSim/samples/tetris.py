#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Simple Tetris prototyping. Move left/right/down by tapping left/right/below tetromino. Rotate by tapping above.

from trabant import *
from trabant.math import *
import random

cam(angle=(0,-pi/2,0), distance=30)
gravity((0,0,0))

tetromino_lines = '''
XXXX  |  XX  |  X    |    X  |   X   |   XX  |  XX 
      |  XX  |  XXX  |  XXX  |  XXX  |  XX   |   XX
'''.strip('\n').split('\n')
tetromino_lines = [tl.split('  |  ') for tl in tetromino_lines]
tetrominos = ['\n'.join(t) for t in zip(*tetromino_lines)]
colors = '#0ff #ff0 #00f #a50 #f0f #0f0 #f00'.split()
directions = [vec3(1,0,0),vec3(0,0,1),vec3(-1,0,0),vec3(0,0,-1)]
gridsize = vec3(10,20,0)
tetromino_index,tetromino,tetromino_offset = None,None,None
fixed_blocks = [[None]*gridsize.x for _ in range(gridsize.y)]

create_cube_object(pos=(0,0,-gridsize.y/2-50), side=100, static=True)

while loop():
	if not tetromino:
		tetromino_index = random.choice(range(len(tetrominos)))
		tetromino = create_ascii_object(tetrominos[tetromino_index], pos=(0,0,gridsize.y/2), col=colors[tetromino_index])
		tetromino_offset = vec3(1.5,0,0) - last_ascii_top_left_offset()
		orientation = quat()#.rotate_x(pi/2)
		#tetromino.orientation(orientation)

	if taps() and timeout(0.15):	# Steering.
		pos = tetromino.pos() - tetromino_offset
		tap = closest_tap(pos)
		v = min(directions, key=lambda s:(tap.pos3d()-s-pos).length())
		if v == vec3(0,0,1):	# Tapping above means "rotate".
			orientation = quat().rotate_y(pi/2) * orientation
			tetromino.orientation(orientation)
		else:	# Tapping left/below/right means move in that direction.
			pos = rect_bound(pos+v, (-gridsize.x/2,0,-gridsize.y/2), (gridsize.x/2,0,gridsize.y/2))
			tetromino._p = pos + tetromino_offset
			tetromino.pos(tetromino._p)

	if timeout(1,timer=2):	# Move down.
		tetromino._p = tetromino.pos()+vec3(0,0,-1)
		tetromino.pos(tetromino._p)

	if collisions():
		# Drop tetromino, fixate position one step above (as if we collided by movement downwards).
		pos = tetromino._p - tetromino_offset + vec3(0,0,+1)
		tetromino.release()
		tetromino = None

		# Break tetromino into blocks. Place each block according to orientation.
		piece = tetrominos[tetromino_index].split('\n')
		for y,row in enumerate(piece):
			for x,block in enumerate(row):
				if block == 'X':
					p = intvec(orientation*vec3(x-1, y, 0) + pos)
					fixed_blocks[p.y+gridsize.y//2][p.x+gridsize.x//2] = create_ascii_object('X', pos=p, col=colors[tetromino_index], static=True)

		# Consume completed rows.
		ri = 0
		while ri < len(fixed_blocks):
			row = fixed_blocks[ri]
			if len([1 for block in row if block]) == gridsize.x:	# Complete row? Yes: shift down, then add empty row at top.
				[block.release() for block in row]
				for rj in range(ri+1,len(fixed_blocks)):
					fixed_blocks[rj-1] = fixed_blocks[rj]
				fixed_blocks[-1] = [None]*gridsize.x
				sound(sound_bang)
			else:
				ri += 1
