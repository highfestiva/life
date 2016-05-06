#!/usr/bin/env python3

from progwar.client import my_tanks,join_game
from math import atan2,atan


def closest_tank(pos, tanks):
    return min(tanks, default=None, key=lambda t:(t.pos-pos).length())

def update(blips):
    other_tanks = [blip for blip in blips if blip.type == 'tank']

    for tank in my_tanks:
        enemy = closest_tank(tank.pos, other_tanks)
        if enemy:
            # Drive towards enemy.
            tank.drive(enemy.pos-tank.pos)
            # Shoot at where enemy was last sighted.
            ds = enemy.pos - tank.pos
            yaw = -atan2(ds.x,ds.y)
            pitch = atan(80/(ds.length()/2))
            tank.shoot(yaw, pitch, 'damage')


join_game('localhost', update)
