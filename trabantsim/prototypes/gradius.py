#!/usr/bin/env python3
# Inspired by Gradius, a 1985 Konami space side scroller.
#
# One of the more advanced prototypes. It does different types of enemies, different types of shots,
# generates tunnels and background, health, powerups such as homing missiles and shields, increases
# pace/pace as time goes, etc.


from trabant import *
from trabant.gameapi import htmlcol
from trabant.objects import gfxscale
from functools import partial
from random import choice, random


shipasc = r'''
    \
   / \
  /XXX>
XXXXX/
 //
//
===
XXXXX\
 XXXXXX>
XXXXX/
===
/X\
\XXXX\
 >XXXXXXX>
/XXXX/
\X/
===
 X
XXX
 XXXXXXXX\
XXXXXXXXXX>
 XXXXXXXX/
XXX
 X
===
    X
 /XXXXX\
\XXXXX/
 \XXXXXXXXX>
  >XXXXXXXXXXXX>
 /XXXXXXXXX>
/XXXXX\
 \XXXXX/
    X
===
  \
  X\
XXXX\
XXXXX
  XXX
XXXXXXX\
XXXXXXX/
  XXX
XXXXX
XXXX/
  X/
  /
===
  \
  X\
XXXXXXXXXXXX
XXXXXXXXXXXX
XXXXXXXXX>
XXXXXXXXXXXX
XXXXXXXXXXXX
  X/
  /
'''
shipascs = shipasc.split('\n===\n')

shipsym = r'''
  ^
 /X\
/X X\
'''

enemyasc = r'''
   /XXX
/XXXX
\XXXX
   \XXX
'''

def create_ship(ship, ship_idx):
    shot_cnt = {'bullet':0,'laser':1,'homing':0}
    pos = vec3()
    if ship:
        shot_cnt = ship.shot_cnt
        pos = ship.pos()
        [s.release() for s in ship.ship_parts]
    collisions(False)
    color = vec3(0.5,0.5,0.7)
    ship = wait_load(create_ascii_object(shipascs[ship_idx], pos=pos, col=color, mass=500))
    ship_radius = len(max(shipascs[ship_idx].split('\n'), key=lambda r:len(r))) / 2
    cockpit = wait_load(create_capsule(pos=pos, orientation=roty(pi/2), radius=1, col='#33b'))
    ship.joint(fixed_joint, cockpit)
    collisions(True)
    ship.ship_parts = cockpit.ship_parts = [ship, cockpit]
    ship.color = color
    ship.blink_start_t = None
    ship.blink_duration = 3
    ship.shield_type = None
    ship.radius = ship_radius
    ship.shields = []
    ship.max_health = 100*(ship_idx+1)
    ship.health = ship.max_health
    ship.hurt = cockpit.hurt = 100
    ship.immortal = False
    ship.shot_cnt = shot_cnt
    return ship

def create_star(x):
    global stars
    idist = random()*0.4+0.3
    stars += [create_sphere((x, 50-10*idist, random()*80-40), radius=0.15, col=vec3(idist,idist,idist), vel=(idist*-30*pace, 0, 0))]
    stars[-1].positionbased = True
def do_celestials():
    global stars
    create_star(100)
    timer_callback(0.2+random(), do_celestials)

def create_tunnel_seg(z, z2, x, rx, fx):
    verts,tris = [],[]
    d = +1 if z2>z else -1
    width,subs = 20,7
    for mx in range(subs+1):
        xx = mx*4-width/2
        rz = random()-0.5 + fx(rx+xx, -d)
        verts += [(xx,0,0), (xx,0,rz+z2-z)]
        t = mx*2
        tris += [0+t,2+t,1+t, 1+t,2+t,3+t] if d>0 else [0+t,1+t,2+t, 1+t,3+t,2+t]
    seg = create_mesh(verts, tris[:-6], pos=(x,0,z), vel=(-5*pace,0,0), col='#334', trigger=True)
    seg.hurt = 100
    seg.positionbased = True
    return seg
def create_tunnel_segs(x, rx, fx):
    global tunnel_segs
    for s in tunnel_segs:
        s.vel((-5*pace,0,0))
    height2,gap2 = 100/2,50/2
    segtop    = create_tunnel_seg(+height2, +gap2, x, rx, fx)
    segbottom = create_tunnel_seg(-height2, -gap2, x, rx, fx)
    tunnel_segs += [segtop, segbottom]
def do_tunnel():
    global tunnel_x
    fx = lambda x,vz: sin(x*0.05)*2
    if not tunnel_segs:
        create_tunnel_segs(-90, tunnel_x, fx)
        tunnel_x += 20
    px = tunnel_segs[-1].pos().x
    if px != 0: # Object not loaded yet.
        while px < 130:
            px += 20-0.5*pace
            tunnel_x += 20
            create_tunnel_segs(px, tunnel_x, fx)
    timer_callback(0.5, do_tunnel)

def slither(angle, circle_amount):
    '''Returns a position; use this function to create slithering motion.'''
    period = int(angle // pi)
    ca_half = 0.5*circle_amount + 0.5
    angle = (angle-pi*period - pi/2) * circle_amount + pi/2
    cosamp = 1 / cos(ca_half*pi)
    x = 2 * period + 1 + cosamp*cos(angle)
    sinoff = sin((ca_half*0.5+0.5)*2*pi)
    y = (sinoff + sin(angle)) * (-1 if period&1 else +1)
    return vec3(x, 0, y)

def shot_freq_idx(ship, ammo):
    return (ship.shot_cnt[ammo]-1)//5 + 1

def shot_mask(ship, ammo):
    return (((ship.shot_cnt[ammo]-1)%5) & 7) + 1

def shot_offsets(ship, pos, ammo):
    d = ship.radius-1
    offsets,smask = [], shot_mask(ship,ammo)
    if ammo == 'bullet':
        offsets += [vec3(d/2,0,-0.5)]
        offsets += [vec3(d/2,0,+0.5)] if smask>1 else []
        offsets += [vec3(  0,0,-2.0)] if smask>2 else []
        offsets += [vec3(  0,0,+2.0)] if smask>3 else []
        offsets += [vec3( -d,0,+0.0)] if smask>4 else []
    else:
        offsets += [vec3(  d,0, 0.0)] if smask&1 else []
        offsets += [vec3(  0,0,+3.0)] if smask&6 else []
        offsets += [vec3(  0,0,-3.0)] if smask&6 else []
        offsets += [vec3(d/2,0,+1.5)] if smask&4 else []
        offsets += [vec3(d/2,0,-1.5)] if smask&4 else []
    angf = {'bullet':lambda i,v:(+0.2,-0.2,+pi/2,-pi/2,+pi)[i], 'homing':lambda i,v:v.z*-0.5, 'laser': lambda i,v:0}
    return [(pos+o, angf[ammo](i,o)) for i,o in enumerate(offsets)]

def drop_shields(ship):
    for s in ship.shields:
        ship.ship_parts.remove(s)
        s.release()
    ship.immortal = False
    ship.shields = []

def powerup(power):
    global ship_idx,ship

    if power == 'ship':
        if ship_idx+1 < len(shipascs) and timeout(1,'create_ship',first_hit=True):
            ship_idx += 1
            ship = create_ship(ship, ship_idx)
        else:
            ship.health = ship.max_health
    elif power == 'spin_shield':
        if ship.shield_type != power:
            drop_shields(ship)
        else:
            for s in ship.shields:
                ship.ship_parts.remove(s) # Don't cause duplicates.
        ship.shield_type = power
        ship.vel((0,0,0)) # Freeze!
        shieldcnt = len(ship.shields)
        start_t = gametime() if not shieldcnt else ship.shields[0].start_t
        r = vec3(ship.radius*1.6+3,0,0)
        pos = ship.pos() + roty(2*pi*shieldcnt/(shieldcnt+1)) * r
        ship.shields += [create_sphere(pos=pos, radius=1.5, col='#c6a')]
        ship.shields[-1].health = 500
        ship.ship_parts += ship.shields
        for i,s in enumerate(ship.shields):
            s.hurt = 200
            s.distance = r
            s.angle = 2*pi*i / len(ship.shields)
            s.start_t = start_t
            s.shield_timeout = None
            s.ship_parts = []
            def move(shield, t):
                v = ship.pos() + roty(shield.angle + (t-shield.start_t)*10) * shield.distance - shield.pos()
                return v*30
            s.move = partial(move,s)
            s.amove = lambda t:None
    elif power == 'sphere_shield':
        drop_shields(ship)
        ship.shield_type = power
        ship.immortal = True
        collisions(False)
        ship.vel((0,0,0)) # Freeze!
        ship.shields = [wait_load(create_sphere(pos=ship.pos(), radius=ship.radius*1.3, col='#c6a5'))]
        ship.ship_parts += ship.shields
        for s in ship.shields:
            s.hurt = 400
            s.health = 1500
            s.move = None
            s.shield_timeout = gametime()+30
            s.ship_parts = []
            ship.joint(fixed_joint, s)
        collisions(True)
    elif power in ('bullet','laser','homing'):
        ship.shot_cnt[power] += 1
    elif power == 'health':
        ship.health = min(ship.health+50, ship.max_health)
    else:
        print('Unknown powerup:', power)
    sound(sound_clank)

def finalize_enemies(es):
    for enemy in es:
        enemy.positionbased = True
        enemy.ship_parts = (enemy,)
        enemy.health = enemy.health if hasattr(enemy,'health') else 20
        enemy.hurt = enemy.hurt if hasattr(enemy,'hurt') else 60
        enemy.immortal = False
        enemy.move = enemy.move if hasattr(enemy,'move') else None
        enemy.amove = enemy.amove if hasattr(enemy,'amove') else lambda t:None
        enemy.shoots = enemy.shoots if hasattr(enemy,'shoots') else None
    return es

def shoot(ship):
    p,new_shots = ship.pos(),[]
    if shoot_cooldown(ship, 'bullet'):
        for off,angle in shot_offsets(ship, p, 'bullet'):
            shot = create_capsule(off, radius=1, length=2, orientation=roty(angle+pi/2), vel=roty(angle)*vec3(90,0,0), col='#ab0', trigger=True, process=gfxscale(0.5))
            shot.positionbased = True
            shot.shooters = ship.ship_parts
            shot.hurt = 30
            shot.move = None
            new_shots += [shot]
        sound(sound_bang, p, volume=0.1)
    if shoot_cooldown(ship, 'homing'):
        es = [e for e in enemies if e.isloaded and e.pos().x > p.x+25]
        tgt = min(es, key=lambda e:(e.pos()-p).length2()) if es else None
        for off,angle in shot_offsets(ship, p, 'homing'):
            explode(off, vel=ship.vel()+roty(angle)*vec3(70,0,0), strength=0.5, volume=0.1)
            shot = create_box(off, orientation=roty(angle), side=3, vel=roty(angle)*vec3(70,0,0), mat='flat', col='#850', trigger=True, process=gfxscale((1, 0.3, 0.3)))
            shot.positionbased = True
            shot.shooters = ship.ship_parts
            shot.start_angle = angle
            shot.angle = angle
            shot.hurt = 100
            shot.move = None
            shot.start_t = gametime()
            shot.tgt = tgt
            def guide(shot, t):
                if not shot.tgt and random()>0.9:
                    es = [e for e in enemies if e.isloaded and e.pos().x > p.x+25]
                    if es:
                        shot.tgt = min(es, key=lambda e:(e.pos()-p).length2())
                if shot.tgt and shot.tgt in enemies:
                    v = shot.tgt.pos()-shot.pos()
                    shot.angle = shot.angle*0.95 + (angmod(-shot.angle+v.angle_y(vec3(1,0,0)))+shot.angle)*0.05
                    gfx_o = roty(shot.angle).rotate_x((t-shot.start_t)*4).rotate_y(-shot.start_angle)
                    phys_o = roty(shot.angle)
                    shot.orientation(gfx_o)
                    return phys_o*vec3(70,0,0)
                shot.tgt = None
                return shot.vel()
            shot.move = partial(guide, shot)
            shot.amove = lambda t:None
            new_shots += [shot]
    if shoot_cooldown(ship, 'laser'):
        scale = gfxscale((0.3, 0.3, 1)) # Scale X & Y; Z is height of capsule.
        laser_shots = []
        for off,angle in shot_offsets(ship, p, 'laser'):
            shot = create_capsule(off+roty(angle)*vec3(75,0, 0.0), orientation=roty(angle+pi/2), length=150, radius=0.8, col='#f0f', trigger=True, process=scale)
            shot.positionbased = False
            shot.shooters = ship.ship_parts
            shot.hurt = 60
            shot.move = None
            laser_shots += [shot]
        new_shots += laser_shots
        def remove_shot(laser_shots):
            for laser_shot in laser_shots:
                if laser_shot in shots:
                    shots.remove(laser_shot)
                    laser_shot.release()
        timer_callback(0.2, partial(remove_shot,laser_shots))
    return new_shots

def create_enemy():
    es = []
    etype,_ = pick_random({'snake':1, 'box':5, 'planet':5, 'capsule':5, 'redship':3}.items())
    if etype == 'snake':
        start_t = gametime()
        for i in range(20):
            ang = 2*pi*i/20
            tgtpos = lambda t,a: vec3(80-t*8,0,0) + slither(-1.2*t+a,1.4)*12
            def targetpos(enemy, t):
                enemy.t += (t-enemy.last_t) * (pace**0.5)
                enemy.last_t = t
                return tgtpos(enemy.t, enemy.angle)
            enemy = create_sphere(tgtpos(0,ang), radius=2.5, vel=(-8*(pace**0.5),0,0), col='#444' if i!=0 else '#911')
            enemy.last_t = start_t
            enemy.t = 0
            enemy.angle = ang
            enemy.move = partial((lambda e,t: (targetpos(e,t) - e.pos()) * 30), enemy)
            enemy.shoots = (i==0)
            es += [enemy]
    elif etype == 'box':
        enemy = create_box((80,0,random()*60-30), side=3, vel=(-10*pace,0,0), avel=rndvec())
        es += [enemy]
    elif etype == 'planet':
        enemy = create_sphere((80,0,random()*60-30), radius=2, vel=(-10*pace,0,0), avel=rndvec(), col='#00a')
        es += [enemy]
    elif etype == 'capsule':
        enemy = create_capsule((80,0,random()*60-30), radius=2, length=4, vel=(-10*pace,0,0), avel=rndvec())
        enemy.hurt = 200
        enemy.color = vec3(0.3,1,0.5)
        es += [enemy]
    elif etype == 'redship':
        enemy = create_ascii_object(enemyasc, pos=vec3(80,0,random()*60-30), vel=(-5*pace,0,0), col='#f00')
        enemy.move = lambda t: (-10*pace,0,10*sin(2*t))
        enemy.shoots = True
        es += [enemy]
    return finalize_enemies(es)

def create_powerup():
    powdesc = { 'health':           (20, partial(create_capsule,col='#d34',avel=(1,0,1))),
                'spin_shield':      ( 7, partial(create_sphere,col='#c6a')),
                'sphere_shield':    ( 4, partial(create_sphere,radius=2,col='#c6a5')),
                'bullet':           (15, partial(create_capsule,col='#ab0',avel=(1,0,1))),
                'laser':            (10, partial(create_capsule,length=3,radius=0.24,col='#f0f',avel=(1,0,1))),
                'homing':           ( 3, partial(create_box,side=(0.9,0.9,3),mat='flat',col='#850',avel=(0,0,1))) }
    pname,pfunc = pick_random(powdesc.items(), term=lambda i:i[1][0], name=lambda i:i[0], retval=lambda i:i[1][1])
    pup = pfunc(vec3(80,0,random()*50-25), vel=(-15*pace,0,0), trigger=True)
    pup.color = vec3(*htmlcol(pfunc.keywords['col'])[:3])
    return finalize_powerup(pup, pname)

def create_ship_powerup():
    pup = create_ascii_object(shipsym, vec3(80,0,random()*50-25), vel=(-15*pace,0,0), col=ship.color, avel=(0,0,1), trigger=True)
    pup.color = ship.color
    return finalize_powerup(pup, 'ship')

def finalize_powerup(pup, pname):
    pup.blink_start_t = gametime()
    pup.blink_duration = 100
    pup.power = pname
    pup.positionbased = True
    return [pup]

def shoot_cooldown(ship, ammo):
    freqidx = shot_freq_idx(ship, ammo)
    if not freqidx:
        return False
    freqs = { 'bullet':1.0, 'homing':1.5, 'laser':1.3 }
    return timeout(freqs[ammo]/(freqidx**0.5), 'shoot_'+ammo, first_hit=True)

def pick_random(coll, term=lambda x:x[1], name=lambda x:x[0], retval=lambda x:x[1]):
    v = random() * sum(term(v) for v in coll)
    n,r = None,None
    for cv in coll:
        n,r = name(cv),retval(cv)
        v -= term(cv)
        if v < 0:
            break
    return n,r


fps = 60 # 30 FPS is default, we want smoother movement in this prototype.
trabant_init(fps=fps)
fg(shadows=False)
gravity((0,0,0))
cam(distance=100)
# Enabling asynchronous loading mode is an optimization so we don't have to wait for all
# objects to load until code can move on. If you *need* an object completely loaded in
# this mode you call wait_load(obj) on each object to make sure loading is completed.
async_load()

pace = 1
stars,shots,enemies,powerups,tunnel_segs = [],[],[],[],[]
tunnel_x = -90
ship_steer = 0
ship_idx = 0
ship = create_ship(None, ship_idx)
for i in range(40):
    create_star((i-20)*5)
do_celestials()
do_tunnel()


while loop():

    tap = closest_tap(ship.pos()).pos3d()-ship.pos() if taps() else vec3()
    tap = tap.normalize() if tap.length() > 1 else tap
    x = keydir().x + tap.x
    z = keydir().y + tap.z
    ship.vel((40*x,0,40*z))
    ship.bounce_in_rect((-65,-0.1,-40), (65,0.1,40), spring=0)
    ship_steer = ship_steer*0.9 - z*0.1
    ship.orientation(rotx(ship_steer))

    if ship.blink_start_t:
        ship.health = ship.max_health
        ship.immortal = True
    else:
        ship.immortal = False

    now = gametime()
    for o in [ship]+powerups:
        if o.blink_start_t:
            t = now - o.blink_start_t
            f = abs(sin(t*8))
            o.col(o.color*f)
            if t > o.blink_duration:
                o.blink_start_t = None
                o.col(o.color)

    # Unlimited ammo, so always keep the finger on the trigger.
    new_shots = shoot(ship)

    if timeout(2/pace, 'create_enemy'):
        enemies += create_enemy()

    pace += 1 / (20*fps) # Make things one unit harder+faster every 20 seconds.

    pup = None
    if timeout(3, 'create_powerup'):
        powerups += create_powerup()
    if timeout(10, 'create_ship_powerup'):
        powerups += create_ship_powerup()

    if timeout(1, 'enemy_shoots'):
        es = [e for e in enemies if e.isloaded and e.shoots]
        if es:
            enemy = choice(es)
            p = enemy.pos()
            shot = create_capsule(p+vec3(-8,0, 0), radius=1, length=2, orientation=roty(pi/2), vel=vec3(-45*pace,0,0), col='#ff4', trigger=True, process=gfxscale(0.5))
            shot.positionbased = True
            shot.shooters = enemy.ship_parts
            shot.hurt = 40
            shot.move = None
            new_shots += [shot]

    shots += new_shots

    died = False
    for obj1,obj2,force,pos in collisions():
        withpowerup = hasattr(obj2, 'power')
        if withpowerup:
            continue
        hasimmortal = hasattr(obj1, 'immortal')
        if hasimmortal and obj1.immortal:   # Shield and the like.
            continue
        if obj1 in ship.ship_parts or obj1 in enemies:
            if obj2 in shots and obj2.shooters == obj1.ship_parts:
                continue
            if not obj1.ship_parts:
                continue
            hull = obj1.ship_parts[0]
            if hull.immortal:
                continue
            hull.health -= obj2.hurt
            if hull.health <= 0:
                explode(pos, obj1.vel(), obj1.hurt/50, volume=0.1)
                if hull == ship:
                    ship.health = 10000
                    died = True
                else:
                    enemies.remove(hull)
                    for o in obj1.ship_parts:
                        obj1.release()
        elif obj1 in shots:
            if obj1.positionbased and obj2 not in obj1.shooters: # E.g. laser shots don't explode.
                explode(pos, obj1.vel()*0.5, obj1.hurt/50, volume=0.1)
                shots.remove(obj1)
                obj1.release()
        elif obj1 in powerups and obj2 in ship.ship_parts and obj2 not in ship.shields:
            powerup(obj1.power)
            powerups.remove(obj1)
            obj1.release()

    if died:
        #drop_shields(ship)
        ship.pos((0,0,0))
        if ship_idx > 0:
            ship_idx -= 1
            ship = create_ship(ship, ship_idx)
        for ammo,cnt in ship.shot_cnt.items():
            ship.shot_cnt[ammo] = max(cnt-1,0)
        if sum(ship.shot_cnt.values()) == 0:
            ship.shot_cnt['laser'] = 1
        ship.immortal = True
        ship.blink_start_t = gametime()
        pace = max(pace-3, 1)

    for s in ship.shields:
        if s.shield_timeout and s.shield_timeout-gametime() <= 0:
            drop_shields(ship)
            break


    for obj in stars[:len(stars)//4+1] + powerups[:len(powerups)//4+1] + shots[:len(shots)//4+1] + enemies[:len(enemies)//4+1] + tunnel_segs[:2]:
        if not obj.positionbased: # Laser is not "position based."
            continue
        x,_,z = obj.pos()
        if x < -100 or x > +150 or z < -70 or z > +70:
            obj.release()
            if obj in stars:
                stars.remove(obj)
            elif obj in powerups:
                powerups.remove(obj)
            elif obj in shots:
                shots.remove(obj)
            elif obj in enemies:
                enemies.remove(obj)
            elif obj in tunnel_segs:
                tunnel_segs.remove(obj)


    t = gametime()
    for o in shots+enemies+ship.shields:
        if o.move:
            o.vel(o.move(t), avel=o.amove(t))
