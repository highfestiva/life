#!/usr/bin/env python3

from progwar.client import my_tanks,join_game,create_sphere
from math import atan2,atan,asin,pi,sin,sqrt
from trabant.math import vec3,quat
from time import time


enemy_tank = None
shoot_time = None
enemy_nextpos,aim_target_ball,aim_dir_ball = None,None,None


def closest_tank(pos, tanks):
    return min(tanks, default=None, key=lambda t:(t.pos-pos).length())


def extrapolate(now, enemy_tanks):
    shoot = False
    # Extrapolate velocity. tank.et is time for extrapolation, tank.epos is extrapolated position.
    for tank in my_tanks+enemy_tanks:
        if hasattr(tank,'prev_pos') and tank.pos != tank.prev_pos:
            dt = now-tank.t
            prev_vel = tank.vel
            tank.vel = (tank.pos-tank.prev_pos)/dt
            tank.acc = ((tank.vel-prev_vel)/dt).with_z(0)
            tank.epos = tank.pos
            tank.prev_pos = tank.pos
            tank.t = tank.et = now
            if shoot_time and shoot_time-now <= 0:
                shoot = True
        elif hasattr(tank,'epos'):
            dt = now-tank.et
            tank.acc = tank.acc*0.9*dt - tank.epos.with_z(0).normalize()*dt
            tank.vel += tank.acc * dt
            tank.epos = tank.epos + tank.vel * dt
            tank.et = now
        else:
            tank.prev_pos = tank.pos
            tank.epos = tank.pos
            tank.vel = vec3()
            tank.acc = vec3()
            tank.t = tank.et = now
    return shoot


def pick_enemy(now, pos, enemy_tanks):
    # Pick one enemy to gather around.
    move2pos = [vec3()]*len(my_tanks)
    global enemy_tank,angle,angle_t
    enemy_tank = enemy_tank if enemy_tank in enemy_tanks else None
    if not enemy_tank and my_tanks:
        enemy_tank = closest_tank(pos, enemy_tanks)
    if enemy_tank:
        center_vec = -enemy_tank.epos.with_z(0).normalize(60)
        angle = 0#sin(now*0.1)*0.4
        move2pos = [quat().rotate_z(angle+pi*i/(len(my_tanks)+8))*center_vec+enemy_tank.epos for i in range(-1,len(my_tanks)-1)]
    return move2pos, enemy_tank


def update(blips):
    global my_tanks
    enemy_tanks = [blip for blip in blips if (blip.type=='tank' and blip.pos.z>-18)]

    now = time()

    # First time shooting we wait some extra.
    global shoot_time,pick_time
    if not my_tanks:
        shoot_time,pick_time = None,None
    elif not shoot_time and my_tanks:
        shoot_time = now+3.6
        pick_time = now+3.4

    shoot = extrapolate(now, enemy_tanks)

    move2pos,enemy = [],None
    if pick_time and pick_time-now <= 0:
        target_pos = my_tanks[0].epos if len(my_tanks)==1 else vec3()
        move2pos, enemy = pick_enemy(now, target_pos, enemy_tanks)

    global enemy_nextpos,aim_target_ball,aim_dir_ball
    if not enemy_nextpos:
        enemy_nextpos = create_sphere(radius=5, col='#00f', static=True)
        aim_target_ball = create_sphere(radius=5, col='#ff0', static=True)
        aim_dir_ball = create_sphere(radius=3, col='#f0f', static=True)
    elif move2pos:
        enemy_nextpos.pos(move2pos[0])

    # Move around and shoot!
    for i,tank in enumerate(my_tanks):
        tank.shoot(None,None,None)  # Only shoot when tanks can coordinate with the best positional approxmiation.
        emergency = False
        selfpos,selfvel = tank.epos.with_z(0),tank.vel.with_z(0)
        if enemy:
            # Drive towards enemy, but keep somewhat centered.
            epos,evel = enemy.epos.with_z(0),enemy.vel.with_z(0)
            if selfvel.length() > 25 or ((selfpos+selfvel*3).length() > 80 and selfpos*selfvel > 10):
                #print('braking!', selfpos*selfvel)
                tank.drive(-selfvel*10)
            elif (epos-selfpos).length() > 100 and evel*selfvel < -1 and (epos-selfpos)*selfvel < 0.7:
                # We're far away, possibly circling one another. Instead copy enemy's movement!
                #print('following enemy')
                tank.drive(evel*10)
            else:
                #print('enemy and self vel:', evel, selfvel)
                direction = move2pos[i].with_z(0) - selfpos - selfvel
                tank.drive(direction)
            if len(my_tanks) == 1:
                shoot = True
            if selfpos.length() > 115 and selfpos*selfvel > 10:
                if tank.pos.z < -18 or tank.health < 30:
                    shoot = True
                else:
                    emergency = True

            # Shoot at where enemy is assumed to be.
            ds = epos+evel*2 - (selfpos+selfvel*2)
            if i==0: aim_target_ball.pos(tank.epos + ds)
            yaw = -atan2(ds.x,ds.y)
            if i==0: aim_dir_ball.pos(quat().rotate_z(yaw)*vec3(0,20,0) + tank.epos)
            if emergency:
                ds = tank.epos
                yaw = -atan2(ds.x,ds.y)
                tank.shoot(yaw,-pi*3/8,'damage')
            elif shoot:
                l = ds.length()
                pitch = atan((0.11*(l**1.02)-2)/(l/2))
                tank.shoot(yaw, pitch, 'damage')
        else:
            #print('moving to center!')
            tank.drive(-selfpos.limit(10) - selfvel)

    global enemy_tank
    if len(my_tanks) == 1 or (enemy_tank and enemy_tank.epos.z > -10):
        enemy_tank = None

join_game('Nessie', update)
