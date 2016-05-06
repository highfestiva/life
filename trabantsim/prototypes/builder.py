#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Just some pieces to play with.

from trabant import *

grav = 0
objs = []
poss = []

def record_pos():
    global poss
    poss = []
    for o in objs:
        poss += [o.pos()]

def reset_pos():
    global objs
    for o in objs[len(poss):]:
        o.release()
    objs = objs[:len(poss)]
    for i,o in enumerate(objs):
        o.vel(vec3(), avel=vec3())
        o.pos(poss[i], orientation=quat())

def toggle_gravity():
    global grav
    if grav:
        record_pos()
    gravity((0,0,grav))
    collisions(grav!=0)
    grav = 0 if grav else -9.8
    if grav:
        reset_pos()

cam(angle=(-0.7,0,0), distance=60, fov=60)
toggle_gravity()

while loop():
    if not taps():
        if 'SPACE' in keys():
            toggle_gravity()
            while keys():
                loop()
        continue

    obj = None
    while taps():
        tap = taps()[0]
        poke_vector = tap.pos3d(1) - cam()
        poke_vector = cam() + poke_vector * (cam().z / -poke_vector.z)
        if not obj:
            import random
            if random.random() < 0.333:
                obj = create_box(pos=poke_vector, side=5)
            elif random.random() < 0.5:
                obj = create_sphere(pos=poke_vector, radius=2.5)
            else:
                obj = create_capsule(pos=poke_vector, radius=2, length=4)
            objs += [obj]
        else:
            obj.pos(poke_vector)
        print(cam(), tap.pos3d(1), poke_vector)
        loop()
