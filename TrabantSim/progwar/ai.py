#!/usr/bin/env python3

from progwar.client import my_tanks,join_game
from math import atan2,atan
from trabant.math import vec3
from time import time


def closest_tank(pos, tanks):
    return min(tanks, default=None, key=lambda t:(t.pos-pos).length())

def update(blips):
    enemy_tanks = [blip for blip in blips if blip.type == 'tank']

    # Extrapolate velocity. tank.et is time for extrapolation, tank.epos is extrapolated position.
    now = time()
    for tank in my_tanks+enemy_tanks:
        if hasattr(tank,'prev_pos') and tank.pos != tank.prev_pos:
            tank.vel = (tank.pos-tank.prev_pos)/(now-tank.t)
            tank.epos = tank.pos
            tank.prev_pos = tank.pos
            tank.t = tank.et = now
        elif hasattr(tank,'epos'):
            tank.epos = tank.epos + tank.vel * (now-tank.et)
            tank.et = now
        else:
            tank.prev_pos = tank.pos
            tank.epos = tank.pos
            tank.vel = vec3()
            tank.t = tank.et = now

    for tank in my_tanks:
        enemy = closest_tank(tank.pos, enemy_tanks)
        if enemy:
            # Drive towards enemy, but keep somewhat centered.
            tank.drive((enemy.epos+enemy.vel)/4 - (tank.pos+tank.vel))
            # Shoot at where enemy is assumed to be.
            ds = (enemy.epos+enemy.vel*2) - (tank.pos+tank.vel*2)
            yaw = -atan2(ds.x,ds.y)
            pitch = atan((0.15*ds.length()-2)/(ds.length()/2))
            tank.shoot(yaw, pitch, 'damage')


join_game('Nessie', update)
