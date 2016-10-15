#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Snake prototype.

from trabant import *

cam(distance=60)

pos = vec3()
step = vec3(1,0,0)
block = lambda p: create_box(p, side=0.7, static=True)
blocks,snake_coords = [block(pos)],[pos]
length = 5

while loop(delay=0.2): # Slow down snake.
    # Steering.
    move = vec3(keydir().x,0,keydir().y)
    move += tapdir(pos, digital_direction=True)
    if move.x or move.z:
        step = move
    pos += step

    # Check if colliding with self.
    if pos in snake_coords: # Collide with block?
        explode(pos)
        while blocks: # Eat up snake from back to front.
            blocks[-1].release()
            blocks,snake_coords = blocks[:-1],snake_coords[:-1]
            sleep(0.05)
        length = 5
    # Move one block forward.
    blocks = [block(pos)] + blocks
    snake_coords = [pos] + snake_coords
    if len(snake_coords) >= length: # Remove last block when moving forward.
        blocks[-1].release()
        blocks,snake_coords = blocks[:-1],snake_coords[:-1]
    if timeout(1): # Every second the snake gets longer.
        length += 1
