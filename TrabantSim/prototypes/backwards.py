#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from trabant import *
from trabant.gameapi import setvar

text = '''
xxx
  x  x  xx   xx xxx
x x x x x x x x  x
 x   x  x x  xx xxx'''

gravity((0,0,0))
cam(distance=30)
pos = []
for y,l in enumerate(text.split('\n')):
    for x,ch in enumerate(l):
        pos += [(x-10,0,-y)] if ch!=' ' else []
objs = [create_box(p) for p in pos]
create_sphere((50,-40,-5), vel=(-30,27,1), mass=80)

rec = [[] for _ in objs]
fwd = True

while loop():
    if timeout(6):
        fwd = False
        setvar('Physics.NoClip', True)
    for i,o in enumerate(objs):
        if fwd:
            v = o.vel()
            rec[i] += [(o.pos(),v,o.avel(),o.orientation())]
            if v.length2() > 0.001:
                o.vel(v-vec3(0,0,0.3))
        else:
            if rec[i]:
                p,v,av,ori = rec[i][-1]
                del rec[i][-1]
                o.pos(p, orientation=ori)
                o.vel(-v, avel=-av)
