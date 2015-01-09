#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# An RC car sim prototype using physical motors for control.

from trabant import *

# ASCII geometry for the car. An X is one unit wide, high and deep.
chassis = r'''
  /XX\
XXXXXXXXX
'''.strip('\n')
chassis = '---'.join([chassis]*4)	# Make the car a bit wider.

car = create_ascii_object(chassis, mass=500)
wheel = lambda x,y,z:create_sphere_object(pos=(x,y,z),radius=1.1,col='#222')
rr,rl,fr,fl = wheel(-4,-2.5,-0.7), wheel(-4,+2.5,-0.7), wheel(3,-2.5,-0.7), wheel(3,+2.5,-0.7)
car.create_joint(hinge2_joint, fl, (0,+1,0))
turn = [car.create_engine(roll_turn_engine)]
car.create_joint(hinge2_joint, fr, (0,-1,0))
turn += [car.create_engine(roll_turn_engine)]
car.create_joint(hinge_joint, rl, (0,+1,0))
roll = [car.create_engine(roll_engine, sound=sound_engine_combustion)]
car.create_joint(hinge_joint, rr, (0,-1,0))
roll += [car.create_engine(roll_engine)]

# Setup ground, gravity, camera angle and controls.
create_cube_object(pos=(0,0,-252), side=500, static=True)
gravity((0,0,-9))
cam(angle=(-0.3,0,0), distance=20, target=car)
left,right = create_joystick((0,1)),create_joystick((1,1))

while loop():
	[e.force(left.y) for e in roll]
	[e.force(right.x) for e in turn]
