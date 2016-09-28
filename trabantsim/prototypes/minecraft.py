#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Minecraft prototype, mainly intended for computers.

from trabant import *

bg(col='#9df')
gravity((0,0,-15), friction=0, bounce=0.1)

player = create_capsule((0,0,15), col='#00f0')    # Alpha=0 means invisible object.
player.create_engine(walk_abs_engine, strength=20, max_velocity=2)
cam_offset = vec3(0,0,0.5)    # Keep the camera in eye level, same as in the original Minecraft.
cam(distance=0, fov=60, pos=cam_offset, target=player, target_relative_angle=True)
stick = create_joystick((0,0))

fg(col='#975')    # Brownish floor.
cube = create_box(pos=(1e5,1e5,1e5), side=1, mat='noise', static=True)    # Template.
flooring_coords = [(vec3(x,y,0),quat()) for y in range(-8,8+1) for x in range(-8,8+1)]
create_clones(cube, flooring_coords, static=True)
fg(col='#3a4')    # Build in green color.

# Touch device controls.
fasttap = lambda: taps()[0].isrelease and timein(0.4, timer=3)
holdtap = lambda: taps()[0].movement2()<3e-5 and timeout(0.6, timer=3)
reset_time_tap = lambda: timeout_restart(timer=3)
is_add_tap = lambda: fasttap() if is_touch_device() else click(right=True)
is_remove_tap = lambda: holdtap() if is_touch_device() else click(left=True)

yaw,pitch,allow_add = 0,-pi*0.4,True
while loop():
    # Update mouse look angles.
    if is_touch_device():
        for tap in taps():
            yaw,pitch = yaw+tap.vx*0.1, pitch+tap.vy*0.1
    else:
        yaw,pitch = yaw-mousemove().x*0.09, pitch-mousemove().y*0.05
    pitch = max(min(pitch,pi/2),-pi/2)    # Allowed to look straight up and straight down, but no further.

    # XY movement relative to the current yaw angle, jumps are controlled with Z velocity.
    xyrot = quat().rotate_z(yaw)
    player.engines[0].force(xyrot * (vec3(stick.x,stick.y,0)+keydir().with_z(0)))
    if keydir().z>0 and timeout(1, first_hit=True):
        player.vel(player.vel()+vec3(0,0,6))

    # Look around.
    cam(angle=(pitch,0,yaw))
    player.avel((0,0,0))    # Angular velocity. Makes sure the player object doesn't start rotating for some reason.
    player.orientation(quat())    # Keep player object straight at all times.

    # Build/destroy blocks or pick color.
    if click() and (is_touch_device() or timeout(0.2, timer=2, first_hit=True)):
        campos,cam_orientation = player.pos()+cam_offset, xyrot.rotate_x(pitch)
        direction = taps()[0].pos3d(1)-campos if is_touch_device() else cam_orientation*vec3(0,1,0)
        os = [(o,p) for o,p in pick_objects(campos, direction, 0,6) if o!=player]
        if os:
            obj,pos = os[0]    # Pick nearest cube.
            if is_remove_tap():
                allow_add = False
                obj.release()
            elif is_add_tap() and allow_add:
                cubepos = obj.pos()
                create_clones(cube, [(toaxis(pos-cubepos)+cubepos,quat())], static=True)
            elif click(middle=True):
                fg(col=obj.col())
    else:
        reset_time_tap()
        allow_add = True

    # Change color.
    if mousewheel():
        fg(col=rndvec().abs())

    # Respawn if fell down.
    if player.pos().z < -30:
        player.vel((0,0,0))
        player.pos((0,0,30))
        yaw,pitch = 0,-pi*0.4
