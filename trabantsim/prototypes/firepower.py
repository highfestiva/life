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

gravity((0,0,0), bounce=0.2, friction=3)    # Create objects floating in mid-air. Use high friction to simulate tank moment better.

# Create the objects and three wheels on each side.
collisions(False)   # Disallow collisions while we're mounting wheels inside tank.
turret,tank = [create_ascii_object(part,pos=pos,col='#282') for part,pos in [(turret,(0.5,0,1.5)), (tank,(0,0,0))]]
[t.mass(m) for t,m in [(turret,500), (tank,8000)]]
wheel = lambda x,y: create_sphere(pos=(x,y,-0.5), radius=1.1, col='#531')
rwheels,lwheels = [wheel(-5,-3),wheel(0,-3),wheel(+5,-3)],[wheel(-5,+3),wheel(0,+3),wheel(+5,+3)]

# Connect turret and wheels to body.
tank.joint(hinge_joint, turret, (0,0,1))
[tank.joint(hinge_joint, wheel, axis=(0,-1,0)) for wheel in lwheels+rwheels]
turret_turn = tank.create_engine(roll_engine, targets=[(turret,1)], max_velocity=[6,-6], strength=0.1, friction=1)
lroll = tank.create_engine(roll_engine, max_velocity=[15,-15], targets=[(w,1) for w in lwheels], friction=0.1)
rroll = tank.create_engine(roll_engine, max_velocity=[15,-15], targets=[(w,1) for w in rwheels], friction=0.1)
rroll.addsound(sound_engine_combustion, 0.2, volume=50000)
collisions(True)   # Wheels mounted, allow collisions again.

# Aiming stuff for turret.
target_pos,reset_turret,shots = None,False,[]

# We could use some flat terrain to drive on.
terrain_meshes,patch_size = {},80

def create_terrain_patch(px,py):
    x,y = px*patch_size,py*patch_size
    ground = create_box(pos=(x,y,-patch_size/2-2), side=patch_size, mat='noise', col='#c93', static=True)
    house = create_box(pos=(x,y+patch_size/4,2-2), side=(patch_size/2,12,4), mat='flat', col='#274', static=True)
    house2 = create_box(pos=(x-patch_size/4,y-patch_size/4,8-2), side=(10,10,16), mat='flat', col='#ddf', static=True)
    return [ground, house, house2]

def update_terrain(pos, create_one_patch_at_a_time=True):
    '''Add new visible patches, drop no longer visible ones.'''
    global terrain_meshes
    old_meshes,created = dict(terrain_meshes),False
    top,left,s = int(pos.y+patch_size/2)//patch_size-1,int(pos.x+patch_size/2)//patch_size-1,3
    for y in reversed(range(top,top+s)):
        for x in range(left,left+s):
            key = y*3301919+x    # Anything large prime ok for hashing.
            if key in terrain_meshes:
                del old_meshes[key]
            elif not created:
                terrain_meshes[key],created = create_terrain_patch(x,y),create_one_patch_at_a_time
    # Drop patches that have gone out of range.
    for key,meshes in old_meshes.items():
        del terrain_meshes[key]
        [mesh.release() for mesh in meshes]

update_terrain(vec3(),False)
gravity((0,0,-9))
cam(angle=(-pi/2,0,0), distance=80, target=tank, light_angle=(-pi/4,0,0))
stick = create_joystick((0,0))

while loop():
    tankpos = tank.pos()
    update_terrain(tankpos)

    lroll.force(-(stick.y+keydir().y) + 0.5*stick.x + 1.5*keydir().x)
    rroll.force(-(stick.y+keydir().y) - 0.5*stick.x - 1.5*keydir().x)

    # Auto aim and shooting.
    if closest_tap(tankpos):
        target_pos,reset_turret = closest_tap(tankpos).pos3d(),False
        timeout(reset=True)
    elif timeout(10.0):
        turret_turn.force(0)    # Stop following target after some seconds.
        target_pos,reset_turret = None,not target_pos   # Double that time, we rotate turret forward again.
    if target_pos or reset_turret:
        aim = tank.orientation()*vec3(-1,0,0) if reset_turret else target_pos-tankpos
        tdir = turret.orientation()*vec3(-1,0,0)
        delta_angle = aim.angle_z(tdir)
        turret_turn.force(delta_angle)
        # Shoot if tapping close to where we're currently aiming.
        if taps() and abs(delta_angle) < 0.3 and timeout(1.0, timer=2):
            closest_tap(tankpos).invalidate()
            shots += [create_sphere(pos=turret.pos()+tdir*10, col='#333', vel=tdir*70)]

    # Explode shells whenever they hit something.
    for shot in shots[:]:
        if shot in collided_objects():
            explode(shot.pos())
            shot.release()
            shots.remove(shot)
