#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# An RC car sim prototype using physical motors for control.

from trabant import *
from trabant.objects import gfx_ortho_pinch
from trabant.gameapi import cmd,setvar

# ASCII geometry for the car. An X is one unit wide, high and deep.
chassis = r'''
  /XX\
XXXXXXXXX
'''
chassis = '\n---\n'.join([chassis]*3)    # Make the car a bit wider.

bg(col='#5af')
gravity((0,0,0), friction=0.7) # Create objects floating in mid-air.
collisions(False)
setvar('Ui.3D.ShadowRange', 500.0)

def create_car(pos):
    car = create_ascii_object(chassis, pos=vec3(*pos), mass=500, col='#36a')
    wheel = lambda x,y,z: create_sphere(pos=vec3(*pos)+vec3(x,y,z), radius=1.1, col='#654', process=gfx_ortho_pinch(0,1,0, func=lambda a:abs(sin(a)**3)*0.3-0.9), resolution=lambda radius:[x*r for x,r in zip([1,2],sphere_resolution(radius))])
    rr,rl,fr,fl = wheel(-2.8,-2.2,-1.2), wheel(-2.8,+2.2,-1.2), wheel(3.4,-2.1,-1.2), wheel(3.4,+2.1,-1.2)
    car.joint(turn_hinge_joint, fl, axis=(0,-1,0))
    car.joint(turn_hinge_joint, fr, axis=(0,-1,0))
    car.joint(suspend_hinge_joint, rl, axis=(0,-1,0))
    car.joint(suspend_hinge_joint, rr, axis=(0,-1,0))
    car.turn = car.create_engine(roll_turn_engine, targets=[(fl,1),(fr,1)], max_velocity=(0.4,-0.4))
    car.roll = car.create_engine(roll_engine, targets=[(rl,1),(rr,1)], strength=0.3, sound=sound_engine_combustion)
    car.create_engine(upright_stabilize)
    return car

def create_ground_patch(x, y):
    x *= 200
    y *= 200
    create_box((x,y,-100-2.3), side=200, static=True)
    box = create_box((x+30,y+40,30-2.3), side=(40,40,60), static=True)
    box.shadow()
    return box

me = create_car((0,0,0))

p1 = create_ground_patch(0,0)
p2 = create_ground_patch(1,0)
#create_ground_patch(1,1)
#create_ground_patch(0,1)
    
# Setup ground, gravity, camera angle and controls.
collisions(True)
gravity((0,0,-9))    # Allow objects to fall down now that they're attached by joints.
camang = vec3(-0.4,0,-pi/2)
cam(angle=camang, distance=15, target=me, target_relative_angle=True, smooth=0.9)
left_stick,right_stick = create_joystick((0,0)),create_joystick((1,0))

while loop():
    me.roll.force(left_stick.y + keydir().y)
    me.turn.force(right_stick.x + keydir().x)
