#!/usr/bin/env python3
# 3D cubes demo.

from trabant import *

box_template = create_box(static=True)
create_clones(box_template, ((rndvec()*7, rndquat()) for _ in range(200)), static=True)

mm = vec3()
while loop():
	t = gametime()
	mm += mousemove() * 0.1
	cam(distance=30-20*cos(t+mm.x), \
		angle=(sin(t*0.3),sin(t*0.27),sin(t*0.26+mm.y)))
