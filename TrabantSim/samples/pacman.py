#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# A Pacman-lookalike.

from trabant import *

# ASCII geometry.
level = r'''
XXXXXXXXXXXXXXXXXXXXXXXXXX
X         X   X          X
X XXXXXXX        XXXXXXX X
X       X XX XX  X       X
XXXXX X X X   X  X X XXXXX
X   X X   XXXXX    X X   X
X   X   X       X    X   X
X   X XXXX     XXXX  X   X
X                        X
XXXXXXXXXXXXXXXXXXXXXXXXXX
'''.strip('\n')

cam(distance=100)

level = create_ascii_object(level)
man = create_sphere_object(radius=0.8)
man.create_engine(push_engine)
directions = [vec3(1,0,0),vec3(0,0,1),vec3(-1,0,0),vec3(0,0,-1)]

while loop():
	if taps():
		pos = man.pos()
		tap = closest_tap(pos)
		force = min(directions, key=lambda d:(tap.pos3d()-pos).length())
		man.engine[0].force(force)
	else:
		man.engine[0].force((0,0,0))
