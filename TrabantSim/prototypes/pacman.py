#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# A Pacman-lookalike.

from trabant import *

# ASCII geometry.
level = r'''
XXXXXXXXXXXXXXXXXXXXXXXXXX
X         X   X          X
X         X   X          X
X         XX XX          X
X XXXXXXX        XXXXXXX X
X       X XX XX  X       X
XXXXX X X X   X  X X XXXXX
X   X X   XXXXX    X X   X
X   X   X       X    X   X
X   X XXXX     XXXX  X   X
X                        X
XXXXXXXXXXXXXXXXXXXXXXXXXX
'''

cam(distance=25)
gravity((0,0,0), bounce=0.1, friction=0)
accurate_ascii_generate(False)    # Optimization to load faster.

userinfo('Please wait while loading PacMan level...')
level = create_ascii_object(level, pos=(0,0,0), col='#00f', static=True)
userinfo()
def ball(pos, col):
    ball = create_sphere(radius=0.4, pos=pos, col=col)
    ball.create_engine(push_abs_engine)
    return ball
pacman = ball((0,0,-3),'#ff0')
ghosts = [ball(p,c) for p,c in [((-9.5,0,4),'#f00'), ((8.5,0,4),'#aaf'), ((-0.5,0,4),'#faa')]]
ghost_random_directions = [None,None,None]

while loop():
    # Steering.
    force = vec3(keydir().x, 0, keydir().y) * 5
    if taps():
        force += closest_tap(pacman.pos()).pos3d() - pacman.pos()
    pacman.engine[0].force(force)

    # Ghost movement.
    if timeout(2):    # Move randomly.
        [ghost.engine[0].force(rndvec().normalize(5).with_y(0)) for ghost in ghosts]
    if timeout(5.5, timer=2):    # Move towards Pacman.
        [ghost.engine[0].force(pacman.pos()-ghost.pos().limit(5)) for ghost in ghosts]

    for o1,o2,_,_ in collisions():
        if o1 == pacman and o2 in ghosts:
            explode(pacman.pos(), pacman.vel())
