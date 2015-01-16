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
XXXXXXXXXXXXXXXXXXXX
´XXXXXXXXX`
'''.split('~~~')]
near,middle,far = [body]*3	# Make the body a bit wider.
near,far = [s.replace('XXXXXXXXX\n', '\n') for s in [near,far]]	# Only keep tail in the middle.
body = near+'\n---\n'+middle+'\n---\n'+far

gravity((0,0,0))	# Create objects afloat in space.

# Create and place helicopter parts.
main_rotor,tail_rotor,body = [create_ascii_object(part,pos=pos,col=col) for part,pos,col in [(main_rotor,(-0.2,0,3),'#444'), (tail_rotor,(12,1.2,-0.1),'#444'), (body,(0,0,0),'#a23')]]

# Main rotor.
main_rotor.mass(0.5)
body.create_joint(hinge_joint, main_rotor, (0,0,1))
thrust = body.create_engine(gyro_engine, sound=sound_engine_rotor, strength=2, max_velocity=(50,10))
lift = body.create_engine(rotor_engine, strength=0.2)
roll = body.create_engine(tilt_engine, offset=(0,1), strength=0.2)
pitch = body.create_engine(tilt_engine, offset=(1,0), strength=0.2)

# Tail rotor.
tail_rotor.mass(0.25)
body.create_joint(hinge_joint, tail_rotor, (0,-1,0))
body.create_engine(gyro_engine, max_velocity=(20,20), strength=0.1)	# No controls necessary as it always spins up to operational speed.
yaw = body.create_engine(rotor_engine, strength=0.1)

# Stabilize the chopper, or it will be a nightmare to fly.
body.add_stabilizer(3)

# Setup ground, gravity, camera angle and controls.
create_cube_object(pos=(0,0,-353), side=700, static=True)
gravity((0,0,-9))
cam(angle=(-0.3,0,1), distance=40, target=body)
left,right = create_joystick((0,0),sloppy=True),create_joystick((1,0),sloppy=True)

while loop():
	thrust.force(left.y)
	lift.force(left.y if left.y>0 else 0)
	yaw.force(left.x)
	pitch.force(right.y)
	roll.force(right.x)
