#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Squash pong-like prototype.

from trabant import *

cam(distance=30)
gravity((0,0,-9), bounce=1)
ball = create_sphere_object()
while loop():
	tap = closest_tap(ball.pos())
	if tap and (tap.pos3d(ball.pos().y)-ball.pos()).length() <= 2:
		ball.vel(tap.vel3d())
	ball.bounce_in_rect((-10,-3,-10),(10,3,10))
