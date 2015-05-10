#!/usr/bin/env python3
# As simple as it gets: bounce the ball around.

from trabant import *

ball = create_sphere()

while loop():
    tap = closest_tap(ball.pos())
    if tap and (tap.pos3d()-ball.pos()).length() <= 1:
        ball.vel(tap.vel3d().limit(50))
        tap.invalidate()
    ball.bounce_in_rect((-5,0,-3),(5,0,3))
