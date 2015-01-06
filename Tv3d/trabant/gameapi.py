#!/usr/bin/env python3

import socket
from trabant.math import vec3


sock = None
proc = None


def open(bg='#000', fg='#b59', fps=30, fov=60, addr='localhost:2541'):
	_opencom(addr)
	r,g,b = _htmlcol(bg)
	set('Ui.3D.ClearRed', r)
	set('Ui.3D.ClearGreen', g)
	set('Ui.3D.ClearBlue', b)
	r,g,b = _htmlcol(fg)
	set('Ui.PenRed', r)
	set('Ui.PenGreen', g)
	set('Ui.PenBlue', b)
	set('Physics.FPS', int(fps))
	set('Ui.3D.FOV', float(fov))

def close():
	_closecom()

def reset():
	'''Kill all objects and joysticks. Set some default values.'''
	cmd('reset')

def debug(enable):
	set('Debug.Enable', enable)

def waitload(oid):
	cmd('wait-until-loaded %i' % oid)

def opened():
	return sock != None


def cam(angle, distance, target_oid, pos, fov):
	if angle:
		set('Ui.3D.CamAngleX', float(angle.x))
		set('Ui.3D.CamAngleY', float(angle.y))
		set('Ui.3D.CamAngleZ', float(angle.z))
	if distance:
		set('Ui.3D.CamDistance', float(distance))
		set('Ui.3D.ClipFar', float(distance*20))
	if target_oid:
		set('Ui.3D.CamTargetObject', target_oid)
	if pos:
		set('Ui.3D.CamLookAtX', float(pos.x))
		set('Ui.3D.CamLookAtY', float(pos.y))
		set('Ui.3D.CamLookAtZ', float(pos.z))
	if fov:
		set('Ui.3D.FOV', float(fov))

def fog(distance):
	set('Ui.3D.FogDistance', float(distance))

def gravity(g):
	set('Physics.GravityX', float(g.x))
	set('Physics.GravityY', float(g.y))
	set('Physics.GravityZ', float(g.z))

def explode(pos,vel):
	cmd('explode %s %s' % (_args2str(pos,'0 0 0'), _args2str(vel,'0 0 0')))

def playsound(snd, pos, vel):
	cmd('play-sound %s %s %s' % (snd, _args2str(pos,'0 0 0'), _args2str(vel,'0 0 0')))


def pop_collisions():
	return cmd('pop-collisions')

def taps():
	return cmd('get-touch-drags')

def accelerometer():
	return cmd('get-accelerometer')

def create_joystick(x,y):
	return cmd('create-joystick %f %f' % (x,y), int)

def joystick_data():
	return cmd('get-joystick-data')

def get_aspect_ratio():
	return cmd('get-aspect-ratio', float)

def clearprepphys():
	cmd('clear-phys')

def initphysbox(orientation, pos, size):
	cmd('prep-phys-box %f %f %f %f %f %f %f %f %f %f' % tuple((pq for pq in orientation[:]+pos[:]+size[:])))

def initphyssphere(orientation, pos, radius):
	cmd('prep-phys-sphere %f %f %f %f %f %f %f %f' % tuple((pq for pq in orientation[:]+pos[:]+[radius])))

def initphysmesh(orientation, pos, vertices, indices):
	setmesh(vertices, indices)
	prepphysmesh(orientation, pos)

def initgfxmesh(orientation, pos, vertices, indices):
	setmesh(vertices, indices)
	prepgfxmesh(orientation, pos)

def prepphysmesh(orientation, pos):
	cmd('prep-phys-mesh %f %f %f %f %f %f %f' % tuple((pq for pq in orientation[:]+pos[:])))

def prepgfxmesh(orientation, pos):
	cmd('prep-gfx-mesh %f %f %f %f %f %f %f' % tuple((pq for pq in orientation[:]+pos[:])))

def setmesh(vertices, indices):
	cmd('set-vertices %s' % ' '.join(str(f) for v in vertices for f in v))
	cmd('set-indices %s' % ' '.join((str(i) for i in indices)))

def createobj(static):
	static = 'static' if static else 'dynamic'
	return cmd('create-object %s' % static, int)

def releaseobj(oid):
	cmd('delete-object %i' % oid)

def release_all_objects():
	cmd('delete-all-objects')

def create_engine(oid, engine_type, max_velocity, offset, sound):
	return cmd('create-engine %i %s %s %s %s' % (oid, engine_type, _args2str(max_velocity, '0 0'), _args2str(offset, '0 0'), sound if sound else 'none'), int)

def create_joint(oid, joint_type, oid2, axis):
	return cmd('create-joint %i %s %i %s' % (oid, joint_type, oid2, _args2str(axis)), int)


def pos(oid, pos):
	return getsetoidcmd('position', oid, pos)

def orientation(oid, orientation):
	return getsetoidcmd('orientation', oid, orientation)

def vel(oid, velocity):
	return getsetoidcmd('velocity', oid, velocity)

def avel(oid, angular_velocity):
	return getsetoidcmd('angular-velocity', oid, angular_velocity)

def weight(oid, w):
	return getsetoidcmd('weight', oid, w)

def col(oid, color):
	return getsetoidcmd('color', oid, _htmlcol(color))

def set_engine_force(oid, eid, xyz):
	return getsetoidcmd('engine-force', oid, eid, xyz)


def getsetoidcmd(name, oid, *args):
	l = []
	if args != (None,):
		for arg in args:
			l += [str(a) for a in arg if a != None]
	result = cmd('%s %i %s' % (name, oid, ' '.join(l)))
	if not l and result:
		result = [float(r) for r in result.split()]
		return result[0] if len(result) == 1 else vec3(*result)

def cmd(c, return_type=str):
	sock.send((c+'\n').encode())
	result = sock.recv(1024).decode()
	return return_type(result[3:]) if result.startswith('ok\n') else result

def set(setting, value):
	if type(value) == bool:
		value = str(value).lower()
	cmd('#%s %s' % (setting, str(value)))


########################################


def _opencom(addr):
	global sock,proc
	if sock:
		return
	_run_local_server(addr)
	ip,port = addr.split(':')
	try:
		sock = socket.socket()
		sock.connect((ip,int(port)))
	except socket.error:
		sock = None
		print('TrabantServer not available on %s.' % addr)

def _closecom():
	global sock,proc
	if sock:
		if proc:
			cmd('quit')
		sock.close()
		sock = None
	if proc:
		print('Closing TrabantServer...')
		proc.kill()
		proc.wait()
		proc = None

def _args2str(args, default=''):
	if not args:
		return default
	' '.join(str(arg) for arg in args)

def _run_local_server(addr):
	import os
	if 'localhost' in addr and os.name in ('nt','darwin'):
		import subprocess
		try:
			proc = subprocess.Popen(['TrabantServer', addr], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
		except:
			print('Error: TrabantServer could not be started.')
		import atexit
		import signal
		def ctrlc(s,f):
			_closecom()
			import sys
			sys.exit(0)
		signal.signal(signal.SIGINT, ctrlc)
		atexit.register(_closecom)

def _htmlcol(col):
	if col:
		if type(col) == str:
			assert col[0]=='#'
			if len(col) == 4:
				col = [int(c,16)/16 for c in col[1:]]
			elif len(col) == 7:
				col = [int(col[i:i+2],16)/16 for i in range(1,7,2)]
	return col
