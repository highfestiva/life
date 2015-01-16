#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Breakout prototyping.

from math import pi
from trabant import *

# ASCII geometries.
paddle = r'''
 ^     ^
<XXXXXXX>
 v     v
'''.strip('\n')
brick = 'XX'

cam(angle=(0,-pi/2,0), distance=35)
gravity((0,0,0), bounce=1, friction=1)
paddle = create_ascii_object(paddle, pos=(0,0,-15), mass=10000)
ball = create_sphere_object(pos=(-7,0,10), vel=(10,0,-30), radius=0.5, col='#fff')
bricks = set()
for y in range(2):
	for x in range(8):
		bricks.add(create_ascii_object(brick, pos=(x*3-11.5,0,15-y*2), col=rndvec().abs(), static=True))

while loop():
	if taps():
		p = paddle.pos()
		v = ((closest_tap(p).pos3d().x-p.x)*6, 0, 0)
	else:
		v = (0,0,0)
	paddle.vel(v, avel=(0,0,0))
	ball.bounce_in_rect((-14,-0.1,-25), (14,0.1,17))
	if ball.pos().z < -17:
		explode(ball.pos(), ball.vel())
		ball.pos((-7,0,10))
		ball.vel((10,0,-30))
	# bv = ball.vel()
	# bv = ball.vel()
	# if bv.length() < 13 or abs(bv.z) < 7:
		# bv.y = 0
		# bv = bv.normalize(15)
		# bv.z *= 8/bv.z if abs(bv.z) < 7 else 1
		# ball.vel(bv)
	for o in collided_objects():
		if o in bricks:
			sound(sound_ping, o.pos())
			o.release()
			bricks.remove(o)
