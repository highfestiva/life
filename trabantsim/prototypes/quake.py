#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Quake prototype, mainly intended for computers.

from trabant import *
from trabant.objects import orthoscale

# ASCII geometries.
floorasc = r'''

XXXX      XXXX
XXXX      XXXX
XXXX      XXXX
XXXX      XXXX


--------------
  XXX     XX
XXXXX     XXXX




XXXX     XXXXX
  XX     XXX
--------------
     XXXX
     XXXX

      XX
      XX

     XXXX
     XXXX
      XX
      XX
      XX
      XX
      ´`
'''


trabant_init(fps=60) # Increase frame rate.
floor = create_ascii_object(floorasc, orientation=rotx(-pi/2), mat='checker', static=True, process=orthoscale((3,3,1)))
bgcol = vec3(0.4,0.8,1)
bg(col=bgcol)
gravity((0,0,0), friction=0, bounce=0.1)
def create_avatar(pos, col):
    # Create a man-like capsule. OK, maybe not man-like but a capsule then.
    avatar = create_capsule(pos, col=col)
    avatar.create_engine(walk_abs_engine, strength=30, max_velocity=3)
    avatar.floortime = gametime()
    avatar.powerup = 1
    return avatar
player = create_avatar((-15,0,4), '#00f0')    # Alpha=0 means invisible. We hide in case we use some rendering mode which displays backfacing polygons.
bot = create_avatar((15,0,2), '#f00')
avatars = (player,bot)
cam(distance=0, fov=60, target=player, target_relative_angle=True)
stick = create_joystick((0,0))
powerup = None

# Create walls.
symmetry = [(rotz(0.2),vec3(1,1,1)), (rotz(-0.2),vec3(-1,1,1)), (rotz(-0.2),vec3(1,-1,1)), (rotz(0.2),vec3(-1,-1,1))]
longwalls = [vec3(-10.5,10.5,2),(18,1,4)]
walls = [create_box(longwalls[0].mulvec(sym), side=longwalls[1], orientation=orientation, mat='flat', col='#943', static=True) for orientation,sym in symmetry]
# Pillars
symmetry = [vec3(1,1,1), vec3(-1,1,1), vec3(1,-1,1), vec3(-1,-1,1)]
[create_box(vec3(-2.7,2.7,2).mulvec(sym), side=(0.3,0.3,3), mat='flat', col=rndvec().abs(), static=True) for sym in symmetry]

# Create sliding doors.
doorcenter,doorwidth = vec3(0,-12,1.5),1.6
coords = [vec3(-doorwidth/2,0,0), vec3(+doorwidth/2,0,0)]
doors = []
for i,c in enumerate(coords):
    door = create_box(doorcenter+c, side=(doorwidth,0.2,2))
    walls[i+2].joint(slider_joint, door, axis=(-c).normalize(), stop=(0,1.5))    # We're bolting the door to the wall, that's why we need to invert the axis.
    door.opener = walls[i+2].create_engine(slider_engine)
    doors += [door]

yaw,pitch = -pi/2,0    # Start looking to the right, towards the center of the map.
grenades = []
gravity((0,0,-15), friction=1, bounce=4)    # Higer gravity than Earth in Quake. Bounce is a state variable. We want bouncing grenades.

# Touch device controls.
fasttap = lambda: timein(0.2, timer=5) and bool(taps()) and taps()[0].isrelease
check_reset_time_tap = lambda: not taps() and timeout_restart(timer=5)
is_shoot_tap = lambda: (fasttap() or check_reset_time_tap()) if is_touch_device() else click(left=True)

while loop():
    # Update mouse look angles.
    if is_touch_device():
        for tap in taps():
            yaw,pitch = yaw+tap.vx*0.1, pitch+tap.vy*0.1
    else:
        yaw,pitch = yaw-mousemove().x*0.09, pitch-mousemove().y*0.05
    pitch = max(min(pitch,pi/2),-pi/2)    # Allowed to look straight up and straight down, but no further.

    # XY movement relative to the current yaw angle, jumps are controlled with Z velocity.
    xyrot = rotz(yaw)
    player.engines[0].force(xyrot * (vec3(stick.x,stick.y,0)+keydir().with_z(0)) * player.powerup)
    if (keydir().z>0 or click(right=True)) and gametime()-player.floortime < 0.1 and timeout(0.3, first_hit=True):
        player.vel(player.vel()+vec3(0,0,6))

    # Look around.
    cam(angle=(pitch,0,yaw))
    [avatar.avel((0,0,0)) for avatar in [player,bot]]    # Angular velocity. Makes sure the avatars doesn't start rotating for some reason.
    [avatar.orientation(quat()) for avatar in [player,bot]]    # Keep avatars straight at all times.

    # Throw grenades.
    if is_shoot_tap() and timeout(1, timer=2, first_hit=True): # Limit shooting frequency.
            orientation = xyrot.rotate_x(pitch)
            vel = player.vel()
            pos = player.pos() + vel*0.05 + orientation*vec3(0,1,0)
            vel = vel + orientation*vec3(0,10,0)
            grenade = create_sphere(pos=pos, vel=vel, radius=0.05, col='#3a3')
            grenade.starttime = gametime()
            grenades += [grenade]
            sound(sound_bang, pos)

    # Check if grenade exploded or if a player touched ground.
    for obj,obj2,force,pos in collisions():
        if obj in grenades and obj2 in avatars:
            obj.starttime -= 100    # Set timeout to long ago, will explode below.
        # Store time of last floor touch so we know if we're able to jump.
        if force.z > force.length()/2 and obj in avatars:    # Check that force is mostly aimed upwards (i.e. stepping on ground).
            obj.floortime = gametime()    # Last time we touched the floor.
        # Check if someone took the powerup.
        if obj2==powerup and obj in avatars:
            sound(sound_engine_wobble, pos, volume=20)
            obj.powerup = 2
            powerup.release()
            powerup = None
    # Respawn powerup.
    if not powerup and timeout(10, timer=3):
        powerup = create_box(vec3(0,-25.5,1.5), side=0.5, mat='flat', col='#ff0')
        timeout_restart(timer=3)    # Erase timeout to avoid immediate spawn next time.

    # Explode grenades and remove missed grenades.
    for g in list(grenades):
        if gametime()-g.starttime > 3:
            pos = g.pos()
            explode(pos, vel=(0,0,3), strength=0.5)
            g.release()
            grenades.remove(g)
            # Add damage=acceleration to avatars.
            for avatar in avatars:
                direction = avatar.pos()-pos
                hurt = (10/direction.length()) ** 2
                if hurt > 10:
                    avatar.vel(direction*hurt)

    pos,bpos = player.pos(),bot.pos()

    # Open sliding doors if anyone nearby, otherwise close 'em.
    if timeout(0.2, timer=4):
        neardoors = [1 for distance in [(p-doorcenter).length() for p in (pos,bpos)] if distance<3.5]
        [door.opener.force(1 if neardoors else -1) for door in doors]

    # Respawn if fell down.
    if pos.z < -5:    # Fade to black.
        bg(col=bgcol*-5/pos.z)
        pitch += 0.05    # Look up towards the level you're falling down from.
    if pos.z < -50:
        sound(sound_clank, pos)
        player.vel((0,0,0))
        player.pos((-15,0,4))
        player.powerup = 1
        bg(col=bgcol)
        yaw,pitch = -pi/2,0
    if bpos.z < -50:
        bot.vel((0,0,0))
        bot.pos((15,0,4))
