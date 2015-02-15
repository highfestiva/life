#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Space Invadersishkebab.

from trabant import *

# ASCII geometries.
shipascii = r'''
    /\
/XXXXXXXX\
v        v
'''
invader = r'''
  /XXXXXX\
 /XXXXXXXX\
 XXXXXXXXXX
 XX  XX  XX
 ´XXXXXXXX`
  /XX  XX\
 /X` ´` `X\
 X`      `X
'''

fg(outline=False)
cam(distance=250)
gravity((0,0,0))

ship = create_ascii_object(shipascii, pos=(0,0,-100), col='#070')
shots = []
invaderspeeds,isi = [(25,0,0), (0,0,-10), (-25,0,0), (0,0,-10)],0
invaders = set()
for y in range(2):
	for x in range(8):
		invaders.add(create_ascii_object(invader, pos=(x*25-130,0,100-y*20), col=rndvec().abs(), physmesh=True))
for invader in invaders:
	invader.vel(invaderspeeds[0])

while loop():
	# Steering, shooting.
	tap_dx = 100
	if taps():
		dx = closest_tap(ship.pos()).pos3d().x - ship.pos().x
		tap_dx = abs(dx)
		vmin = min(25,tap_dx*3)*4
		ship.vel((vmin if dx>0 else -vmin, 0, 0))
	else:
		ship.vel((keydir().x*50,0,0))
	if ('SPACE' in keys() or tap_dx<5) and timeout(0.7):
		shots += [create_sphere_object(ship.pos()+vec3(0,0,10), vel=(0,0,200), col='#fff')]
		sound(sound_bang, shots[-1].pos())

	# Run invaders.
	if timeout(3,timer=2):
		isi = (isi+1)%len(invaderspeeds)
		[i.vel(invaderspeeds[isi]) for i in invaders]

	# Check collisions, make explosions.
	for o in collided_objects():
		if o in invaders:
			invaders.remove(o)
			explode(o.pos(),o.vel(),5)
		elif o == ship:
			while loop():
				pass
		o.release()
