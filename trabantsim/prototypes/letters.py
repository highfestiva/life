#!/usr/bin/env python3
# A few boxes (arranged as text), which start falling down when hit by a ball.
# Every frame the position, orientation, velocity and angular velocity is recorded
# for each box, and after some time the reversed motion is played back.

from trabant import *

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
create_sphere((73,-1,0), vel=(-37,0.6,0.6), radius=1.2, mass=140)

rec = {o:[] for o in objs}
fwd = True

while loop():
    if timeout(5): # After some time we reverse the process.
        fwd = False
        # Convert all objects to triggers, so they won't collide when intersecting.
        [o.physics_type(trigger=True) for o in objs]
    for o in objs:
        if fwd:
            v = o.vel()
            rec[o] += [(o.pos(),v,o.avel(),o.orientation())]
            if v.length2(): # If object is moving we add gravity.
                o.vel(v-vec3(0,0,0.2))
        elif rec[o]:
            p,v,av,ori = rec[o][-1]
            del rec[o][-1]
            o.vel(-v, avel=-av)
            o.pos(p, orientation=ori)
