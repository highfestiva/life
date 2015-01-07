#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Squash pong-like prototype.

from trabant import *

gravity((0,0,-3))	# We're on a low gravity planet.
ball = create_sphere_object()
while loop():
	tap = closest_tap(ball.pos())
	if tap and (tap.pos3d()-ball.pos()).length() <= 2:
		ball.vel(drag.vel3d())
	ball.bounce_in_rect((-10,-3,-10),(10,3,10))
