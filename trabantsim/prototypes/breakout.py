#!/usr/bin/env python3
# Breakout prototyping.

from trabant import *

# ASCII geometries.
paddle = r'''
 ^     ^
<XXXXXXX>
 v     v
'''
brick = 'XX'

cam(angle=(0,0,0), distance=45)
gravity((0,0,0), bounce=1, friction=1)

paddle = create_ascii_object(paddle, pos=(0,0,-15), mass=10000)
ball = create_sphere(pos=(-7,0,10), vel=(10,0,-30), radius=0.5, col='#fff')
bricks = set()
for y in range(2):
    for x in range(12):
        bricks.add(create_ascii_object(brick, pos=(x*3-16,0,15-y*2), col=rndvec().abs(), static=True))

while loop():
    # Paddle controls.
    v = keydir()*20 + tapdir(paddle.pos())*10
    paddle.vel((v.x,0,0), avel=(0,0,0)) # Only move in X, no rotation.
    # Make the ball bounce against invisible edges.
    ball.bounce_in_rect((-20,-0.1,-25), (20,0.1,17))
    # Check if ball fell down below the paddle.
    if ball.pos().z < -17:
        explode(ball.pos(), ball.vel())
        ball.pos((-7,0,10))
        ball.vel((10,0,-30))
    # Play a clanking sound if we hit a brick.
    for o in collided_objects():
        if o in bricks:
            sound(sound_clank, o.pos())
            o.release()
            bricks.remove(o)
