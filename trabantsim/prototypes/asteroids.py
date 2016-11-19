#!/usr/bin/env python3
# Asteroids-like prototype.

from trabant import *

# ASCII geometries. Ship is created in X and Z; the Y axis is depth.
shipasc = r'''
     /\
    /XX\
   /XXXX\
    X  X
'''
rockasc = '/X\nXX'    # Slightly irregular shape ftw.

create_rock = lambda p: create_ascii_object(rockasc, pos=p, vel=(rndvec()*3).with_y(0), avel=rndvec(), col='#777')

def init():
    global ship
    ship = create_ascii_object(shipasc, vel=(0,0,5), col='#fff')
    ship.push = ship.create_engine(push_rel_engine, strength=30)
    ship.push.addsound(sound_engine_hizz, intensity=0.5, volume=0.5)
    ship.turn = ship.create_engine(push_turn_abs_engine, friction=2, strength=30, max_velocity=4)
    cam(distance=100, target=ship)
    [create_rock((rndvec()*60).with_y(0)) for _ in range(10)]

gravity((0,0,0))
init()
while loop():
    shippos,shipvel = ship.pos(),ship.vel()

    # Steering.
    if any(tap.ispress for tap in taps()):
        angle = tapdir(shippos).angle_y(vec3(0,0,1))
        ship.orientation(roty(angle))
    is_moving = taps() or keydir().y>0
    ship.push.force((0,0,1 if is_moving else 0))
    ship.turn.force((0,keydir().x,0))

    if timeout(1): # Create a new rock every second.
        create_rock(shippos+shipvel.normalize()*80+(rndvec()*15).with_y(0))

    # Check if we crashed into something, if so explode.
    if ship in collided_objects():
        [e.force((0,0,0)) for e in ship.engines]
        explode(shippos,shipvel)
        sleep(1)
        release_objects()
        init()
