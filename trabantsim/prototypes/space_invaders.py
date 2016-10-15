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
 /X` ´` ´X\
 X`      ´X
'''

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
    # Steering.
    vel = keydir()*50 + tapdir(ship.pos())*4
    ship.vel((vel.x,0,0)) # Only move in X.

    # Shooting.
    is_tap_close = taps() and tapdir(ship.pos()).x < 3
    is_shooting = 'Space' in keys() or 'LCtrl' in keys() or is_tap_close
    if is_shooting and timeout(0.7, first_hit=True):
        shots += [create_sphere(ship.pos()+vec3(0,0,10), vel=(0,0,200), col='#fff')]
        sound(sound_bang, shots[-1].pos())

    # Run invaders.
    if timeout(3, timer='invaders'):
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
