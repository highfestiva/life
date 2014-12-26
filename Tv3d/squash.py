#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Squash pong-like prototype.

from trabant import *

# ASCII geometry.
ball = r'''
 ^
<X>
 v
'''.strip('\n')

gravity((0,0,-3))	# We're on a low gravity planet.
ball = create_sphere_object()
while loop():
	tap = closest_tap(ball.pos())
	if tap and (tap.pos3d(10)-ball.pos()).length() <= 2:
		ball.vel(drag.vel3d(10))
	ball.bounce_in_rect((-10,-3,-10),(10,3,10))
