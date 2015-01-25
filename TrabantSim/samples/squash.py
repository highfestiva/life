#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Squash pong-like prototype.

from trabant import *

cam(distance=30)
ball = create_sphere_object()

while loop(delay=0.01):	# Need low latencies to be able to get accurate tap velocities.
	tap = closest_tap(ball.pos())
	if tap and (tap.pos3d(ball.pos().y)-ball.pos()).length() <= 3:
		ball.vel(tap.vel3d().limit(50))
		tap.invalidate()
	ball.bounce_in_rect((-17,0,-10),(17,0,10))
