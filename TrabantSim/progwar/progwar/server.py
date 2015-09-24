from progwar.socket import socket as pw_socket,proto_version
from progwar.tank import init_tank_obj,create_tank_obj,process_tank_floor
from time import time as now
from trabant import *
from traceback import print_exc


users = {}
shots = set()
pos_cache = {}
blip_update_interval = 1
tank_update_interval = 0.3
round_wait = 20 # First time we wait a little longer.
normal_round_wait = 5
round_t = None
round_started = False
tanks_per_person = 3
shoot_interval = 5
_throttle_on = True


class user:
    def __init__(self, username, addr, col):
        self.username = username
        self.addr = addr
        self.col = col
        self.tanks = []
        self.comm_t = now()
        self.disconnected = False
        self.score = 0
    def send(self, sock, data):
        if not self.disconnected:
            sock.sendto(data, self.addr)
    def create_tank(self, sock, pos):
        tank = create_tank_obj(pos, col=self.col)
        tank.health = 100
        tank.target_t = now()-shoot_interval+random()*1
        tank.target_dir = getpos(tank)
        tank.shoot_t = tank.target_t
        tank.shoot_yaw = None
        tank.shoot_pitch = None
        tank.shoot_dmg_type = None
        tank.create_engine(push_abs_engine, strength=1.3)
        self.tanks += [tank]
    def kill_tank(self, sock, tank):
        users_out = len([1 for u in users.values() if not u.tanks])
        self.tanks.remove(tank)
        tank.release()
        if sock:
            post_tanks(sock, force=True)
            if not self.tanks:
                self.addscore(users_out)
    def kill_all_tanks(self, sock):
        for tank in list(reversed(self.tanks)):
            self.kill_tank(None,tank)
        if sock:
            post_tanks(sock, force=True)
    def addscore(self, i):
        self.score += i


def connected_users():
    return [u for u in users.values() if not u.disconnected]

def throttle(on):
    global _throttle_on
    if _throttle_on != on:
        _throttle_on = on
        from trabant.gameapi import setvar
        setvar('Physics.FPS', 30 if on else 5)
        setvar('Ui.3D.CamRotateZ', 0.01 if on else 0.0)

def getpos(obj):
    if not obj in pos_cache:
        pos_cache[obj] = obj.pos()
        pos_cache[obj].health = obj.health if hasattr(obj,'health') else 0
    return pos_cache[obj]

def create_shot(tankpos, tankvel, direction, dmg_type):
    global shots
    shot = create_sphere(tankpos+direction*8, vel=direction*30+tankvel, radius=1, col='#0ff')
    shot.dmg_type = dmg_type
    shots.add(shot)

def control_tanks():
    t = now()
    for u in users.values():
        for tank in u.tanks:
            if t-tank.target_t > 0.2:
                tank.engine[0].force(tank.target_dir.with_z(0).limit(1)*7)
                tank.target_t = t
            if tank.shoot_dmg_type and t-tank.shoot_t > shoot_interval:
                create_shot(getpos(tank), tank.vel(), quat().rotate_z(tank.shoot_yaw).rotate_x(tank.shoot_pitch)*vec3(0,1,0), tank.shoot_dmg_type)
                tank.shoot_dmg_type = None
                tank.shoot_t = t
            tank.update_tank(tank.vel())

def handle_timeouts(sock):
    t = now()
    for u in connected_users():
        if t-u.comm_t > 5:
            print('Disconnected user %s due to inactivity.' % u.username)
            u.send(sock, 'disconnect')
            u.disconnected = True

def post_blips(sock):
    for to_user in connected_users():
        blips = []
        for u in users.values():
            if to_user != u:
                for tank in u.tanks:
                    blips += [('t',getpos(tank))]
        for s in shots:
            blips += [('s',getpos(s))]
        info = ' '.join(['%s%i,%i,%i'%(t,b.x,b.y,b.z) for t,b in blips if b])
        to_user.send(sock, 'b '+info)

def post_tanks(sock, force):
    for u in connected_users():
        tanks = []
        for tank in u.tanks:
            tanks += [getpos(tank)]
        if tanks or force:
            info = ' '.join(['%g,%g,%g,%i'%(p.x,p.y,p.z,p.health) for p in tanks])
            u.send(sock, 'p '+info)

def send_positions(sock):
    if timeout(blip_update_interval, timer=1):
        post_blips(sock)
    elif timeout(tank_update_interval, timer=2):
        post_tanks(sock, force=False)

def handle_explosions(sock):
    explosions = []
    for o,o2,f,pos in collisions():
        if o in shots:
            shots.remove(o)
            o.release()
            explosions += [pos]
            explode(pos, strength=5)
            if o.dmg_type == 'damage':
                for u in users.values():
                    for t in u.tanks[:]:
                        d = getpos(t)-pos
                        dl = d.length()
                        damage = 30 - dl
                        if damage > 0:
                            t.health -= damage
                            if t.health > 0:
                                t.vel(t.vel() + d*(damage/dl))
                            else:
                                u.kill_tank(sock, t)
    if explosions:
        for u in connected_users():
            u.send(sock, 'e '+' '.join('%g,%g,%g'%(e.x,e.y,e.z) for e in explosions))

def handle_client_data(sock, data, addr):
    u = users[addr]
    u.comm_t = now()
    if data.startswith('connect'):
        handle_login(sock, data, addr)
        return
    [tank.shoot_dmg_type for tank in u.tanks]   # Assume holding off on shots for timing.
    for line in data.split('\n'):
        cmd,idx,args = line.split()
        idx = int(idx)
        tank,args = u.tanks[idx] if idx < len(u.tanks) else None,eval(args)
        if cmd == 't':
            if tank:
                tank.target_dir = vec3(*args)
        elif cmd == 's':
            if tank:
                tank.shoot_yaw,tank.shoot_pitch,tank.shoot_dmg_type = args
        elif cmd == 'p':
            pass
        else:
            print('WARN: invalid user command:', line)

def _readsettings():
    configname = '../progwar/server.ini'
    return dict([[w.strip() for w in line.split('=')] for line in open(configname) if line.strip()])

def handle_login(sock, data, addr):
    if data.startswith('connect'):
        if not data.startswith('connect %s ' % proto_version):
            print('WARN: user tries to connect with old version:', data)
            sock.sendto('server/client version mismatch', addr)
            return
    else:
        print('WARN: disconnected user sends us non-connect.')
        sock.sendto('disconnect', addr)
        return
    words = data.split()
    username = words[2]
    try:
        password = _readsettings()[username+'_pw']
    except:
        print('WARN: no such user %s (connected from %s).' % (username,str(addr)))
        sock.sendto('bad user/password', addr)
        return
    if password != words[3]:
        print('WARN: user %s from %s send bad password.' % (username,str(addr)))
        sock.sendto('bad user/password', addr)
        return
    if addr not in users:
        for u in users.values():
            if u.username == username:
                users.pop(u.addr)
                users[addr] = u
                u.addr = addr
                u.col = words[4]
                u.disconnected = False
                u.comm_t = now()
                break
        else:
            users[addr] = user(username,addr,words[4])
    users[addr].send(sock, 'ok')
    msg(sock, 'User %s logged in from %s.' % (username,str(addr)))
    if not round_started and round_t:
        users[addr].send(sock, 'm Round starts in %i seconds.' % (round_t-now()))
    if len(users) >= 1:
        throttle(True)

def fall_off_world(sock):
    for u in users.values():
        for tank in list(u.tanks):
            if getpos(tank).z < -100:
                u.kill_tank(sock, tank)
                break
    for s in list(shots):
        if getpos(s).z < -100:
            shots.remove(s)
            s.release()

def msg(sock, s):
    print(s)
    for u in connected_users():
        u.send(sock, 'm '+s)

def print_score(sock):
    s = ['Score:']
    for u in users.values():
        s += ['  %15s  %2i' % (u.username,u.score)]
    msg(sock, '\n'.join(s))


def handle_round(sock):
    global round_started, round_t, round_wait, normal_round_wait
    if not round_started and round_t and round_t-now() <= 0:
        [u.kill_all_tanks(sock) for u in users.values()]
        cu = connected_users()
        [users.pop(u.addr) for u in list(users.values()) if u not in cu]  # Drop all disconnected users.
        [u.send(sock, 'b ') for u in users.values()]    # Empty all blips.
        i,c = 0,len(cu)*(tanks_per_person+6)
        for u in cu:
            for _ in range(tanks_per_person):
                pos = quat().rotate_z(2*pi*i/c)*vec3(0,100,0)
                u.create_tank(sock, pos)
                i += 1
            i += 6  # Some space between players.
        msg(sock, 'GO!')
        round_started = True
    elif round_started:
        if not connected_users():
            msg(sock, 'All users logged out, no winner appointed.')
            round_started = False
            round_t = None
        else:
            # Check if someone won.
            u = [u for u in users.values() if u.tanks]
            if len(u) == 0:
                msg(sock, 'Game is a draw, no winner appointed.')
                round_started = False
                round_t = None
            if len(u) == 1:
                msg(sock, '%s wins the round!' % u[0].username)
                round_started = False
                round_t = None
            if not round_started:
                [u.addscore(len(users)) for u in users.values() if u.tanks]
                print_score(sock)
    elif not round_started and not round_t:
        if len(connected_users()) >= 2:
            round_t = now()+round_wait
            msg(sock, 'Round starts in %i seconds...' % round_wait)
            round_wait = normal_round_wait
    if len(connected_users()) < 1:
        throttle(False)

def run():
    sock = pw_socket()
    sock.bind(('0.0.0.0',4554))
    print('Server running at %s.' % pw_socket.gethostname())
    settings = _readsettings()
    trabant_init(addr=settings['sim_addr'])
    fg(outline=False)
    cam(distance=250, angle=(-pi/6,0,0))
    gravity((0,0,-9), friction=0)
    create_box((0,0,-140), side=240, static=True, process=process_tank_floor)
    init_tank_obj()
    while loop():
        try:
            pos_cache.clear()
            handle_timeouts(sock)
            data,addr = sock.recv()
            if data:
                if addr not in users:
                    handle_login(sock, data, addr)
                else:
                    handle_client_data(sock, data, addr)
            control_tanks()
            handle_explosions(sock)
            send_positions(sock)
            fall_off_world(sock)
            handle_round(sock)
        except ConnectionResetError:
            pass
        except Exception as e:
            print_exc()

if __name__ == '__main__':
    run()
