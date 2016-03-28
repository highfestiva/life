#!/usr/bin/env python3

import socket
import trabant.socket
from trabant.math import vec3,quat


sock = None
proc = None
osname = '???'
_cached_vertices,_cached_indices = [],[]
_connecting = False


def init(bg='#000', fg='#b59', fps=30, fov=60, addr='localhost:2541', connect_retries=2, restart=True):
	if ':' not in addr:
		addr += ':2541'
	if not _opencom(addr, connect_retries):
		raise Exception('unable to connect to simulator at %s' % addr)
	if restart:
		release_all_objects()
		reset()	# Kill all joysticks, set all default values, etc.
		setbgcolor(bg)
		setpencolor(fg)
		setvar('Physics.FPS', int(fps))
		setvar('Ui.3D.FOV', float(fov))
	return True

def close():
	_closecom()

def reset():
	'''Kill all objects and joysticks. Set some default values.'''
	cmd('reset')

def debugsim(enable):
	setvar('Debug.Enable', enable)

def userinfo(message):
	setvar('Game.UserMessage', '"%s"' % message)

def areloaded(oids):
	c = 'are-loaded %s' % ','.join('%i'%oid for oid in oids)
	conv = lambda r: [s=='1' for s in r.split(',')]
	return cmd(c, conv)

def waitload(oid):
	cmd('wait-until-loaded %i' % oid)

def opened():
	return sock != None


def cam(angle, distance, target_oid, pos, fov, relative_angle, smooth):
	if angle:
		setvar('Ui.3D.CamAngleX', float(angle.x))
		setvar('Ui.3D.CamAngleY', float(angle.y))
		setvar('Ui.3D.CamAngleZ', float(angle.z))
	if distance != None:
		setvar('Ui.3D.CamDistance', float(distance))
		setvar('Ui.3D.ClipFar', float((distance+10)*20))
	if target_oid != None:
		setvar('Ui.3D.CamTargetObject', target_oid)
	if pos:
		setvar('Ui.3D.CamLookAtX', float(pos.x))
		setvar('Ui.3D.CamLookAtY', float(pos.y))
		setvar('Ui.3D.CamLookAtZ', float(pos.z))
	if fov:
		setvar('Ui.3D.FOV', float(fov))
	if relative_angle != None:
		setvar('Ui.3D.CamAngleRelative', bool(relative_angle))
	if smooth != None:
		smooth = float(smooth)
		if smooth < 0 or smooth >= 1:
			print('Camera smooth %f is outside range [0,1). Causing problems?' % smooth)
		setvar('Ui.3D.CamSmooth', float(smooth))

def light(angle):
	if angle:
		setvar('Ui.3D.LightAngleX', float(angle.x))
		setvar('Ui.3D.LightAngleY', float(angle.y))
		setvar('Ui.3D.LightAngleZ', float(angle.z))

def fog(near,far):
	setvar('Ui.3D.FogNear', float(near))
	setvar('Ui.3D.FogFar', float(far))

def gravity(g):
	setvar('Physics.GravityX', float(g.x))
	setvar('Physics.GravityY', float(g.y))
	setvar('Physics.GravityZ', float(g.z))

def bounce(factor):
	setvar('Physics.Bounce', float(factor))

def friction(factor):
	setvar('Physics.Friction', float(factor))

def explode(pos,vel,strength,volume):
	cmd('explode %s %s %f %f' % (_args2str(pos,'0 0 0'), _args2str(vel,'0 0 0'), float(strength), float(volume)))

def playsound(snd, pos, vel, volume):
	cmd('play-sound %s %s %s %f' % (snd, _args2str(pos,'0 0 0'), _args2str(vel,'0 0 0'), float(volume)))


def pop_collisions():
	return cmd('pop-collisions')

def keys():
	return cmd('get-keys')

def taps():
	return cmd('get-touch-drags')

def accelerometer():
	return cmd('get-accelerometer')

def mousemove():
	return cmd('get-mousemove')

def create_joystick(x,y,sloppy):
	sloppy = str(not not sloppy).lower()
	return cmd('create-joystick %f %f %s' % (x,y,sloppy), int)

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

def initphyscapsule(orientation, pos, radius, length):
	cmd('prep-phys-capsule %f %f %f %f %f %f %f %f %f' % tuple((pq for pq in orientation[:]+pos[:]+[radius,length])))

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
	verts = [f for v in vertices for f in v]
	global _cached_vertices, _cached_indices
	if verts != _cached_vertices or indices != _cached_indices:
		_break_any_cmd('set-vertices ', 'add-vertices ', '%g', verts)
		_break_any_cmd('set-indices ', 'add-indices ', '%i', indices)
		_cached_vertices,_cached_indices = verts,indices

def setbgcolor(col):
	r,g,b,a = _htmlcol(col)
	setvar('Ui.3D.ClearRed', r)
	setvar('Ui.3D.ClearGreen', g)
	setvar('Ui.3D.ClearBlue', b)

def setoutline(enable):
	setvar('Ui.3D.OutlineMode', enable)

def setpencolor(col):
	r,g,b,a = _htmlcol(col)
	setvar('Ui.PenRed', r)
	setvar('Ui.PenGreen', g)
	setvar('Ui.PenBlue', b)
	setvar('Ui.PenAlpha', a)

def createobj(static, trigger, mat, pos, orientation):
	phystype = 'static' if static else 'dynamic'
	phystype += '_trigger' if trigger else ''
	c = 'create-object %s %s %s %s' % (phystype, mat, _args2str(pos,'0 0 0'), _args2str(orientation,'0 0 0 0'))
	for _ in range(3):
		try:	return cmd(c, int)
		except:	pass

def cloneobjs(oid, static, mat, str_placements):
	static = 'static' if static else 'dynamic'
	c = 'create-clones %i %s %s %s' % (oid, static, mat, str_placements)
	s = cmd(c, lambda s:s)
	return (int(i) for i in s.split(','))

def releaseobj(oid):
	cmd('delete-object %i' % oid)

def release_all_objects():
	cmd('delete-all-objects')

def pickobjs(pos, direction, near, far):
	c = 'pick-objects %s %s %f %f' % (_args2str(pos,'0 0 0'), _args2str(direction,'0 0 0'), near, far)
	s = cmd(c, lambda s:s)
	if s:
		h = s.split(',')
		return [(int(h[i]),vec3(*[float(j) for j in h[i+1:i+4]])) for i in range(0,len(h),4)]
	return []

def create_engine(oid, engine_type, max_velocity, strength, friction, target_efcts):
	max_velocity = _args2str(max_velocity, '0 0')
	target_efcts = ' '.join('%i %f'%(oid,efct) for oid,efct in target_efcts) if target_efcts else ''
	return cmd('create-engine %i %s %s %f %f %s' % (oid, engine_type, max_velocity, strength, friction, target_efcts), int)

def create_joint(oid, joint_type, oid2, axis, stop, spring):
	return cmd('create-joint %i %s %i %s %s %s' % (oid, joint_type, oid2, _args2str(axis, '0 0 1'), _args2str(stop, '0 0'), _args2str(spring, '0 0')), int)

def addtag(oid, name, floats, strings, physidxs, engineidxs, meshidxs):
	cmd('set-tag-floats %s' % ','.join('%g'%float(f) for f in floats))
	cmd('set-tag-strings %s' % ' '.join(strings))
	cmd('set-tag-phys %s' % ','.join('%i'%i for i in physidxs))
	cmd('set-tag-engine %s' % ','.join('%i'%i for i in engineidxs))
	cmd('set-tag-mesh %s' % ','.join('%i'%i for i in meshidxs))
	cmd('add-tag %i %s' % (oid, name))


def pos(oid, pos):
	return getsetoidcmd('position', oid, pos)

def orientation(oid, orientation):
	return getsetoidcmd('orientation', oid, orientation)

def vel(oid, velocity):
	return getsetoidcmd('velocity', oid, velocity)

def avel(oid, angular_velocity):
	return getsetoidcmd('angular-velocity', oid, angular_velocity)

def mass(oid, w):
	return getsetoidcmd('mass', oid, w)

def col(oid, color):
	return getsetoidcmd('color', oid, _htmlcol(color))

def set_engine_force(oid, eid, xyz):
	return getsetoidcmd('engine-force', oid, (eid,), xyz)


def getsetoidcmd(name, oid, *args):
	l = []
	if args != (None,):
		for arg in args:
			l += [str(a) for a in arg if a != None]
	result = cmd('%s %i %s' % (name, oid, ' '.join(l)))
	if not l and result:
		result = [float(r) for r in result.split()]
		if len(result) == 1:
			return result[0]
		if len(result) == 3:
			return vec3(*result)
		if len(result) == 4:
			return quat(*result)
		return result

def cmd(c, return_type=str, errhandle=None):
	global sock
	#print(c)
	sock.send((c+'\n').encode())
	try:
		result = sock.recv(80*1024)
	except ConnectionResetError as e:
		if errhandle:
			errhandle(e)
		elif not _connecting:
			import sys
			sys.exit(1)
		raise e
	except socket.error as e:
		if errhandle:
			errhandle(e)
		elif str(e) == 'timeout':
			print('Simulator timed out!')
			sock = None	# Don't even try to close to avoid delay.
			import sys
			sys.exit(1)
		raise e
	#print(result)
	result = result.decode(errors='replace')
	if result.startswith('ok\n'):
		return return_type(result[3:])
	result = result.strip()
	print(result)
	result

def setvar(setting, value):
	if type(value) == bool:
		value = str(value).lower()
	cmd('#%s %s' % (setting, str(value)))


########################################


def _opencom(addr, retries):
	global _cached_vertices,_cached_indices
	if sock:
		return True
	_cached_vertices,_cached_indices = [],[]
	_tryconnect(addr, 1)
	if not sock:
		_run_local_sim(addr)
		_tryconnect(addr, 10 if proc else retries)
	if proc or sock:
		try:
			import atexit
			import signal
			def ctrlc(s,f):
				_closecom()
				import sys
				sys.exit(0)
			[signal.signal(s,ctrlc) for s in (signal.SIGABRT,signal.SIGINT,signal.SIGTERM)]
			atexit.register(_closecom)
		except ImportError:
			pass
	return sock != None

def _closecom():
	global sock,proc
	if sock:
		if proc:
			try:
				cmd('quit')
			except:
				pass
		try:
			sock.close()
		except:
			pass
		sock = None
	if proc:
		#print('Closing TrabantSim...')
		proc.kill()
		proc.wait()
		proc = None

def _break_any_cmd(start, add, fmt, values):
	c,s = start,''
	for v in values:
		s += ',' if s else ''
		s += fmt%v
		if len(s) > 1450:
			cmd(c+s)
			c,s = add,''
	if s:
		cmd(c+s)

def _args2str(args, default=''):
	if not args:
		return default
	return ' '.join(str(arg) for arg in args)

def _tryconnect(addr, retries):
	global sock,osname,_connecting
	_connecting = True
	ip,port = addr.split(':')
	for attempt in range(1,retries+1):
		try:
			if proc and attempt>1:
				import time
				time.sleep(1)
			sock = trabant.socket.socket()
			if retries>1 and attempt==retries:
				sock.settimeout(sock.timeout+1)
			sock.connect((ip,int(port)))
			def errhandle(exc):
				global sock
				sock = None
			osname = cmd('get-platform-name', errhandle=errhandle)	# To make sure UDP connection alright.
			sock.settimeout(sock.timeout+3)
			_connecting = False
			break
		except socket.error as e:
			sock = None
			if retries>1 and attempt==retries:
				print('TrabantSim not available on %s.' % addr)
				break

def _run_local_sim(addr):
	global proc
	if proc:
		return
	import sys
	if 'localhost' in addr and sys.platform in ('win32','darwin','linux'):
		if sys.platform == 'darwin':
			open_prefix,end_suffix,binname = ['/usr/bin/open','-W'],['--args'],'TrabantSim.app'
		elif sys.platform == 'linux':
			open_prefix,end_suffix,binname = [],[],'TrabantSim'
		else:
			open_prefix,end_suffix,binname = [],[],'TrabantSim.exe'
		for directory,rel in [('.',''), ('.','./'), ('..','./'), ('../sim','./'), ('../../bin/sim','./'), ('..','Final/'), ('..','Debug/')]:
			import os.path
			import subprocess
			curdir = os.path.abspath(os.path.curdir)
			try:
				os.chdir(directory)
				if not os.path.exists(rel+binname):
					raise Exception('trynextpath')
				args = open_prefix+[rel+binname]+end_suffix+[addr]
				#print(args)
				proc = subprocess.Popen(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
				break
			except:
				os.chdir(curdir)
				continue
		if not proc:
			print('Error: TrabantSim process could not be started.')
	return proc != None

def _htmlcol(col):
	if col:
		if type(col) == str:
			assert col[0]=='#'
			if len(col) in (4,5):	# 5 = with alpha.
				col = [int(c,16)/16 for c in col[1:]]
			elif len(col) in (7,9):	# 9 = with alpha.
				col = [int(col[i:i+2],16)/256 for i in range(1,len(col),2)]
		else:
			col = [float(c) for c in col]
		if len(col) < 4:	# No alpha included?
			col += [1.0]	# Opaque by default.
	return col
