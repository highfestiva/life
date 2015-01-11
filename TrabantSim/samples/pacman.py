#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# A Pacman-lookalike.

from trabant import *

# ASCII geometry.
level = r'''
XXXXXXXXXXXXXXXXXXXXXXXXXX
X         X   X          X
X         X   X          X
X         XX XX          X
X XXXXXXX        XXXXXXX X
X       X XX XX  X       X
XXXXX X X X   X  X X XXXXX
X   X X   XXXXX    X X   X
X   X   X       X    X   X
X   X XXXX     XXXX  X   X
X                        X
XXXXXXXXXXXXXXXXXXXXXXXXXX
'''.strip('\n')

open(addr='127.0.0.1:2541')
cam(distance=25)
gravity((0,0,0), bounce=0, friction=0)

userinfo('Please wait while loading PacMan level...')
level = create_ascii_object(level, pos=(-12.5,0,0), static=True)
userinfo()
man = create_sphere_object(radius=0.4, pos=(0,0,-3), col='#ff0')
man.create_engine(push_abs_engine)
directions = [vec3(1,0,0),vec3(0,0,1),vec3(-1,0,0),vec3(0,0,-1)]


while loop():
	if taps():
		force = closest_tap(man.pos()).pos3d() - man.pos()
		force.y = 0
		man.engine[0].force(force)
	else:
		man.engine[0].force((0,0,0))
