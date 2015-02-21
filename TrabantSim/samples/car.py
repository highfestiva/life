#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# An RC car sim prototype using physical motors for control.

from trabant import *

# ASCII geometry for the car. An X is one unit wide, high and deep.
chassis = r'''
  /XX\
XXXXXXXXX
'''
chassis = '\n---\n'.join([chassis]*3)	# Make the car a bit wider.

bg(col='#5af')
gravity((0,0,0))	# Create objects floating in mid-air.

car = create_ascii_object(chassis, mass=500, col='#36a')
wheel = lambda x,y,z: create_sphere(pos=(x,y,z), radius=1.1, col='#654')
rr,rl,fr,fl = wheel(-2.8,-2.7,-1.2), wheel(-2.8,+2.7,-1.2), wheel(3.4,-2.7,-1.2), wheel(3.4,+2.7,-1.2)
car.joint(turn_hinge_joint, fl, axis=(0,-1,0))
car.joint(turn_hinge_joint, fr, axis=(0,-1,0))
car.joint(suspend_hinge_joint, rl, axis=(0,-1,0))
car.joint(suspend_hinge_joint, rr, axis=(0,-1,0))
turn = car.create_engine(roll_turn_engine, targets=[(fl,1),(fr,1)])
roll = car.create_engine(roll_engine, targets=[(rl,1),(rr,1)], strength=0.5, sound=sound_engine_combustion)

# Setup ground, gravity, camera angle and controls.
create_cube(pos=(0,0,-253), side=500, static=True)
gravity((0,0,-9))	# Allow objects to fall down now that they're attached by joints.
cam(angle=(-0.3,0,0), distance=20, target=car)
left_stick,right_stick = create_joystick((0,0)),create_joystick((1,0))

while loop():
	roll.force(left_stick.y + keydir().y)
	turn.force(right_stick.x + keydir().x)
