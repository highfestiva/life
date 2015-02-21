#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Breakout prototyping.

from trabant import *

# ASCII geometries.
paddle = r'''
 ^     ^
<XXXXXXX>
 v     v
'''
brick = 'XX'

cam(angle=(0,-pi/2,0), distance=45)	# Flip camera for iPad portrait layout.
gravity((0,0,0), bounce=1, friction=1)
paddle = create_ascii_object(paddle, pos=(0,0,-15), mass=10000)
ball = create_sphere(pos=(-7,0,10), vel=(10,0,-30), radius=0.5, col='#fff')
bricks = set()
for y in range(2):
	for x in range(8):
		bricks.add(create_ascii_object(brick, pos=(x*3-11.5,0,15-y*2), col=rndvec().abs(), static=True))

while loop():
	if taps():
		p = paddle.pos()
		v = ((closest_tap(p).pos3d().x-p.x)*6 - keydir().y*20, 0, 0)
	else:
		v = (-keydir().y*20,0,0)
	paddle.vel(v, avel=(0,0,0))
	ball.bounce_in_rect((-14,-0.1,-25), (14,0.1,17))
	# Check if ball fell down below the paddle.
	if ball.pos().z < -17:
		explode(ball.pos(), ball.vel())
		ball.pos((-7,0,10))
		ball.vel((10,0,-30))
	# Play a clanking sound if we hit a brick.
	for o in collided_objects():
		if o in bricks:
			sound(sound_clank, o.pos())
			o.release()
			bricks.remove(o)
