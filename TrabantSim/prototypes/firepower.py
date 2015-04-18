#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# A bit like Amiga Fire Power of 1987.

from trabant import *

# ASCII geometries.
turret = '\n---\n'.join([r'      /XX\ '] +
	                [r'XXXXXXXXX\ '] +
	                [r'      /XX\ '])
tank = '\n---\n'.join([r'/XXXXXXXXXXXX\ ']   +
                      [r'XXXXXXXXXXXXXX ']*3 +
                      [r'/XXXXXXXXXXXX\ '])

gravity((0,0,0), bounce=0.2)    # Create objects floating in mid-air.

# Create the objects and three wheels on each side.
turret,tank = [create_ascii_object(part,pos=pos,col='#282') for part,pos in [(turret,(-1,0,2)), (tank,(0,0,0))]]
[t.mass(m) for t,m in [(turret,500), (tank,8000)]]
wheel = lambda x,y: create_sphere(pos=(x,y,-0.5), radius=1.1, col='#151')
rwheels,lwheels = [wheel(-5,-3.7),wheel(0,-3.7),wheel(+5,-3.7)],[wheel(-5,+3.7),wheel(0,+3.7),wheel(+5,+3.7)]

# Connect turret and wheels to body.
tank.joint(hinge_joint, turret, (0,0,1))
[tank.joint(hinge_joint, wheel, axis=(0,-1,0)) for wheel in lwheels+rwheels]
turret_turn = tank.create_engine(roll_engine, targets=[(turret,1)])
lroll = tank.create_engine(roll_engine, targets=[(w,1) for w in lwheels])
rroll = tank.create_engine(roll_engine, targets=[(w,1) for w in rwheels], sound=sound_engine_combustion)

fg(outline=False)
cam(angle=(-pi/2,0,0), distance=80, target=tank, light_angle=(-pi/4,0,0))

terrain_meshes,patch_size = {},60

def create_terrain_patch(px,py):
    x,y = px*patch_size,py*patch_size
    ground = create_cube(pos=(x,y,-patch_size/2-2), side=patch_size, mat='smooth', col='#c93', static=True)
    house = create_cube(pos=(x,y+patch_size/4,2), side=(patch_size/2,12,4), mat='smooth', col='#274', static=True)
    return [ground]

def update_terrain(pos, create_one_at_a_time=True):
    '''Add new visible patches, drop no longer visible ones.'''
    global terrain_meshes
    old_meshes,created = dict(terrain_meshes),False
    top,left,s = int(pos.y)//patch_size-1,int(pos.x)//patch_size-1,3
    for y in reversed(range(top,top+s)):
        for x in range(left,left+s):
            key = y*3301919+x    # Anything large prime ok for hashing.
            if key in terrain_meshes:
                del old_meshes[key]
            elif not created:
                terrain_meshes[key],created = create_terrain_patch(x,y),create_one_at_a_time
    # Drop patches that have gone out of range.
    for key,meshes in old_meshes.items():
        del terrain_meshes[key]
        [mesh.release() for mesh in meshes]

update_terrain(vec3(),False)
gravity((0,0,-9))

while loop():
    tank.pos()
    update_terrain(tank.pos())
