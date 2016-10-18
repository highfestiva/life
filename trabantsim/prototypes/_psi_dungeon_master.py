#!/usr/bin/env python3
'''A simple precognition PRNG game. Press left or right on the keyboard and try to predict the next turn.'''

from math import factorial
from random import choice
from trabant import *


houses = {}
moving = True
pos,target_pos = vec3(),vec3()
look_angle,target_angle = 0,0
hits,trials = 0,0


cam(distance=0, fov=60)
create_box(vec3(0,0,-500), side=1000-2, static=True)    # Ground.
template_house = create_box(vec3(0,0,-10), side=6, mat='flat', col='#fff', static=True)


def newhouse(pos):
    pos *= 10
    pos.x -= 5
    pos.y -= 5
    pos.z = -1
    fg(rndvec()+vec3(0.5,0.5,0.5))
    return create_clones(template_house, [(pos,quat())], static=True)[0]

def update(pos):
    unused = set(houses.keys())
    x,y = (pos.x-5)//10,(pos.y-5)//10
    for ix in range(-2,5):
        for iy in range(-2,5):
            h = (x+ix)*1753 + y+iy
            if h not in houses:
                newpos = vec3(x+ix,y+iy,0)
                houses[h] = newhouse(newpos)
            else:
                unused.remove(h)
    for h in unused:
        houses[h].release()
        del houses[h]



def n_comb_k(n,k):
    return factorial(n) // factorial(k) // factorial(n-k)

def pvalue(hits,trials):
    return sum(n_comb_k(trials,k)*(0.5**trials) for k in range(hits,trials+1))

def odds(hits,trials):
    return 1/pvalue(hits,trials)


while loop():
    update(pos)
    cam(pos=pos, angle=vec3(0,0,look_angle))
    if moving:
        look_angle = look_angle*0.93 + target_angle*0.07
        if abs(target_angle-look_angle) < 0.1:
            pos = pos*0.9 + target_pos*0.1
            if (target_pos-pos).length() < 0.1:
                moving = False
    elif keydir().x:
        d = choice([-1,+1])
        if keydir().x*d > 0:
            hits,trials = hits+1,trials+1
            print('Correct! %g%% hits on %i trials, odds %g:1 of getting this many or more right.' % (hits*100/trials,trials,odds(hits,trials)))
        else:
            hits,trials = hits,trials+1
            print('Wrong! %g%% hits on %i trials, odds %g:1 of getting this many or more right.' % (hits*100/trials,trials,odds(hits,trials)))
        target_angle -= pi/2 * d
        target_pos += quat().rotate_z(target_angle)*vec3(0,10,0)
        moving = True
    keydir()    # Drop any pending keys, don't let them queue up.
