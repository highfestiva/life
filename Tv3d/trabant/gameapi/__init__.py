#!/usr/bin/env python3


sock = None
proc = None


def open(bg='#000', fg='#b59', fps=30, fov=60, addr='localhost:2541'):
	opencom(addr)
	r,g,b = _htmlcol(bg)
	set('Ui.3D.ClearRed', r)
	set('Ui.3D.ClearGreen', g)
	set('Ui.3D.ClearBlue', b)
	r,g,b = _htmlcol(fg)
	set('Ui.3D.PenRed', r)
	set('Ui.3D.PenGreen', g)
	set('Ui.3D.PenBlue', b)
	set('Physics.FPS', int(fps))
	set('Ui.3D.FOV', float(fov))

def close():
	closecom()

def reset():
	'''Kill all objects and joysticks. Set some default values.'''
	cmd('reset')

def debug(enable):
	set('Debug.Enable', enable)

def waitload(oid):
	return cmd('wait-until-loaded %i' % oid)

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
	cmd('Phys.GravityX %f' % g.x)
	cmd('Phys.GravityY %f' % g.y)
	cmd('Phys.GravityZ %f' % g.z)

def explode(pos,vel):
	cmd('explode %s %s' % (_args2str(pos,'0 0 0'), _args2str(vel,'0 0 0')))

def playsound(snd, pos, vel):
	cmd('play-sound %s %s %s' % (snd, _args2str(pos,'0 0'), _args2str(vel,'0 0')))


def pop_collisions():
	return cmd('pop-collisions')

def taps():
	return cmd('get-taps')

def accelerometer():
	return cmd('get-accelerometer')

def joystick_data():
	return cmd('get-joystick-data')


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
	cmd('set-vertices %s' % ' '.join((str(v) for v in vertices)))
	cmd('set-indices %s' % ' '.join((str(i) for i in indices)))

def createobj(static):
	static = 'static' if static else 'dynamic'
	return cmd('create-object %s' % static)

def releaseobj(oid):
	cmd('release-object %i' % oid)

def release_all_objects():
	cmd('release-all-objects')

def create_engine(oid, engine_type, max_velocity, offset, sound):
	return cmd('create-engine %i %s %s %s %s' % (oid, engine_type, _args2str(max_velocity, '0 0'), _args2str(offset, '0 0'), sound if sound else 'none'))

def create_joint(oid, joint_type, oid2, axis):
	return cmd('create-joint %i %s %i %s' % (oid, joint_type, oid2, _args2str(axis)))


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
	if args:
		l = []
		for arg in args:
			try:	l += [str(a) for a in arg if a != None]
			except:	l += [str(arg)]
		cmd('set-%s %i %s' % (name, oid, ' '.join(l)))
	else:
		return cmd('get-%s %i' % (name, oid))

def cmd(c):
	global sock
	if sock and s:
		try:
			sock.send((s+'\n').encode())
			return sock.recv(1024).decode()
		except socket.error as e:
			print('tv3d connection failure:', e)
			sock = None

def set(setting, value):
	if type(value) == bool:
		value = str(value).lower()
	cmd('#%s %s' % (setting, str(value)))

def opencom(addr):
	global sock,proc
	if sock:
		return
	_run_local_server(addr)
	import socket
	ip,port = addr.split(':')
	try:
		sock = socket.socket()
		sock.connect((ip,int(port)))
	except socket.error:
		sock = None
		print('TrabantServer not available on %s.' % addr)

def closecom():
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
			closecom()
			import sys
			sys.exit(0)
		signal.signal(signal.SIGINT, ctrlc)
		atexit.register(closecom)

def _htmlcol(col):
	if col:
		if type(col) == str:
			assert col[0]=='#'
			if len(col) == 4:
				col = [int(c,16)/16 for c in col[1:]]
			elif len(col) == 7:
				col = [int(col[i:i+2],16)/16 for i in range(1,7,2)]
	return col
