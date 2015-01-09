#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# I made this test when fixing a bug in the orientation handling.
# When you realize how objects are created through ASCII, you'll quickly grasp the rest.

from trabant import *
from trabant.gameapi import set

shipasc = r'''
     /\
    /XX\
   /XXXX\
  /XXXXXX\
 /XXXXXXXX\
   XX  XX
'''.strip('\n')

diamond_with_bom_hook = r'''
 XX\
 /XX\
 ´XX`
  ´`
---
 X
'''.strip('\n')

square_with_diamond = r'''
   ^
   ´>

XX
-----
   ^
 XXX>
 XXX
 XXX
'''.strip('\n')

gravity((0,0,0))
debug()
cam(distance=50)

create_ascii_object(shipasc, pos=(-10,-10,10), col='#f00', static=True)
create_ascii_object(diamond_with_bom_hook, pos=(+10,-10,10), col='#f00', static=True)
create_ascii_object(square_with_diamond, pos=(-10,-10,-10), col='#f00', static=True)
create_ascii_object(diamond_with_bom_hook, pos=(+10,-10,-10), col='#f00', static=True, physmesh=True)
create_ascii_object(shipasc, pos=(-10,+10,10), col='#0f0')
create_ascii_object(diamond_with_bom_hook, pos=(+10,+10,10), col='#0f0')
create_ascii_object(square_with_diamond, pos=(-10,+10,-10), col='#0f0')
create_ascii_object(diamond_with_bom_hook, pos=(+10,+10,-10), col='#0f0', physmesh=True)
ball = create_sphere_object(pos=(0,0,-500), radius=0.5, mass=1)

a = 0
while loop():
	cam(angle=(0,0,a))
	a += 0.01

	if taps():
		tap = taps()[0]
		pos = tap.pos3d(1)
		vel = (tap.pos3d(5) - pos)*5
		ball.pos(pos)
		ball.vel(vel, avel=(0,0,0))
