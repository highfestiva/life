#!/usr/bin/env python3
# Generate a fern recursively.

from trabant import *
from trabant.objects import nophys

bg(col='#44d')

# Turn off waiting for each object to be created. After we enable asynchroneous load, creation of
# objects (branches and leaves) becomes a fire-and-forget operation, making it a *lot* faster than
# if we'd have to wait for each load to finish.
async_load()

# Sets the number of vertices to use in each capsule: along circumference and end-caps. Since it's
# only two vertices "wide," this type of capsule will be flat.
min_capsule_resolution = lambda radius,length:(2,2)
def create_fern_part(pos, orientation, radius, length):
    '''Creates a flat, green hexagon.'''
    create_capsule(pos, orientation, radius=radius, length=length, \
            col='#5d5', mat='flat', resolution=min_capsule_resolution, process=nophys)

def generate_fern(pos, angle, delta_angle, length, subtype='trunk'):
    if length < (0.1 if subtype=='trunk' else 0.02): # Branches grow thinner than the trunk.
        return
    orientation = quat().rotate_y(angle*pi/180).rotate_x(abs(angle)*0.3*pi/180) # X-rotation bends fern towards camera.
    v = orientation * vec3(0,0,length)
    create_fern_part(pos+v*0.5, orientation, radius=length/10, length=length)
    if subtype == 'trunk':
        # One branch in each direction.
        generate_fern(pos+v*0.3, angle+80, -abs(delta_angle)*1.5, length**1.2/3, 'branch')
        generate_fern(pos+v*0.7, angle-80, +abs(delta_angle)*1.5, length**1.2/3, 'branch')
        # Continue the trunk upwards.
        generate_fern(pos+v, angle+delta_angle, delta_angle, length*0.85, 'trunk')
    elif subtype == 'branch':
        # One leaf in each direction.
        leaf_length = length*0.7
        orientation = quat().rotate_y((angle+90)*pi/180).rotate_x(angle*0.2*pi/180) # X-rotation tilt leaves slightly upwards.
        u = orientation * vec3(0,0,leaf_length)
        create_fern_part(pos+v*0.8+u, orientation, radius=leaf_length/2, length=leaf_length)
        create_fern_part(pos+v*0.8-u, orientation, radius=leaf_length/2, length=leaf_length)
        # Continue branch.
        generate_fern(pos+v, angle+delta_angle, delta_angle, length*0.9, 'branch')

generate_fern(pos=vec3(0,0,-4), angle=-20, delta_angle=+2, length=1.3)

while loop():
    cam(angle=gametime())
