#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Simple Tetris prototyping. Move left/right/down by tapping left/right/below tetromino. Rotate by tapping above.

from trabant import *
import random

bg(col='#888')
cam(angle=(0,-pi/2 if is_touch_device() else 0,0), distance=30, pos=(5,0,10))
gravity((0,0,0))

tetromino_lines = '''
XXXX  |  XX  |  X    |    X  |  XXX  |   XX  |  XX 
      |  XX  |  XXX  |  XXX  |   X   |  XX   |   XX
'''.strip('\n').split('\n')
# Slice and dice the tetrominos to get them on the form 'XX \n XX'.
tetromino_lines = [tl.split('  |  ') for tl in tetromino_lines]
tetrominos = ['\n'.join(t) for t in zip(*tetromino_lines)]
tetromino = None

colors = '#0ff #ff0 #00f #a50 #f0f #0f0 #f00'.split()
gridsize = vec3(10,20,0) # Standard tetris grid.
fixed_blocks = [[None]*gridsize.x for _ in range(gridsize.y+5)]

# Create floor and walls.
create_box(pos=(5,0,-20-0.5), side=40, static=True)
create_box(pos=(-20-0.5,0,10), side=40, static=True)
create_box(pos=(10+20-0.5,0,10), side=40, static=True)

def get_tetronimo_blocks(pos, orientation):
    '''Returns a list coordinates of the blocks which constitude the current tetromino.'''
    coords = []
    piece = [line for line in tetrominos[tetromino_index].split('\n') if line.strip()]
    for y,row in enumerate(piece):
        for x,block in enumerate(row):
            if block == 'X':
                intpos = vec3(*[round(n) for n in (orientation*vec3(x-1, 0, -y-tetromino_rot_center.z*2) + pos)])
                coords += [vec3(intpos.x,intpos.z,0)]
    return coords

def trymove(movement, orientation):
    '''Attempt moving and/or rotating the current tetromino. Will fixate tetromino if it
       hits "ground" and if so consume any completed rows.'''
    global tetromino,fixed_blocks
    pos = tetromino.pos() + tetromino.orientation()*tetromino_rot_center
    coords = get_tetronimo_blocks(pos+movement, orientation)
    if movement.z<0 and ([c for c in coords if c.y<0] or [c for c in coords if fixed_blocks[c.y][c.x]]):
        # We've hit something below, kill tetromino and fixate blocks without movement.
        tetromino = tetromino.release()
        for coord in get_tetronimo_blocks(pos, orientation):
            fixed_blocks[coord.y][coord.x] = create_ascii_object('X', pos=(coord.x, 0, coord.y), col=colors[tetromino_index], static=True)
        # Consume completed rows.
        row_i = 0
        while row_i < len(fixed_blocks):
            row = fixed_blocks[row_i]
            if len([1 for block in row if block]) == gridsize.x:    # Complete row? Yes: shift down, then add empty row at top.
                [block.release() for block in row]
                for row_j in range(row_i+1,len(fixed_blocks)):
                    fixed_blocks[row_j-1] = fixed_blocks[row_j]
                    [block.pos((x,0,row_j-1)) for x,block in enumerate(fixed_blocks[row_j-1]) if block]
                fixed_blocks[-1] = [None]*gridsize.x
                sound(sound_clank)
            else:
                row_i += 1

    elif not [c for c in coords if c.x<0 or c.x>=gridsize.x or c.y<0] and not [c for c in coords if fixed_blocks[c.y][c.x]]:
        # This is an ok rotation or move: it neither hits the side nor a fixed block.
        tetromino.pos(pos+movement - orientation*tetromino_rot_center, orientation=orientation)

while loop():
    if not tetromino:
        # Create a random tetromino.
        tetromino_index = random.choice(range(len(tetrominos)))
        tetromino = create_ascii_object(tetrominos[tetromino_index], pos=(0,0,100), col=colors[tetromino_index])
        # We can't rotate around a "randomly" placed center, so instead we rotate about a point *relative* to the
        # top-left corner of the tetromino. Center of rotation (1.5,0.5,0.5) works well for all types of tetrominos.
        tetromino_rot_center = vec3(1.5,0.5,0.5) - last_ascii_top_left_offset()    # Rotate about a point in the middle of the first row.
        tetromino.pos(vec3(gridsize.x/2,0,gridsize.y) - tetromino_rot_center)

    if (keys() or taps()) and timeout(0.15): # Steering.
        v = vec3(keydir().x,0,keydir().y)
        v += tapdir(tetromino.pos(), digital_direction=True)
        # Tapping above means "rotate", tapping left/below/right means move in that direction.
        if v.z > 0: trymove(vec3(), tetromino.orientation().rotate_y(pi/2))
        else:       trymove(v, tetromino.orientation())

    # Move down every once in a while.
    if tetromino and timeout(0.8): # Use same timer as user movement. This hinders step down indefinitely when tapping/holding/pressing.
        trymove(vec3(0,0,-1), tetromino.orientation())
