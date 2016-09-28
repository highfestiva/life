#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# An RC car sim prototype using physical motors for control.

from trabant import *
from trabant.objects import gfx_ortho_pinch

# ASCII geometry for the car. An X is one unit wide, high and deep.
chassis = r'''
    XX\
XXXXXXXXX
XXXXXXXXX
'''
chassis = '\n---\n'.join([chassis]*3)    # Make the car a bit wider.

bg(col='#5af')
# Create objects floating in mid-air, or joints will be off when things start falling down before they're bolted in.
gravity((0,0,0), friction=1)
collisions(False)
ground_patches = {}

def create_brake_lights(car, pos):
    car._brake_lights = [create_box(vec3(*pos)+vec3(*v), side=0.5, col='#300', mat='flat') for v in [(-4.9,+1.2,-0.3),(-4.9,-1.2,-0.3)]]
    car.joint(fixed_joint, car._brake_lights[0])
    car.joint(fixed_joint, car._brake_lights[1])
    car.brake_lights = lambda c: [bl.col(c) for bl in car._brake_lights]

def create_wheels(car, pos):
    wheel = lambda x,y,z: create_sphere(pos=vec3(*pos)+vec3(x,y,z), col='#111', process=gfx_ortho_pinch(0,1,0, func=lambda a:abs(sin(a)**3)*0.5-0.7), resolution=lambda radius:[x*r for x,r in zip([1,2],sphere_resolution(radius))])
    rr,rl,fr,fl = wheel(-3.5,-1.8,-1.5), wheel(-3.5,+1.8,-1.5), wheel(2.6,-1.6,-1.5), wheel(2.6,+1.6,-1.5)
    suspension_spring,suspension_damping = 0.1,2
    car.joint(turn_hinge_joint, fl, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    car.joint(turn_hinge_joint, fr, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    car.joint(suspend_hinge_joint, rl, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    car.joint(suspend_hinge_joint, rr, axis=(0,-1,0), spring=(suspension_spring,suspension_damping))
    return rr,rl,fr,fl

def create_car(pos, col):
    car = create_ascii_object(chassis, pos=vec3(*pos), mass=200, col=col)
    create_brake_lights(car, pos)
    rr,rl,fr,fl = create_wheels(car, pos)
    car.turn = car.create_engine(roll_turn_engine, targets=[(fl,1),(fr,1)])
    car.roll = car.create_engine(roll_engine, targets=[(rl,1),(rr,1)], strength=0.4, sound=sound_engine_combustion)
    car.brake = car.create_engine(roll_brake_engine, targets=[(rl,-1),(rr,-1),(fl,1),(fr,1)], strength=0.2, sound=sound_engine_combustion)
    car.create_engine(upright_stabilize)
    return car

def create_ground_patch(x, y):
    ground = create_box((x+35,y+35,-50-2.3), side=100, static=True)
    house = create_box((x+50,y+50,20-2.3), side=(70,70,40), col=rndvec().abs(), mat='flat', static=True)
    return [ground,house]

def update_terrain(pos):
    cx,cy = int((pos.x-50)/100), int((pos.y-50)/100)
    remove_patches = set(ground_patches.keys())
    for yo in [0,1,-1,2,-2,3,-3]:
        for xo in [0,1,-1,2,-2,3,-3]:
            if abs(yo)>=2 and abs(xo)>=2:
                continue # Just do two corridors: north-south and east-west.
            x = cx + xo
            y = cy + yo
            patch_id = y*757+x
            if patch_id not in ground_patches:
                ground_patches[patch_id] = create_ground_patch(x*100,y*100)
                return
            else:
                remove_patches.remove(patch_id)
    for patch_id in remove_patches:
        [o.release() for o in ground_patches[patch_id]]
        del ground_patches[patch_id]

me = create_car((30,0,0), '#cc5')
ai_cars = create_car((100,0,0), '#b11'),create_car((-40,0,0), '#11b')
[update_terrain(vec3(30,0,0)) for _ in range(4)]

# Setup ground, gravity, camera angle and controls.
collisions(True)
gravity((0,0,-9))    # Allow objects to fall down now that they're attached by joints.
camang = vec3(-0.4,0,+0.2-pi/2)
cam(angle=camang, target=me, pos=(0,3,0), target_relative_angle=True, smooth=0.8, light_angle=(-1.3,0,0.1))
left_stick,right_stick = create_joystick((0,0)),create_joystick((1,0))

def ai_reverse_due_to_obstruction(ai_car):
    obstructed = ai_car.vel().length2() < 1 and timeout(2, 'obstruction'+str(ai_car))
    if obstructed:
        timein_restart('reverse'+str(ai_car)) # Start backing up.
    if timein(1, 'reverse'+str(ai_car), auto_start=False):
        timeout_restart('obstruction'+str(ai_car)) # We're not obstructed while we reverse.
        return True
    return False

def move_ai(ai_car):
    v = (me.pos()+me.vel() - ai_car.pos()).normalize()
    forward = ai_car.orientation() * vec3(1,0,0)
    should_go_fwd = (v * forward > 0)
    if ai_reverse_due_to_obstruction(ai_car):
        should_go_fwd = not should_go_fwd
    ai_car.roll.force(1 if should_go_fwd else -1)
    angle = -forward.angle_z(v)
    ai_car.turn.force(angle if should_go_fwd else -angle/abs(angle))
    got_lost = ai_car.pos().z < -1  # Fell down when ground patch was taken away underneath it.
    if got_lost and timeout(1):
        userinfo('Got away from one!')
        place_ai_car(ai_car)

def place_ai_car(ai_car):
    ai_car.vel((0,0,0))
    ahead_of_me = me.pos() + me.vel().with_z(0).normalize(100)
    closest_intersection = vec3(*[round(v/100)*100 for v in ahead_of_me])
    if abs(closest_intersection.x-ahead_of_me.x) < abs(closest_intersection.y-ahead_of_me.y):
        ahead_of_me.x = closest_intersection.x
    else:
        ahead_of_me.y = closest_intersection.y
    ai_car.pos(ahead_of_me)

while loop():
    control_fwd = left_stick.y + keydir().y
    me.roll.force(control_fwd)
    me.turn.force(right_stick.x + keydir().x)
    moving_forward = (me.vel() * (me.orientation()*vec3(1,0,0)) > -0.1)
    if moving_forward and control_fwd < 0:
        me.brake.force(control_fwd)
        me.brake_lights('#f22f8')
    else:
        me.brake.force(0)
        me.brake_lights('#300')
    look_forward = moving_forward ^ ('Space' in keys())
    cam(angle=camang if look_forward else camang+vec3(0,0,pi*0.9))

    update_terrain(me.pos())

    [move_ai(car) for car in ai_cars]
