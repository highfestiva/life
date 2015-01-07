#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Breakout prototyping.

from trabant import *

# ASCII geometries.
paddle = r'''
 ^         ^
<XXXXXXXXXXX>
 v         v
'''.strip('\n')
brick = 'XX'

paddle = create_ascii_object(paddle, pos=(0,0,-10))
ball = create_sphere_object(vel=(2,2,0))
bricks = set()
for y in range(2):
	for x in range(8):
		bricks.add(create_ascii_object(brick, pos=(x*3-11.5,9-y*2,0), col=absrndvec()))

while loop():
	if taps():
		paddle.pos((closest_tap(ball.pos()).pos3d().x, 0, -10))
	ball.bounce_in_rect((-10,-0.5,-12), (10,0.5,10))
	for o in collisions():
		if o in bricks:
			sound(sound_ping, o.pos())
			o.release()
			bricks.remove(o)
