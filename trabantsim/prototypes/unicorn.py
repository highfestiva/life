#!/usr/bin/env python3
# The ugliest unicorn in history was conceived with objects, rig and keyframes written in code.

from trabant import *

cam()

def tube(name, pos, ang, r, l):
    '''Create a tube, used as a body part.'''
    o = quat().rotate_y((90-ang)*pi/180)
    c = create_capsule(pos=pos,orientation=o,radius=r,length=l,static=True)
    c.name = name
    c.original_pos = vec3(*pos)
    c.original_offset = o * vec3(0,0,-l/2)
    c.original_orientation = quat()
    c.time_offset = 0
    c.children = []
    return c

# The body parts of a unicorn as a programmer sees it.
parts = [ tube('torso',   (+0.0,+0.0,+0.0),  0,1.0,3.0),
          tube('neck',    (+2.3,+0.0,+1.4), 45,0.6,1.0),
          tube('head',    (+3.5,+0.0,+2.4),  0,0.5,1.0),
          tube('horn',    (+3.3,+0.0,+2.9), 90,0.1,1.0),
          tube('tail',    (-3.0,+0.0,+1.4),135,0.3,1.7),
          tube('f-thigh', (+1.5,+0.5,-1.6),-90,0.3,1.0),
          tube('f-thigh', (+1.5,-0.5,-1.6),-90,0.3,1.0),
          tube('r-thigh', (-1.5,+0.5,-1.6),-90,0.4,1.0),
          tube('r-thigh', (-1.5,-0.5,-1.6),-90,0.4,1.0),
          tube('f-calf',  (+1.5,+0.5,-2.8),-90,0.2,0.6),
          tube('f-calf',  (+1.5,-0.5,-2.8),-90,0.2,0.6),
          tube('r-calf',  (-1.5,+0.5,-2.8),-90,0.2,0.6),
          tube('r-calf',  (-1.5,-0.5,-2.8),-90,0.2,0.6) ]

# Center torso.
parts[0].original_offset = vec3(0,0,0)

# Build hierarchy of parts.
parts[0].children = [parts[1]] + parts[4:9]
parts[1].children = [parts[2]] # neck -> head
parts[2].children = [parts[3]] # head -> horn
for i in range(5,9):
    parts[i].children = [parts[i+4]]

# Offset left and right thighs a little bit to make it look less atrocious.
parts[5].time_offset = 0.1
parts[8].time_offset = -0.1

# Animation keyframes is a list of angles and a list of body part indices.
keyframes = [ ([0,+10,0,-10], [ 0   ]),  # Torso wag.
              ([0,+20,0,-20], [ 1, 4]),  # Neck and tail moving in opposite direction of torso.
              ([0,-20,0,+20], [ 2   ]),  # Head.
              ([-30,0,+30,0], [ 5, 6]),  # Front thighs.
              ([+30,0,-30,0], [ 7, 8]),  # Rear thighs.
              ([5,-40,5,-20], [ 9,10]),  # Front calfs.
              ([40,20,50,20], [11,12]) ] # Rear calfs.

def rotate(part, center, angle):
    '''Rotate a part and its children.'''
    radians = angle*pi/-180
    part.anim_pos = (roty(radians) * (part.anim_pos - part.anim_offset - center)) + part.anim_offset + center
    part.anim_offset = roty(radians) * part.anim_offset
    part.anim_orientation = part.anim_orientation.rotate_y(radians)
    for child in part.children:
        rotate(child, center, angle)

while loop():
    # Reset animation of all parts.
    for part in parts:
        part.anim_pos = part.original_pos
        part.anim_offset = part.original_offset
        part.anim_orientation = part.original_orientation

    # Rotate all body parts according to the keyframes.
    for angles,indexes in keyframes:
        for part in [parts[i] for i in indexes]:
            t = (gametime() + part.time_offset) * len(angles) % len(angles)
            i,j = int(t),int(t+1)%len(angles)
            angle = lerp(angles[i], angles[j], t%1)
            rotate(part, part.anim_pos + part.anim_offset, angle)

    # Update all parts on screen.
    for part in parts:
        part.pos(part.anim_pos, orientation=part.anim_orientation)
