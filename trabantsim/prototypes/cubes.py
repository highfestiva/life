#!/usr/bin/env python3
# 3D cubes demo.

from trabant import *

box_template = create_box(static=True)
create_clones(box_template, ((rndvec()*7, rndquat()) for _ in range(200)), static=True)

mmove = vec3()
while loop():
    t = gametime()
    mmove += mousemove() * 0.1
    angle = (sin(t*0.3),sin(t*0.27),sin(t*0.26+mmove.y))
    cam(distance=30-20*cos(t+mmove.x), angle=angle)
