#!/usr/bin/env python3
# Crossy Road prototype.

from trabant import *
from random import randint, choice

lanes,cars,avatar = [],[],None
gravity((0,0,-20), friction=0) # No friction, so the "cars" can move freely.
async_load() # So game won't "freeze" during creation of lanes, cars, etc.
tap_directions = [vec3(0,+1,0),vec3(0,-1,0),vec3(-1,0,0),vec3(+1,0,0)] # Forward, backward, left, right.

def create_lane(y, name):
    speed = 0
    if name == 'grass':
        # Create a grassy lane with some trees on.
        l = [create_box((0,y,0), side=(30,1,1), col='#0a0', mat='noise', static=True)]
        if y >= 3: # Not in the first few lanes of the level.
            for _ in range(randint(0,5)):
                # Create a random number of trees of random height.
                x = randint(-5,5)
                z = random()/2
                l += [create_box((x,y,1.5+z/2), side=(0.8,0.8,1+z), col='#040', mat='flat', static=True)]
                l += [create_box((x,y,1), side=(0.6,0.6,1), col='#630', mat='noise', static=True)]
    elif name == 'road':
        # Just a gray slab. Each road lane has a given speed which the "cars" on it will follow.
        name = 'road_left' if random() < 0.5 else 'road_right'
        l = [create_box((0,y,0), side=(30,1,1), col='#555', mat='flat', static=True)]
        speed = 3 + random()*2
    elif name == 'pavement':
        # Just a striped lane with nothing on.
        l = [create_box((0,y,0), side=(30,1,1), static=True)]
    l[0].lane_type = name
    l[0].lane_speed = speed
    l[0].lane_y = y
    l[0].lane_interval = 1.8 + random()*4
    return l

def fill_lanes():
    global lanes
    if not lanes[-1][0].isloaded:
        return
    # Create lanes at the top of the screen, remove when they disappear at the bottom.
    ay = avatar.pos().y
    last_lane_y = int(lanes[-1][0].pos().y)
    if ay+7 >= last_lane_y:
        last_lane_y += 1
        lanes += [create_lane(last_lane_y, choice('grass grass road pavement'.split()))]
    elif len(lanes) > 13:
        for o in lanes[0]:
            o.release()
        del lanes[0]

def init():
    global lanes, cars, avatar
    release_objects()
    lanes = [create_lane(y, 'grass') for y in range(-5,3)]
    cars = []
    avatar = wait_load(create_box((0,0,1), side=0.5, col='#ff0', mat='flat'))
    avatar.create_engine(stabilize)
    avatar.adjusted = False
    fill_lanes()
    cam(distance=6, angle=(-1,0,0.3), target=avatar, pos=(0,1,0), smooth=0.5, light_angle=(-0.8,0,1.3))

init()

while loop():
    avatar_on_ground = avatar in collided_objects()
    if avatar_on_ground:
        # Each time the avatar skips one jump ahead, we just stop it as it lands (preventing it from
        # sliding away) and place it on the center of the closest coordinate.
        if not avatar.adjusted:
            avatar.vel((0,0,0))
            p = avatar.pos()
            avatar.pos((int(round(p.x)), int(round(p.y)), 0.75))
            avatar.adjusted = True
            keys()
        else:
            if keydir().length2() or taps():
                pos = avatar.pos()
                godir = keydir() + tapdir(pos, digital_direction=True)
                vel = 2.2
                if godir.x > 0 and pos.x < +4:
                    avatar.vel((+vel,0,4))
                elif godir.x < 0 and pos.x > -4:
                    avatar.vel((-vel,0,4))
                elif godir.y < 0:
                    avatar.vel((0,-vel,4))
                elif godir.y or godir.z:
                    avatar.vel((0,+vel,4))
                def unadjusted(avatar):
                    if avatar:
                        avatar.adjusted = False
                timer_callback(0.1, lambda: unadjusted(avatar))

    for lane_objs in lanes:
        lane = lane_objs[0]
        if 'road' in lane.lane_type and timeout(lane.lane_interval, lane.lane_y):
                if 'left' in lane.lane_type:
                    cars += [create_box((+10,lane.lane_y,1), side=(2,0.8,1), vel=(-lane.lane_speed,0,0), col=rndvec().abs(), mat='flat')]
                else:
                    cars += [create_box((-10,lane.lane_y,1), side=(2,0.8,1), vel=(+lane.lane_speed,0,0), col=rndvec().abs(), mat='flat')]

    carset = set(cars)
    for o1,o2,_,_ in collisions():
        if o1 == avatar and o2 in carset:
            avatar.scale((1,1,0.2))
            avatar = None
            userinfo('Splat!')
            sound(sound_bang)
            timer_callback(3, init)
            break

    if cars and cars[0].pos().z < 0:
        cars[0].release()
        del cars[0]

    if avatar:
        fill_lanes()
