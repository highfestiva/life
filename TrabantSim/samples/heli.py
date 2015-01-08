#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# An RC heli sim prototype, using physical motors for control.

from trabant import *
from trabant.math import *

# ASCII geometries. An X is one unit wide, high and deep. This is going to be a very flat helicopter.
main_rotor,tail_rotor,body = [part.strip('\n') for part in r'''
XXXXXXXXXXX
~~~
XXX
~~~
    /XXX\
/XXXXXXXXX\
XXXXXXXXXXXXXXXXXXXXXXXX
´XXXXXXXXX`
'''.split('~~~')]

# Combine parts into helicopter.
main_rotor,tail_rotor,body = [create_ascii_object(part,col=col) for part,col in [(main_rotor,'#444'), (tail_rotor,'#444'), (body,'#623')]]
main_rotor.pos((1,0,3.5))
main_rotor.mass(0.5)
body.create_joint(hinge_joint, main_rotor, (0,0,1))
thrust = []	# Collective and throttle engines go here
thrust += [body.create_engine(gyro_engine, max_velocity=(100,40), sound=sound_engine_hizz)]
thrust += [body.create_engine(rotor_engine)]
roll = body.create_engine(tilt_engine, offset=(0,-1))
pitch = body.create_engine(tilt_engine, offset=(3,0))
tail_rotor.pos((16,-1,-0.1))
tail_rotor.mass(0.25)
body.create_joint(hinge_joint, tail_rotor, (0,-1,0))
thrust += [body.create_engine(rotor_engine, max_velocity=(70,70))]
yaw = body.create_engine(collective_engine)

# Setup ground, gravity, camera angle and controls.
create_cube_object(pos=(0,0,-252), side=500, static=True)
gravity((0,0,-9))
cam(angle=(-0.3,0,0), distance=40, target=body)
left,right = create_joystick((0,1)),create_joystick((1,1))

while loop():
	[e.force(left.y) for e in thrust]
	yaw.force(left.x)
	pitch.force(right.y)
	roll.force(right.x)
