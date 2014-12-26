#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Space Invadersishkebab.

from trabant import *

# ASCII geometries.
ship = r'''
    /\
/XXXXXXXX\
v        v
'''.strip('\n')
invader = r'''
  /XXXXXX\
 /XXXXXXXX\
 XXXXXXXXXX
 XX  XX  XX
 ´XXXXXXXX`
  /XX  XX\
 /X` ´` `X\
 X`      `X
'''.strip('\n')

ship = create_ascii_object(ship, pos=(0,0,-100))
shots = []
invaderspeeds,isi = [(0.5,0,0), (0,0,-0.1), (-0.5,0,0), (0,0,-0.1)],0
invaders = set()
for y in range(2):
	for x in range(8):
		invaders.add(create_ascii_object(invader, pos=(x*50-50,90-y*20,0), vel=invaderspeeds[0], col=absrndvec())

cam(distance=200)

while loop():
	if taps():
		tx = closest_taps(ship.pos()).pos3d(200).x
		sx = ship.pos().x
		ship.vel((1 if sx<tx else -1, 0, 0))
		if abs(sx-tx) < 0.5 and timeout(1):
			shots += [create_sphere_object(sx+vec3(0,0,4), r=2, vel(0,0,3))]
	else:
		ship.vel((0,0,0))
	if timeout(3,timer=2):
		isi = (isi+1)%len(invaderspeeds)
		[i.vel(invaderspeeds[isi]) for i in invaders]
	for o in collisions():
		if o in invaders:
			invaders.remove(o)
		explode(o.pos(),o.vel())
		o.release()
