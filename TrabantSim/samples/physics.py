#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Just some pieces to play with.

from trabant import *

fg(outline=False)
camdistance = 25
cam(distance=camdistance)
gravity((0,0,-9), bounce=0.3)
create_cube(pos=(0,0,-105), side=200, static=True)
geometry = 'XX'    # ASCII geometry.
things = [create_ascii_object(geometry, pos=rndvec()*5+vec3(0,0,5), vel=rndvec()*3, col=rndvec().abs()) for _ in range(20)]
things += [create_sphere(pos=rndvec()*5+vec3(0,0,5), radius=random()*0.7+0.3, col=rndvec().abs()) for _ in range(5)]

while loop():
    if not taps():
        continue
    tap = taps()[0]
    thing = min(things, key=lambda t:(tap.pos3d(t.pos().y+camdistance)-t.pos()).length())
    pos = thing.pos()
    thing.pos(tap.pos3d(pos.y+camdistance).with_y(pos.y))
    thing.vel(tap.vel3d(pos.y+camdistance).with_y(0))
