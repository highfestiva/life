#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from trabant import *
from trabant.gameapi import setvar,waitload

text = '''
xxxx
   x
   x  xx  xxx   xxx    xx
   x x  x x  x x   x  x
x  x x  x x  x x   x   x
x  x x  x x  x x  xx x  x
 xx   xx  x  x  xx x  xx'''

gravity((0,0,0))
cam(distance=30)
pos = [(x-12,0,5-y) for y,l in enumerate(text.split('\n')) for x,ch in enumerate(l) if ch!=' ']
objs = create_clones(create_box((0,-100,-100)), zip(pos,[quat()]*len(pos)))
#for o in objs: waitload(o.id)
create_sphere((73,-1,0), vel=(-37,0.6,0.6), radius=1.2, mass=140)

rec = {o:[] for o in objs}
fwd = True

while loop():
    if timeout(5):
        fwd = False
        setvar('Physics.NoClip', True)
    for o in objs:
        if fwd:
            v = o.vel()
            rec[o] += [(o.pos(),v,o.avel(),o.orientation())]
            if v.length2():
                o.vel(v-vec3(0,0,0.2))
        elif rec[o]:
            p,v,av,ori = rec[o][-1]
            del rec[o][-1]
            o.vel(-v, avel=-av)
            o.pos(p, orientation=ori)
