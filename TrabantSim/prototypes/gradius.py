#!/usr/bin/env python3
# Side scroller prototype.

# Ideas:
#  + planets in the background,
#  + health,
#  + blink when immortal after death,
#  - tunnel,
#  - different & classical enemies,
#  - power-ups including weapons and shield,
#  - explosion effect when firing heavy weapons,
#  - bosses.

from trabant import *
from trabant.gameapi import setvar
from trabant.objects import gfxscale
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

celestials,shots,enemies = [],[],[]
tunnel_segs,tunnel_top_seed,tunnel_bottom_seed = [],0,501

ship = create_ascii_object(shipasc, col='#88a', mass=500)
ship.create_engine(push_turn_rel_engine, friction=2, strength=7, max_velocity=10)
collisions(False)
bubble = create_sphere(radius=1.5, col='#33b')
ship.joint(fixed_joint, bubble)
collisions(True)
ship_parts = (ship, bubble)
ship.ship_parts = bubble.ship_parts = ship_parts
ship.health = 100
ship.hurt = bubble.hurt = 100
ship.immortal = False
blink_time = None

fg(outline=False)
gravity((0,0,0))
setvar('Ui.3D.Shadows','no')
cam(distance=100)
async_load()

def create_celestial(x):
    global celestials
    if random() > 0.95:  # Create planets sometimes.
        celestials += [create_sphere((x, 40, random()*80-40), radius=random()*2+1, col='#444', vel=(-35,0,0))]
    else:   # Create stars.
        idist = random()*0.6+0.4
        celestials += [create_sphere((x, 50-10*idist, random()*80-40), radius=0.2, col=vec3(idist,idist,idist), vel=(idist*-30, 0, 0))]
def dobkg():
    global celestials
    while celestials and celestials[0].pos().x < -100:
        celestials[0].release()
        celestials = celestials[1:]
    create_celestial(100)
    timer_callback(0.2+random(), dobkg)
for i in range(40):
    create_celestial((i-20)*5)
dobkg()

def create_tunnel_seg(z, h, x, y1, y2, seed):
    verts,tris = [],[]
    width,subs = 20,5
    for mx in range(subs+1):
        lastseed = seed
        seed = (seed * 214013 + 2531011) & 0x7fffffff
        rz = (seed*4/0x7fffffff) - 2
        verts += [(mx*4-width/2,0,rz), (mx*4-width/2,0,rz+h)]
        t = mx*2
        tris += [0+t,2+t,1+t, 1+t,2+t,3+t]
    seg = create_mesh(verts, tris[:-6], pos=(x,0,z), vel=(-5,0,0), col='#f00', trigger=True)
    seg.hurt = 50
    seg.physical = True
    return seg,lastseed
def create_tunnel_segs(x):
    global tunnel_segs,tunnel_top_seed,tunnel_bottom_seed
    segtop,tunnel_top_seed = create_tunnel_seg(-50, 20, x, -20, 0, tunnel_top_seed)
    segbottom,tunnel_bottom_seed = create_tunnel_seg(40, 20, x, 0, 20, tunnel_bottom_seed)
    tunnel_segs += [segtop, segbottom]
def update_tunnel():
    px = tunnel_segs[-1].pos().x
    if px == 0:
        return  # Object not loaded yet.
    if px < 90-10:
        print('Creating tunnel seg', px)
        create_tunnel_segs(px+20-0.5)
create_tunnel_segs(-90)

while loop():
    update_tunnel()

    x = keydir().x
    z = keydir().y
    ship.vel((40*x,0,40*z))
    ship.engine[0].force(-z)
    ship.bounce_in_rect((-65,-0.1,-40), (65,0.1,40), spring=0)
    if timeout(1, 2):
        q = ship.orientation()
        q.q[2] = q.q[3] = 0
        q = q.normalize()
        ship.avel(ship.avel().with_y(0).with_z(0))
        ship.orientation(q)

    if blink_time:
        ship.health = 100
        t = time() - blink_time
        f = abs(sin(t*8))
        ship.col((f,f,f))
        if t > 3:
            blink_time = None
            ship.immortal = False
            ship.col('#88a')

    #o = ship.orientation()
    #u = vec3(1,0,0)
    #v = o*u
    #if timeout(0.2):
    #    print(u.angle_z(v), angmod(u.angle_z(v)))
    #o = o.rotate_z(-angmod(u.angle_z(v))*0.2)
    #o = o.rotate_y(+angmod(u.angle_y(v))*0.2)
    #ship.orientation(o)

    new_shots = []
    if 'SPACE' in keys() and timeout(0.1, first_hit=True):
        p = ship.pos()
        new_shots += [create_capsule(p+vec3(12,0, 0), orientation=quat().rotate_y(pi/2), vel=vec3(90,0,0), col='#a60', trigger=True)]
        new_shots += [create_capsule(p+vec3( 0,0,+3), orientation=quat().rotate_y(pi/2), vel=vec3(90,0,0), col='#a60', trigger=True)]
        new_shots += [create_capsule(p+vec3( 0,0,-3), orientation=quat().rotate_y(pi/2), vel=vec3(90,0,0), col='#a60', trigger=True)]
        for shot in new_shots:
            shot.physical = True
            shot.shooters = ship_parts
            shot.hurt = 30
        sound(sound_bang, p, volume=0.1)
    elif 'LCTRL' in keys() and timeout(1, first_hit=True):
        p = ship.pos()
        scale = gfxscale((0.3, 0.3, 1)) # Scale X & Y, Z is height of capsule.
        laser_shots  = [create_capsule(p+vec3(85,0, 0.0), orientation=quat().rotate_y(pi/2), length=150, radius=1, col='#f0f', trigger=True, process=scale)]
        laser_shots += [create_capsule(p+vec3(75,0,+2.5), orientation=quat().rotate_y(pi/2), length=150, radius=1, col='#f0f', trigger=True, process=scale)]
        laser_shots += [create_capsule(p+vec3(75,0,-2.5), orientation=quat().rotate_y(pi/2), length=150, radius=1, col='#f0f', trigger=True, process=scale)]
        for shot in laser_shots:
            shot.physical = False
            shot.shooters = ship_parts
            shot.hurt = 60
        new_shots += laser_shots[:]
        def remove_shot():
            for laser_shot in laser_shots:
                shots.remove(laser_shot)
                laser_shot.release()
        timer_callback(0.2, remove_shot)

    # Create new enemies.
    if timeout(1, 1):
        if random() > 0.3:
            enemy = create_box((80,0,random()*80-40), vel=(-10,0,0), avel=rndvec(), side=3)
            enemy.move = None
        else:
            enemy = create_ascii_object(enemyasc, pos=vec3(80,0,random()*80-40), vel=(-5,0,0), col='#f00')
            enemy.move = lambda t: (-10,0,10*sin(2*t))
        enemy.physical = True
        enemy.ship_parts = (enemy,)
        enemy.health = 20
        enemy.hurt = 60
        enemy.immortal = False
        enemies += [enemy]

    # Enemies shoot.
    ais = [e for e in enemies if e.isloaded and e.move]
    if ais and timeout(1, 5):
        enemy = choice(ais)
        p = enemy.pos()
        shot = create_capsule(p+vec3(-8,0, 0), orientation=quat().rotate_y(pi/2), vel=vec3(-45,0,0), col='#ff4', trigger=True)
        shot.physical = True
        shot.shooters = enemy.ship_parts
        shot.hurt = 40
        new_shots += [shot]

    shots += new_shots

    for obj1,obj2,force,pos in collisions():
        withshot = hasattr(obj2, 'shooters')
        if obj1 in ship_parts or obj1 in enemies:
            if withshot and obj2.shooters == obj1.ship_parts:
                continue
            hull = obj1.ship_parts[0]
            if hull.immortal:
                continue
            hull.health -= obj2.hurt
            if hull.health <= 0:
                explode(pos, obj1.vel(), obj1.hurt/50, volume=0.1)
                if hull == ship:
                    ship.health = 100
                    ship.immortal = True
                    blink_time = time()
                else:
                    enemies.remove(hull)
                    for o in obj1.ship_parts:
                        obj1.release()
        elif obj1 in shots:
            if obj1.physical and obj2 not in obj1.shooters:
                explode(pos, obj1.vel()*0.5, obj1.hurt/50, volume=0.1)
                shots.remove(obj1)
                obj1.release()

    for obj in shots[:len(shots)//4+4] + enemies[:len(enemies)//4+4] + tunnel_segs[:2]:
        if not obj.physical:
            continue
        x = obj.pos().x
        if x < -100 or x > +100:
            obj.release()
            if obj in shots:
                shots.remove(obj)
            elif obj in enemies:
                enemies.remove(obj)
            elif obj in tunnel_segs:
                tunnel_segs.remove(obj)


    t = time()
    for enemy in enemies:
        if enemy.move:
            enemy.vel(enemy.move(t), avel=(2,0,0))
