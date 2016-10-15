#!/usr/bin/env python3
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

userinfo('Loading level...')
level = create_ascii_object(level, pos=(0,0,0), col='#00f', static=True)

def ball2smiley(orientation, gfx, phys):
    '''Moves some vertices of a sphere to make it into a smiley.'''
    gfx.vertices = [(v if abs(v.z)>0.1 or v.x<0 else v.with_x(0)) for v in gfx.vertices]
    return orientation,gfx,phys

def ball2ghost(orientation, gfx, phys):
    '''Moves some vertices of a sphere to make it into a ghost.'''
    gfx.vertices = [(v if v.z>=0 else v.with_z(-0.4)) for v in gfx.vertices]
    return orientation,gfx,phys

def ball(pos, col, process):
    ball = create_sphere(radius=0.4, pos=pos, col=col, process=process)
    ball.create_engine(push_abs_engine)
    return ball
pacman = ball((0,0,-3), '#ff0', ball2smiley)
ghosts = [ball(p,c,ball2ghost) for p,c in [((-9.5,0,4),'#f00'), ((8.5,0,4),'#aaf'), ((-0.5,0,4),'#faa')]]
ghost_random_directions = [None,None,None]

while loop():
    # Steering.
    force = vec3(keydir().x, 0, keydir().y) * 5
    force += tapdir(pacman.pos()).with_y(0)
    pacman.engines[0].force(force)

    # Ghost movement.
    if timeout(2, timer='random_move'): # Move randomly.
        [ghost.engines[0].force(rndvec().normalize(5).with_y(0)) for ghost in ghosts]
    if timeout(5.5, timer='to_target'): # Move towards Pacman.
        [ghost.engines[0].force(pacman.pos()-ghost.pos().limit(5)) for ghost in ghosts]

    for o1,o2,_,_ in collisions():
        if o1 == pacman and o2 in ghosts:
            explode(pacman.pos(), pacman.vel())
