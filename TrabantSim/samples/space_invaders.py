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

cam(distance=250)
gravity((0,0,0))

ship = create_ascii_object(ship, pos=(0,0,-100), col='#070')
shots = []
invaderspeeds,isi = [(25,0,0), (0,0,-10), (-25,0,0), (0,0,-10)],0
invaders = set()
for y in range(2):
	for x in range(8):
		invaders.add(create_ascii_object(invader, pos=(x*25-130,0,100-y*20), col=absrndvec(), physmesh=True))
for invader in invaders:
	invader.vel(invaderspeeds[0])

while loop():
	if taps():
		tx = closest_tap(ship.pos()).pos3d().x
		sx = ship.pos().x
		d = abs(sx-tx)
		vmin = min(25,d*3)*4
		ship.vel((vmin if sx<tx else -vmin, 0, 0))
		if d < 5 and timeout(0.7):
			shots += [create_sphere_object(ship.pos()+vec3(0,0,10), vel=(0,0,350), col='#fff')]
			sound(sound_bang, shots[-1].pos())
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
