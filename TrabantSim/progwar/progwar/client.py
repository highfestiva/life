from progwar.socket import socket as psocket
from progwar.tank import init_tank_obj,create_tank_obj,process_tank_floor
from time import time as now
from trabant import *


my_tanks = []
blips = []
_blip_objs = []


class coord:
    def __init__(self, ctype, pos):
        self.type = ctype
        self.pos = pos
        self.obj = None

class tank(coord):
    def __init__(self, pos, health):
        super().__init__('tank', pos)
        self.health = health
        self.direction = None
        self.shoot_yaw = None
        self.shoot_pitch = None
        self.shoot_dmg_type = None
        self.skipped_shot = True
    def drive(self, direction):
        self.direction = direction
    def shoot(self, yaw, pitch, dmg_type):
        self.shoot_yaw = yaw
        self.shoot_pitch = pitch
        self.shoot_dmg_type = dmg_type


def _readsettings():
    import sys
    txtfiles = [a for a in sys.argv if a.endswith('.ini')]
    settingsfile = txtfiles[0] if txtfiles else 'client.ini'
    info = dict([[w.strip() for w in line.split('=')] for line in open(settingsfile) if line.strip()])
    return info

def handle_server_data(data):
    args = data.split()
    cmd,args = args[0],args[1:]
    if cmd == 'b':
        global blips
        to_coord = lambda t,x,y,z: coord('tank' if t=='t' else 'shot', vec3(*[int(i) for i in [x,y,z]]))
        bs = [to_coord(b[0],*b[1:].split(',')) for b in args]
        def update_blips(ctype, bs):
            keepers = []
            remaining = set(b for b in blips if b.type==ctype)
            for b in [c for c in bs if c.type==ctype]:
                if remaining:
                    closest = min(remaining,key=lambda t:(t.pos-b.pos).length2())
                    closest.pos = b.pos
                    remaining.remove(closest)
                    keepers += [closest]
                else:
                    keepers += [b]
            return keepers
        blips = update_blips('tank',bs) + update_blips('shot',bs)

    elif cmd == 'e':
        # Uncomment to show explosions client-side.
        #[explode(vec3(*[float(f) for f in pos.split(',')]),strength=5) for pos in args]
        pass
    elif cmd == 'm':
        print(data[2:])
    elif cmd == 'p':
        global my_tanks
        crds = []
        for p in args:
            n = p.split(',')
            crds += [vec3(*[float(f) for f in n[:3]])]
            crds[-1].health = int(n[3])
        default_crd = vec3(1000,1000,1000)
        if len(crds) < len(my_tanks):
            suspect_tanks = sorted(my_tanks, key=lambda t: (min(crds,default=default_crd,key=lambda c:(c-t.pos).length2())-t.pos).length2())
            killed_tanks = list(suspect_tanks)[len(crds):]
            for killed in killed_tanks:
                my_tanks.remove(killed)
        if len(crds) > len(my_tanks):
            tank_crds = [t.pos for t in my_tanks]
            suspect_crds = sorted(crds, key=lambda c: (min(tank_crds,default=default_crd,key=lambda tc:(tc-c).length2())-c).length2())
            new_crds = list(suspect_crds)[len(my_tanks):]
            for nc in new_crds:
                crds.remove(nc)
                my_tanks += [tank(nc,nc.health)]
        for i,c in enumerate(crds):
            my_tanks[i].pos = c
            my_tanks[i].health = c.health
    else:
        print('WARN: <unknown server command>')

def send_tanks(sock):
    any_shoot = True
    if not timeout(0.2):
        any_shoot = False
        if not [1 for t in my_tanks if t.skipped_shot and t.shoot_dmg_type]:
            return
    s = []
    for i,t in enumerate(my_tanks):
        if t.direction:
            s += ['t %i %i,%i,%i' % (i,t.direction.x,t.direction.y,t.direction.z)]
            t.direction = None
        t.skipped_shot = True
        if (any_shoot or t.skipped_shot) and t.shoot_dmg_type:
            s += ['s %i %f,%f,"%s"' % (i,t.shoot_yaw,t.shoot_pitch,t.shoot_dmg_type)]
            t.shoot_dmg_type = None
            t.skipped_shot = False
    if s:
        sock.send('\n'.join(s))
    else:
        sock.send('p 0 "?"')

def update_gfx():
        global my_tanks,_blip_objs
        unused = _blip_objs
        _blip_objs = set()
        for blip in my_tanks+blips:
            istank = (blip.type == 'tank')
            col = '#0f0' if blip in my_tanks else '#f00'
            col = col if istank else '#0ff'
            pos = blip.epos if hasattr(blip,'epos') else blip.pos
            vel = blip.vel if hasattr(blip,'vel') else None
            if blip.obj:
                blip.obj.pos(pos)
                if vel:
                    blip.obj.vel(vel)
                    if istank:
                        blip.obj.update_tank(vel)
                unused.remove(blip.obj)
            else:
                if istank:
                    blip.obj = create_tank_obj(pos, vel=vel, col=col)
                else:
                    blip.obj = create_sphere(pos, vel=vel, col=col)
            _blip_objs.add(blip.obj)
        for u in unused:
            u.release()

def join_game(server_addr, update):
    global my_tanks

    settings = _readsettings()
    trabant_init(addr=settings['sim_addr'])
    settings.pop('sim_addr')
    fg(outline=False)
    cam(distance=250, angle=(-pi/6,0,0))
    gravity((0,0,-9), friction=0)
    create_box((0,0,-140), side=240, static=True, process=process_tank_floor)
    init_tank_obj()

    # Might as well crash right away, before negotiations. Consider it a unit test. :)
    my_tanks += [tank(vec3(1,2,3),1)]
    update([coord('tank',vec3(10,20,30))])
    my_tanks.clear()

    # Login.
    sock = psocket()
    login_result,_ = sock.login((server_addr,4554), **settings)
    print('Login: %s' % login_result)
    if login_result != 'ok':
        return

    # Good to go.
    while loop():
        collisions()
        data = 1
        while data:
            data,_ = sock.recv()
            if data:
                handle_server_data(data)
        send_tanks(sock)
        update(blips)
        update_gfx()
