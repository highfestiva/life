#!/usr/bin/env python3
# Side scroller prototype.

# Ideas:
#  - planets in the background,
#  - health,
#  - blink when immortal after death,
#  - different & classical enemies,
#  - tunnel,
#  - power-ups including weapons and shield,
#  - bosses.

from trabant import *
from math import fmod
from time import time
from random import choice

shipasc = r'''
/XX\
´XXXXXX\
 lXXXXXXXXXXXX>
/XXXXXX`
´XX`'''

enemyasc = r'''
   /XXX
/XXXX
´XXXX
   ´XXX
'''

stars,shots,enemies = [],[],[]
ship = create_ascii_object(shipasc, col='#88a', mass=500)
ship.create_engine(push_abs_engine, friction=3, strength=40, max_velocity=60)
ship.create_engine(push_turn_rel_engine, friction=2, strength=7, max_velocity=10)
collisions(False)
bubble = create_sphere(radius=1.5, col='#33b')
ship.joint(fixed_joint, bubble)
collisions(True)

fg(outline=False)
gravity((0,0,0))
cam(distance=100)
async_load()

def create_star(x):
    global stars
    id = random()*0.7+0.3
    stars += [create_sphere((x, 50-10*id, random()*200-100), radius=0.2, col=vec3(id,id,id), vel=(id*-30, 0, 0))]

def dostars():
    global stars
    while stars and stars[0].pos().x < -100:
        stars[0].release()
        stars = stars[1:]
    create_star(100)
    timer_callback(0.2+random(), dostars)

for i in range(40):
    create_star((i-20)*5)
dostars()

while loop():
    x = keydir().x
    z = keydir().y
    ship.engine[0].force((x,0,z))
    ship.engine[1].force(keydir().y)
    ship.bounce_in_rect((-65,-0.1,-40), (65,0.1,40), spring=0)
    if timeout(1, 2):
        q = ship.orientation()
        q.q[2] = q.q[3] = 0
        q = q.normalize()
        ship.avel(ship.avel().with_y(0).with_z(0))
        ship.orientation(q)

    #o = ship.orientation()
    #u = vec3(1,0,0)
    #v = o*u
    #if timeout(0.2):
    #    print(u.angle_z(v), angmod(u.angle_z(v)))
    #o = o.rotate_z(-angmod(u.angle_z(v))*0.2)
    #o = o.rotate_y(+angmod(u.angle_y(v))*0.2)
    #ship.orientation(o)

    new_shots = []
    if 'SPACE' in keys() and timeout(0.2, first_hit=True):
        p = ship.pos()
        new_shots += [create_capsule(p+vec3(12,0, 0), orientation=quat().rotate_y(pi/2), vel=vec3(90,0,0), col='#a60', trigger=True)]
        #new_shots += [create_capsule(p+vec3( 0,0,+5), orientation=quat().rotate_y(pi/2), vel=vec3(90,0,0), col='#a60', trigger=True)]
        #new_shots += [create_capsule(p+vec3( 0,0,-5), orientation=quat().rotate_y(pi/2), vel=vec3(90,0,0), col='#a60', trigger=True)]
        for shot in new_shots:
            shot.physical = True
        sound(sound_bang, p, volume=0.1)
    elif 'LCTRL' in keys() and timeout(1, first_hit=True):
        p = ship.pos()
        laser_shots  = [create_capsule(p+vec3(85,0, 0), orientation=quat().rotate_y(pi/2), length=150, radius=0.2, col='#f0f', trigger=True)]
        laser_shots += [create_capsule(p+vec3(75,0,+4), orientation=quat().rotate_y(pi/2), length=150, radius=0.2, col='#f0f', trigger=True)]
        laser_shots += [create_capsule(p+vec3(75,0,-4), orientation=quat().rotate_y(pi/2), length=150, radius=0.2, col='#f0f', trigger=True)]
        for shot in laser_shots:
            shot.physical = False
        new_shots += laser_shots[:]
        def remove_shot():
            for laser_shot in laser_shots:
                shots.remove(laser_shot)
                laser_shot.release()
        timer_callback(0.2, remove_shot)

    if timeout(1, 1):
        if random() > 0.3:
            enemy = create_box((100,0,random()*80-40), vel=(-10,0,0), side=3)
            enemy.avel(rndvec()) # TODO: fix!
            enemy.move = lambda x: 0
        else:
            enemy = create_ascii_object(enemyasc, pos=vec3(100,0,random()*80-40), vel=(-5,0,0), col='#f00')
            enemy.move = sin
        enemy.physical = True
        enemies += [enemy]

    ais = [e for e in enemies if e.move(1)]
    if ais and timeout(1, 5):
        enemy = choice(ais)
        p = enemy.pos()
        shot = create_capsule(p+vec3(-10,0, 0), orientation=quat().rotate_y(pi/2), vel=vec3(-45,0,0), col='#ff4', trigger=True)
        shot.physical = True
        new_shots += [shot]

    shots += new_shots

    for obj1,obj2,force,pos in collisions():
        for obj in (obj1,obj2):
            if obj in enemies:
                enemies.remove(obj)
                obj.release()
                explode(pos)
            elif obj in shots:
                if obj.physical:
                    shots.remove(obj)
                    obj.release()

    for obj in shots+enemies:
        if not obj.physical:
            continue
        x = obj.pos().x
        if x < -100 or x > +100:
            obj.release()
            if obj in shots:
                shots.remove(obj)
            else:
                enemies.remove(obj)


    t = time()
    for enemy in enemies:
        enemy.vel((-5,0,enemy.move(t*2)*5))
        enemy.avel((1,0,0))
