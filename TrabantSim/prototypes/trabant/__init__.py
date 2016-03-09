#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from math import radians,tan
from trabant.math import *
import sys
import trabant.asc2obj
import trabant.gameapi
import trabant.objgen
import time


class _flushfile:
	'''Use unbuffered standard output, for debugging purposes.'''
	def __init__(self, f):
		self.f = f
	def write(self, x):
		self.f.write(x)
		self.flush()
	def flush(self):
		self.f.flush()
sys.stdout = _flushfile(sys.stdout)

roll_turn_engine,roll_engine,walk_abs_engine,push_abs_engine,push_rel_engine,push_turn_abs_engine,push_turn_rel_engine,gyro_engine,rotor_engine,tilt_engine,slider_engine = 'roll_turn roll walk_abs push_abs push_rel push_turn_abs push_turn_rel gyro rotor tilt slider'.split()
hinge_joint,suspend_hinge_joint,turn_hinge_joint,slider_joint,fixed_joint = 'hinge suspend_hinge turn_hinge slider fixed'.split()
sound_clank,sound_bang,sound_engine_hizz,sound_engine_wobble,sound_engine_combustion,sound_engine_rotor = 'clank bang hizz wobble combustion rotor'.split()

osname = sys.platform
_lastlooptime = time.time()
_accurate_ascii_generate = False
_has_opened = False
_last_ascii_top_left_offset = None
_last_created_object = None
_prev_gfx = None
_prev_phys = []
_prev_col = None
_asc2obj_lookup = []
_aspect_ratio = 1.33333
_keys = None
_taps = None
_want_mousemove = False
_mousemove = vec3()
_invalidated_taps = set()
_collisions = None
_joysticks = {}
_accelerometer_calibration = vec3()
_timers = {}
_timer_callbacks = {}
_objects = {}
_async_load = False
_async_loaders = {}
_cam_angle,_cam_distance,_cam_target,_cam_lookat,_cam_fov_radians,_cam_relative,_cam_is_smooth = vec3(),10,None,vec3(),0.5,False,False
_cam_pos,_cam_q,_cam_inv_q = vec3(0,-10,0),quat(),quat()


class Engine:
	'''An engine adds force and/or torque to an Obj (a game object). Use engines for simulation-style games.'''
	def __init__(self, oid, eid, etype):
		self.oid,self.eid,self.etype = oid,eid,etype
	def force(self,f):
		'''Force parameter can either be a number or a 3-tuple controlling force in X, Y and Z; both are ok.
		   For example, a rolling wheel engine typically uses a number (throttle), while a push engine
		   ("spaceship") is usually controlled in 3D.'''
		try:
			gameapi.set_engine_force(self.oid, self.eid, iter(f))
		except TypeError:
			gameapi.set_engine_force(self.oid, self.eid, (f,0,0))
	def addsound(self, sound, intensity=1, volume=20):
		'''Intensity controls pitch, volume controls audible distance.'''
		if self.etype == gyro_engine:
			gameapi.addtag(self.oid, 'engine_sound', [0, 0.001*intensity,0.8*intensity,1.4, 0,volume,1, 0,1,1, 1], [sound+'.wav'], [0], [self.eid], [])
		elif self.etype in (push_abs_engine,push_rel_engine,push_turn_abs_engine,push_turn_rel_engine):
			gameapi.addtag(self.oid, 'engine_sound', [0.5, 1*intensity,5*intensity,1, 0.05*volume,volume,1, 0,0.5,1, 1], [sound+'.wav'], [0], [self.eid], [])
		else:
			gameapi.addtag(self.oid, 'engine_sound', [1, 1*intensity,5*intensity,1, 0.05*volume,volume,1, 0,0.5,1, 1], [sound+'.wav'], [0], [self.eid], [])

class Obj:
	def __init__(self, id, gfx, phys):
		self.id = id
		self.engine = []
		self.last_joint_axis = None
		self.gfx = gfx
		self.phys = phys
		self.isloaded = False
	def pos(self, pos=None, orientation=None):
		'''Orientation is a quaternion.'''
		if pos:
			gameapi.pos(self.id, pos)
		if orientation:
			gameapi.orientation(self.id, orientation)
		if not pos and not orientation:
			return gameapi.pos(self.id, None)
	def orientation(self, orientation=None):
		'''Orientation is a quaternion.'''
		return gameapi.orientation(self.id, orientation)
	def vel(self, vel=None, avel=None):
		'''avel means angular velocity, i.e. rotation speed about each axis.'''
		if vel:
			gameapi.vel(self.id, vel)
		if avel:
			gameapi.avel(self.id, avel)
		if not vel and not avel:
			return gameapi.vel(self.id, None)
	def avel(self, avel=None):
		'''Angular velocity.'''
		return gameapi.avel(self.id, avel)
	def mass(self, w):
		'''Setting mass is only useful for interaction between dynamic objects.'''
		return gameapi.mass(self.id, (w,))
	def col(self, col=None):
		'''Set color, input is either a 3-tuple (R,G,B) or an html string color such as #ff3 or #304099.'''
		return gameapi.col(self.id, col)
	def bounce_in_rect(self,ltn,rbf,spring=1):
		'''Change velocity if position goes outside box defined by left-top-near corner (ltn)
		   and right-bottom-far corner (rbf).'''
		p,v = self.pos(),self.vel()
		_v = vec3(v)
		ltn,rbf = tovec3(ltn),tovec3(rbf)
		if p.x < ltn.x:
			p.x = ltn.x
			v.x = +abs(v.x)*spring
		if p.x > rbf.x:
			p.x = rbf.x
			v.x = -abs(v.x)*spring
		if p.y < ltn.y:
			p.y = ltn.y
			v.y = +abs(v.y)*spring
		if p.y > rbf.y:
			p.y = rbf.y
			v.y = -abs(v.y)*spring
		if p.z < ltn.z:
			p.z = ltn.z
			v.z = +abs(v.z)*spring
		if p.z > rbf.z:
			p.z = rbf.z
			v.z = -abs(v.z)*spring
		if v != _v:
			self.pos(p)
			self.vel(v)
	def create_engine(self, engine_type, max_velocity=None, offset=None, strength=1, friction=0, targets=None, sound=None):
		'''Offset is only used in a few engines (such as rotor tilt). Friction is used for engine brake,
		   friction=1 means "try to stop at once" in push and roll engines. The targets parameter is used
		   when adding multiple controlled objects from a single engine, for example a roll engine
		   controlling two hinge joints on the rear wheels in a rear-wheel drive car. Each targets tuple
		   contains a controlled object and a factor.'''
		topmounted_gyro = self.last_joint_axis and self.last_joint_axis.normalize()*vec3(0,0,1) > 0.8
		max_velocity, strength, friction = _normalize_engine_values(engine_type, max_velocity, offset, strength, friction, topmounted_gyro)
		target_efcts = [(t.id,efct) for t,efct in targets] if targets else []
		eid = gameapi.create_engine(self.id, engine_type, max_velocity, strength, friction, target_efcts)
		self.engine += [Engine(self.id, eid, engine_type)]
		if sound:
			self.engine[-1].addsound(sound, 1)
		return self.engine[-1]
	def joint(self, joint_type, obj2, axis=None, stop=None, spring=None):
		'''Create a joint between two objects. The stop parameter contains low and high stops, in hinge-type
		   joints this is the low and high angle (in radians).'''
		self.last_joint_axis = tovec3(axis)
		return gameapi.create_joint(self.id, joint_type, obj2.id, axis, stop, spring)
	def add_stabilizer(self, force=0.5):
		'''Adds a stabilizer to the object, this is useful if you're building a helicopter or similar.'''
		gameapi.addtag(self.id, 'upright_stabilizer', [force], [], [0], [], [])
	def release(self):
		'''Remove the object from the game.'''
		if self.id in _async_loaders:
			del _async_loaders[self.id]
		gameapi.releaseobj(self.id)
		global _objects,_last_created_object
		del _objects[self.id]
		if _last_created_object == self:
			_last_created_object = None
		self.id = None
	def released(self):
		'''Check if an object is still present in the game.'''
		return self.id == None


class Tap:
	'''Represents a tap/click/swipe/drag. 2D positions in range [0,1], but you can get ahold of 3D
	   coordinates by functions in this class.'''
	def __init__(self, x, y, startx, starty, vx, vy, ispress, buttonmask):
		self.x,self.y = x,y
		self.startx,self.starty = startx,starty
		self.vx,self.vy = vx,vy
		self.ispress = ispress
		self.buttonmask = buttonmask
	@property
	def isrelease(self):
		return not self.ispress
	def pos3d(self, z=None):
		'''Converts the tap on-screen 2D position to a world 3D position. z=None returns a position
		   on the camera distance.'''
		if not z:
			z = _cam_distance
		return _screen2world(self.x,self.y, z)
	def vel3d(self, z=None):
		'''Converts the swipe/drag on-screen 2D velocity to a world 3D velocity. z=None returns a
		   velocity relative to the camera distance.'''
		if not z:
			z = _cam_distance
		return _relscreen2world(self.vx,self.vy, z)
	def invalidate(self):
		'''This tap won't be returned again from the taps() function.'''
		global _invalidated_taps
		_invalidated_taps.add((self.startx,self.starty))
	def movement2(self):
		'''Returns the magnitude between the tap and it's starting point.'''
		return (self.x-self.startx)**2+(self.y-self.starty)**2
	def _distance2(self, x, y):
		return (self.x-x)**2+(self.y-y)**2
	def __hash__(self):
		return self.starty*35797+self.startx	# Prime larger than screen width is ok.


class Joystick:
	def __init__(self, id, sloppy):
		'''X,Y in [-1,1].'''
		self.id = id
		self.sloppy = sloppy
		self.x,self.y = 0,0


def trabant_init(**kwargs):
	'''Usually called implicitly by some other function in the API.'''
	interactive = bool(hasattr(sys, 'ps1') or sys.flags.interactive)
	config = {'restart': not interactive}
	try:
		import os.path
		configfile = os.path.join(os.path.expanduser("~"), '.trabant')
		for line in open(configfile, 'rt'):
			words = [w.strip() for w in line.split('=')]
			if len(words) == 2 and words[0] and words[0][0]!='#':
				config[words[0]] = words[1]
	except:
		pass
	for arg in sys.argv[1:]:
		try:
			k,v = arg.split('=')
			config[k] = v
		except:
			pass
	config.update(kwargs)
	global _keys,_joysticks,_timers,_timer_callbacks,_async_loaders,_has_opened,_accelerometer_calibration
	_keys,_joysticks,_timers,_timer_callbacks,_async_loaders,_has_opened = None,{},{},{},{},True
	if 'osname' in config:
		global osname
		osname = config['osname']
		del config['osname']
	gameapi.init(**config)
	gameapi.setvar('Game.AllowPowerDown', not interactive)
	if osname not in ['ios']:
		gameapi.sock.settimeout(None if interactive else 5)
		gameapi.sock.s.setblocking(interactive)
	cam(angle=(0,0,0), distance=10, target=None, fov=45, light_angle=(-0.8,0,0.1))
	loop(delay=0)	# Resets taps+collisions.
	_accelerometer_calibration = accelerometer()
	async_load(False)

def debugsim(enable=True):
	'''Turns on/off simulation debug mode, which by default renders physics shapes.'''
	gameapi.debugsim(enable)

def async_load(enable=True):
    global _async_load
    _async_load = enable

def userinfo(message=''):
	'''Shows a message dialog to the user. Dismiss dialog by calling without parameters.'''
	_tryinit()
	gameapi.userinfo(message)

def loop(delay=0.03, end_after=None):
	'''Call this every loop, check return value if you should continue looping.'''
	_tryinit()
	global _lastlooptime,_timer_callbacks,_async_loaders
	looptime = time.time()-_lastlooptime
	_lastlooptime += looptime
	sleep(max(0,delay-looptime))
	global _keys,_taps,_mousemove,_collisions,_cam_pos
	_keys,_taps,_collisions,_cam_pos = None,None,None,None
	_poll_joysticks()
	for timer,t_func in list(_timer_callbacks.items()):
		t,func = t_func
		if timeout(t, timer):
			del _timer_callbacks[timer]
			timeout(timer=timer, reset=True)
			func()
	if _async_loaders:
		loaded = gameapi.areloaded(_async_loaders.keys())
		for loaded,oid_ldr in zip(loaded,list(_async_loaders.items())):
			if loaded:
				oid,loader = oid_ldr
				loader()
				del _async_loaders[oid]
	if _want_mousemove:
		_mousemove = tovec3([float(a) for a in gameapi.mousemove().split()])
	if end_after and timeout(end_after,timer=-154):
		timeout(timer=-154, reset=True)	# Remove timer.
		return False
	return gameapi.opened()

def sleep(t):
	'''Wraps time.sleep so you won't have to import it.'''
	time.sleep(min(0.5,t))

def timeout(t=1, timer=0, first_hit=False, reset=False):
	'''Will check if t seconds elapsed since first called. If first_hit is true, it will elapse
	   immediately on first call. You can run several simultaneous timers, use the timer parameter
	   to select which one. Uses the same timers as timein().'''
	global _timers
	if not timer in _timers:
		_timers[timer] = time.time()
		if first_hit:
			return True
	if reset:
		del _timers[timer]
		return False
	if time.time() - _timers[timer] > t:
		_timers[timer] = time.time()
		return True
	return False

def timein(t, timer=0):
	'''Checks if a less than t seconds elapsed since last called. Uses the same times as timeout().'''
	global _timers
	if not timer in _timers:
		_timers[timer] = time.time()
	if time.time() - _timers[timer] < t:
		return True
	return False

def timer_callback(t, func):
	global _timer_callbacks
	for tr in range(-2000,-1000):
		if tr not in _timers:
			timeout(t, timer=tr)
			_timer_callbacks[tr] = (t,func)
			break

def cam(angle=None, distance=None, target=None, pos=None, fov=None, target_relative_angle=None, light_angle=None, smooth=None):
	'''Set camera angle, distance, target object, position, fov. target_relative_angle=True means that the
	   angle is relative to your target object rather than absolute. light_angle is used to change the
	   direction of the directional light in the scene.'''
	_tryinit()
	if not (angle or distance or target or pos or fov or target_relative_angle or light_angle or smooth):
		_update_cam_shadow()
		return _cam_pos
	angle = tovec3(angle)
	gameapi.cam(angle, distance, target.id if target else target, tovec3(pos), fov, target_relative_angle, smooth)
	gameapi.light(tovec3(light_angle))
	# Update shadow variables for screen<-->world space transformations.
	global _cam_angle,_cam_distance,_cam_target,_cam_lookat,_cam_fov_radians,_cam_relative,_cam_is_smooth
	if angle:		 _cam_angle = angle
	if distance != None:	_cam_distance = distance
	if target:		_cam_target = target
	if pos:			_cam_lookat = tovec3(pos)
	if fov:			_cam_fov_radians = radians(fov)
	if target_relative_angle != None: _cam_relative = target_relative_angle
	if smooth != None:	_cam_is_smooth = not not smooth

def bg(col):
	'''Set background color either as (0.3,1,0.5), '#ffa', or '#F6AA03'.'''
	_tryinit()
	gameapi.setbgcolor(col)

def fg(col=None, outline=None):
	'''Set default foreground color either as (0.3,1,0.5), '#ffa', or '#F6AA03'. outline=False means the whole
	   shape will be filled with color. Default is outline=True.'''
	_tryinit()
	if col:
		global _prev_col
		if type(_prev_col)!=type(col) or _prev_col!=col:
			_prev_col = col
			gameapi.setpencolor(col)
	if outline != None:
		gameapi.setoutline(outline)

def fog(near,far):
	'''Sets fog near and far plane in physical range, e.g. fog(130,750).'''
	_tryinit()
	gameapi.fog(near,far)

def gravity(g, bounce=None, friction=None):
	'''Sets physical gravity force. bounce factor [0,1] is used to control how much new objects created
	   will bounce, 0=low bounce and 1=preserve energy. friction factor [0,1] controls how much friction
	   new objects created will have.'''
	_tryinit()
	gameapi.gravity(tovec3(g))
	if bounce != None:
		gameapi.bounce(bounce)
	if friction != None:
		gameapi.friction(friction)

def create_ascii_object(ascii, pos=None, orientation=None, vel=None, avel=None, mass=None, col=None, mat='flat', static=False, trigger=False, physmesh=False, process=None):
	'''Returns an Obj. static=True means object if fixed in absolute space. trigger=True means it won't apply any
	   force during collisions. Only four types of materials exist: flat, smooth, checker and noise. orientation
	   is a quaternion, avel is angular velocity. You can pre-process the physics and graphics with the process
	   callback function before it's added to the simulation.'''
	global _last_ascii_top_left_offset,_asc2obj_lookup
	physmesh = True if physmesh==True else False
	# Keep a small cache of generated objects. Most small prototypes will reuse shapes.
	gfx = None
	for s,g,p,lo in _asc2obj_lookup:
		if s == ascii+str(physmesh)+str(process):
			gfx,phys,_last_ascii_top_left_offset = g,p,lo
	if not gfx:
		gfx,phys = asc2obj.str2obj(ascii, fast=not _accurate_ascii_generate, force_phys_mesh=physmesh)
		_last_ascii_top_left_offset = -vec3(min(gfx.vertices, key=lambda v:v.x).x, min(gfx.vertices, key=lambda v:v.z).z, min(gfx.vertices, key=lambda v:v.y).y)
		orientation = toquat(orientation) if orientation else quat()
		if process:
			orientation,gfx,phys = process(orientation,gfx,phys)
		_asc2obj_lookup.append((ascii+str(physmesh)+str(process),gfx,phys,_last_ascii_top_left_offset))
		if len(_asc2obj_lookup) > 10:
			del _asc2obj_lookup[0]
	return _create_object(gfx, phys, static, trigger, pos=pos, orientation=orientation, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_mesh(vertices, triangles, pos=None, orientation=None, vel=None, avel=None, mass=None, col=None, mat='smooth', static=False, trigger=False, process=None):
	'''Returns an Obj. static=True means object if fixed in absolute space. trigger=True means it won't apply any
	   force during collisions. Only four types of materials exist: flat, smooth, checker and noise. orientation
	   is a quaternion, avel is angular velocity. You can pre-process the physics and graphics with the process
	   callback function before it's added to the simulation.'''
	orientation = toquat(orientation) if orientation else quat()
	gfx,phys = objgen.createmesh(vertices,triangles)
	if process:
		orientation,gfx,phys = process(orientation,gfx,phys)
	return _create_object(gfx, phys, static, trigger, pos=pos, orientation=orientation, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_box(pos=None, orientation=None, side=1, vel=None, avel=None, mass=None, mat='checker', col=None, static=False, trigger=False, process=None):
	'''Returns an Obj. static=True means object if fixed in absolute space. trigger=True means it won't apply any
	   force during collisions. A box can have different length sides (cuboid), you create one by supplying a
	   three-tuple instead of a scalar. Only four types of materials exist: flat, smooth, checker and noise.
	   orientation is a quaternion, avel is angular velocity. You can pre-process the physics and graphics with
	   the process callback function before it's added to the simulation.'''
	try:	side = tovec3(side)
	except:	side = vec3(side,side,side)
	orientation = toquat(orientation) if orientation else quat()
	gfx,phys = objgen.createcube(side)
	if process:
		orientation,gfx,phys = process(orientation,gfx,phys)
	return _create_object(gfx, phys, static, trigger, pos=pos, orientation=orientation, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_sphere(pos=None, radius=1, vel=None, avel=None, mass=None, col=None, mat='smooth', static=False, trigger=False, process=None):
	'''Returns an Obj. static=True means object if fixed in absolute space. trigger=True means it won't apply any
	   force during collisions. Only four types of materials exist: flat, smooth, checker and noise. orientation
	   is a quaternion, avel is angular velocity. You can pre-process the physics and graphics with the process
	   callback function before it's added to the simulation.'''
	orientation,resolution = quat(),int(min(8, max(4,radius**0.3)*8))
	gfx,phys = objgen.createsphere(radius, latitude=resolution, longitude=int(resolution*1.5))
	if process:
		orientation,gfx,phys = process(orientation,gfx,phys)
	return _create_object(gfx, phys, static, trigger, pos=pos, orientation=orientation, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_capsule(pos=None, orientation=None, radius=0.5, length=1, vel=None, avel=None, mass=None, col=None, mat='smooth', static=False, trigger=False, process=None):
	'''Returns an Obj. static=True means object if fixed in absolute space. trigger=True means it won't apply any
	   force during collisions. Only four types of materials exist: flat, smooth, checker and noise. orientation
	   is a quaternion, avel is angular velocity. You can pre-process the physics and graphics with the process
	   callback function before it's added to the simulation.'''
	resolution = int(min(8, max(4,radius**0.3)*8))
	gfx,phys = objgen.createcapsule(radius, length, latitude=resolution, longitude=int(resolution*1.5))
	if process:
		orientation,gfx,phys = process(orientation,gfx,phys)
	return _create_object(gfx, phys, static, trigger, pos=pos, orientation=orientation, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_clones(obj, placements, mat=None, static=False):
	'''Creates multiple clones at once of the original Obj, returns a list of Objs. Placement is a list of
	   tuples, each tuple contains position (vec3) and orientation (quat) IN THAT ORDER. This function is
	   much faster when creating many objects.'''
	global _objects
	mat = mat if mat else _last_mat
	objs = []
	str_placements = ''
	def append(splacements):
		global _last_created_object
		for oid in gameapi.cloneobjs(obj.id, static, mat, splacements):
			o = Obj(oid,None,None)
			_objects[oid] = o
			objs.append(o)
			_last_created_object = o
	for pos,q in placements:
		pos,q = tovec3(pos),toquat(q)
		str_placements += ',' if str_placements else ''
		str_placements += '%g,%g,%g,%g,%g,%g,%g' % (pos.x,pos.y,pos.z,q.q[0],q.q[1],q.q[2],q.q[3])
		if len(str_placements) >= 1440:
			append(str_placements)
			str_placements = ''
	if str_placements:
		append(str_placements)
	return objs

def last_created_object():
	'''Returns the last created object. Pretty much only useful if experimenting in an interactive shell.'''
	return _last_created_object

def accurate_ascii_generate(enable):
	'''Turn this on to remove unnecessary graphics triangles when generating ascii objects. It's sloooww.'''
	global _accurate_ascii_generate
	_accurate_ascii_generate = enable

def pick_objects(pos, direction, near=2, far=1000):
	'''Ray-pick an list of Objs.'''
	pos = tovec3(pos)
	objects = [(_objects[oid],pos) for oid,pos in gameapi.pickobjs(pos, direction, near, far) if oid in _objects]
	return sorted(objects, key=lambda op: (op[1]-pos).length2())

def release_all_objects():
	'''Clean slate, start over fresh.'''
	gameapi.release_all_objects()
	global _objects,_last_created_object
	for o in _objects.values():
		o.id = None
	_objects,_last_created_object = {},None

def last_ascii_top_left_offset():
	'''Returns the distance from the center of the last create ASCII object to the top-left-front corner of
	   its AABB. See the tetris example for how it's used.'''
	return _last_ascii_top_left_offset

def explode(pos, vel=vec3(), strength=1, volume=1):
	'''Show a graphical explosion and play an explosion sound.'''
	_tryinit()
	gameapi.explode(tovec3(pos),tovec3(vel),strength,volume)

def sound(snd, pos=vec3(), vel=vec3(), volume=5):
	'''Play a sound using the given position, velocity and volume. Volume controls audible distance.'''
	_tryinit()
	gameapi.playsound(snd+'.wav', tovec3(pos), tovec3(vel), volume)

def collided_objects():
	'''Returns all objects that collided last loop.'''
	return set(o for o,_,_,_ in collisions())

def collisions(enable=None):
	'''Returns all collisions that occurred last loop. Each collision is a 4-tuple:
	   (object1,object2,force,position).'''
	if enable in (True, False):
		gameapi.setvar('Physics.NoClip', not enable)
		return
	global _collisions
	if _collisions != None:
		return _collisions
	_collisions = []
	for line in gameapi.pop_collisions().split('\n'):
		if not line:
			continue
		words = line.split()
		oid,other_oid = int(words[0]),int(words[7])
		if oid in _objects and other_oid in _objects:
			force,pos = tovec3([float(f) for f in words[1:4]]),tovec3([float(f) for f in words[4:7]])
			_collisions.append((_objects[oid],_objects[other_oid],force,pos))
			_collisions.append((_objects[other_oid],_objects[oid],-force,pos))
	return _collisions

def keys():
	'''Returns a list of all keys pressed this loop.'''
	global _keys
	if _keys == None:
		_keys = [key for key in gameapi.keys().split('\n') if key]
	return _keys

def keydir():
	'''Returns the keyboard input direction as a 3D vector. X and Y is controlled by arrow keys
	   up/down/left/right and by WSAD. +Z is controlled by SPACE, -Z is controlled by SHIFT.'''
	directions = vec3()
	directions += vec3(0,+1,0) if   'UP'  in keys() or 'W' in keys() else vec3()
	directions += vec3(0,-1,0) if  'DOWN' in keys() or 'S' in keys() else vec3()
	directions += vec3(-1,0,0) if  'LEFT' in keys() or 'A' in keys() else vec3()
	directions += vec3(+1,0,0) if 'RIGHT' in keys() or 'D' in keys() else vec3()
	directions += vec3(0,0,+1) if 'SPACE' in keys() else vec3()
	directions += vec3(0,0,-1) if [1 for k in keys() if k in ('SHIFT','LSHIFT','RSHIFT')] else vec3()
	return directions

def taps():
	'''Returns all taps since last loop. Each tap is an object of class Tap.'''
	global _taps
	if _taps != None:
		return _taps
	def processline(line):
		ws = line.split()
		return [float(w) for w in ws[:6]] + [ws[6]=='true',int(ws[7])]
	taps_info = [processline(line) for line in gameapi.taps().split('\n') if line]
	_taps = []
	used_invalidations = set()
	for x,y,startx,starty,vx,vy,ispress,buttonmask in taps_info:
		if (startx,starty) not in _invalidated_taps:
			_taps.append(Tap(x,y,startx,starty,vx,vy,ispress,buttonmask))
		else:
			used_invalidations.add((startx,starty))
	for tapstart in set(_invalidated_taps):
		if not tapstart in used_invalidations:
			_invalidated_taps.remove(tapstart)
	return _taps

def closest_tap(pos3):
	'''The parameter pos3 is a 3D coordinate. Returns an instance of the Tap class or None.'''
	if not taps():
		return None
	pos3 = tovec3(pos3)
	x,y = _world2screen(pos3)
	tap = min(taps(), key=lambda t: t._distance2(x,y))
	tap.close_pos = pos3
	return tap

clicks = taps
closest_click = closest_tap

def click(left=False, right=False, middle=False):
	'''Returns clicks (instances of Tap) matching left/right/middle.
	   E.g. click(right=True) returns a tap when you right-click.'''
	mask = (1 if left else 0) | (2 if right else 0) | (4 if middle else 0)
	mask = mask if mask else 0xFF	# Default to any click.
	rcs = [c for c in clicks() if c.buttonmask&mask]
	return rcs[0] if rcs else None

def create_joystick(xy, sloppy=False):
	'''Creates an on-screen tap soft joystick. sloppy=True keeps the stick in place, as if it had no spring.'''
	_tryinit()
	jid = gameapi.create_joystick(*xy, sloppy=sloppy)
	global _joysticks
	j = Joystick(jid, sloppy)
	_joysticks[j.id] = j
	return j

def accelerometer(relative=False):
	'''Returns the accelerometer values. relative=True will return a value relative to the way it was when
	   the prototype was started. More importantly, relative=True will also include pitch and roll values in
	   the return value, useful for race/flight simulation.'''
	_tryinit()
	acc = tovec3([float(a) for a in gameapi.accelerometer().split()])
	if not relative:
		return acc
	rel = acc - _accelerometer_calibration
	if rel.length() < 0.1:
		rel.pitch = rel.roll = 0.0
		return rel
	rel.pitch = -_accelerometer_calibration.angle_x(acc)
	if abs(_accelerometer_calibration.z) > abs(_accelerometer_calibration.y):
		rel.roll = -_accelerometer_calibration.angle_y(acc)
	else:
		rel.roll = _accelerometer_calibration.angle_z(acc)
	return rel

def mousemove():
	'''Useful for mouselook computer controls (quake, minecraft). Hides the cursor.'''
	global _want_mousemove
	_want_mousemove = True
	return _mousemove

def mousewheel():
	'''Useful for mouselook computer controls (quake, minecraft). Hides the cursor.'''
	global _want_mousemove
	_want_mousemove = True
	return _mousemove.z

def is_touch_device():
	'''Only use this function if your touch device's and computer's controls clash.'''
	return gameapi.osname.lower() in ('ios', 'android')

def onload(obj, func):
	'''Perform function when object has been loaded.'''
	global _async_loaders
	oldfunc = _async_loaders[obj.id]
	def doit():
		oldfunc()
		func()
	_async_loaders[obj.id] = doit


########################################


def _poll_joysticks():
	if not _joysticks:
		return
	jdata,used_joys = [],set()
	for line in [l for l in gameapi.joystick_data().split('\n') if l]:
		w = line.split()
		jdata.append((int(w[0]),float(w[1]),float(w[2])))
	for jid,x,y in jdata:
		used_joys.add(jid)
		j = _joysticks[jid]
		j.x,j.y = x,y
	if timeout(0.05, timer=-155):	# Ensure an engine frame has passed.
		for j in [joy for jid,joy in _joysticks.items() if jid not in used_joys and not joy.sloppy]:
			j.x = j.y = 0.0

def _create_object(gfx, phys, static, trigger, pos, orientation, vel, avel, mass, col, mat):
	_tryinit()
	global _prev_gfx,_prev_phys,_last_mat
	_last_mat = mat
	base_eq = (_prev_gfx == gfx and len(_prev_phys) == len(phys))
	if not base_eq or any(pp!=p for pp,p in zip(_prev_phys, phys)):
		gameapi.initgfxmesh(gfx.q, gfx.pos, gfx.vertices, gfx.indices)
		gameapi.clearprepphys()
		for p in phys:
			if 'Box' in str(type(p)):
				gameapi.initphysbox(p.q, p.pos, p.size)
			elif 'Sphere' in str(type(p)):
				gameapi.initphyssphere(p.q, p.pos, p.radius)
			elif 'Capsule' in str(type(p)):
				gameapi.initphyscapsule(p.q, p.pos, p.radius, p.length)
			elif 'Mesh' in str(type(p)):
				gameapi.initphysmesh(p.q, p.pos, p.vertices, p.indices)
	_prev_gfx,_prev_phys = gfx,phys
	fg(col)
	objpos = tovec3(pos) if tovec3(pos) else vec3()
	objori = toquat(orientation) if toquat(orientation) else quat()
	oid = gameapi.createobj(static, trigger, mat, objpos, objori)
	if not _async_load:
		gameapi.waitload(oid)
	o = Obj(oid,gfx,phys)
	global _objects,_last_created_object,_async_loaders
	_objects[oid] = o
	def postload(o):
		if not o.id:
			return
		if vel:
			o.vel(tovec3(vel))
		if avel:
			o.avel(tovec3(avel))
		if mass:
			o.mass(mass)
		o.isloaded = True
	if _async_load:
		_async_loaders[oid] = lambda: postload(o)
	else:
		postload(o)
	_last_created_object = o
	return o

def _normalize_engine_values(engine_type, max_velocity, offset, strength, friction, topmounted_gyro):
	# Convert single number to list.
	try:	max_velocity = (float(max_velocity), 0)
	except:	max_velocity = (0,0) if not max_velocity else max_velocity
	try:	offset = (float(offset), 0)
	except:	offset = (0,0) if not offset else offset

	# Offset and max_velocity are used for different purposes for different engines.
	max_velocity = [mv+ov for mv,ov in zip(max_velocity,offset)]

	# Make engine-specific adjustions.
	if engine_type == roll_turn_engine:
		max_velocity[0] = 2 if not max_velocity[0] else max_velocity[0]
		max_velocity[1] = 2 if not max_velocity[1] else max_velocity[1]
	elif engine_type == roll_engine:
		max_velocity[0] = 100 if not max_velocity[0] else max_velocity[0]
		max_velocity[1] = -20 if not max_velocity[1] else max_velocity[1]
		strength *= 10
	elif engine_type in (walk_abs_engine, push_abs_engine, push_rel_engine):
		max_velocity[0] = 20 if not max_velocity[0] else max_velocity[0]
		if engine_type == walk_abs_engine:
			friction = friction if friction else 1
	elif engine_type in (push_turn_abs_engine, push_turn_rel_engine):
		max_velocity[0] = 0.03 if not max_velocity[0] else max_velocity[0]
	elif engine_type == gyro_engine:
		max_velocity[0] = 50 if not max_velocity[0] else max_velocity[0]
		max_velocity[1] = 20 if not max_velocity[1] else max_velocity[1]
		strength *= 3
		friction = 0.05 if not friction else friction
	elif engine_type == rotor_engine:
		strength /= 15
		if topmounted_gyro:
			if not max_velocity[0] and not max_velocity[1]:
				max_velocity[0] = 2	# Rotor Z lift offset.
				max_velocity[1] = 2	# This means rotor VTOL stabilization factor.
			friction = 0.001 if not friction else friction
		else:
			friction = 0.1 if not friction else friction
	elif engine_type == tilt_engine:
		strength /= 1500
	elif engine_type == slider_engine:
		strength *= 5
		max_velocity[0] = 3 if not max_velocity[0] else max_velocity[0]
		max_velocity[1] = 3 if not max_velocity[1] else max_velocity[1]
	return max_velocity, strength, friction

def _world2screen(crd):
	'''Return screen X,Y in [0,1].'''
	_update_cam_shadow()
	#c = crd
	crd = _cam_inv_q*(crd-_cam_pos)
	iar = 1/_get_aspect_ratio()
	itana = 1/tan(_cam_fov_radians*0.5);
	x,y = crd.x/crd.y,crd.z/crd.y
	x,y = x*itana*iar+1, -y*itana+1
	return x,y

def _screen2world(x,y,distance):
	'''Return world X,Z scaled to Y=1.'''
	_update_cam_shadow()
	tana = tan(_cam_fov_radians*0.5);
	dx = tana * (x*2-1) * _get_aspect_ratio()
	dy = tana * (1-y*2)
	crd = (_cam_q * vec3(dx, 1, dy) * distance) + _cam_pos
	return crd

def _relscreen2world(x,y,distance):
	'''From relative screen coordinates, return world X,Z scaled to Y=1.'''
	_update_cam_shadow()
	tana = tan(_cam_fov_radians*0.5);
	dx = tana * 2*x * _get_aspect_ratio()
	dy = tana * -2*y
	crd = (_cam_q * vec3(dx, 1, dy) * distance) + _cam_pos
	return crd

def _update_cam_shadow():
	global _cam_pos,_cam_q,_cam_inv_q,_cam_target
	if _cam_pos:
		return
	if _cam_target:
		o = None
		_cam_pos = _cam_target.pos()
		if not _cam_pos:
			_cam_target = None
			return _update_cam_shadow()
		elif _cam_lookat:
			o = _cam_target.orientation()
			_cam_pos += o*_cam_lookat
	elif _cam_lookat:
		_cam_pos = _cam_lookat
	else:
		_cam_pos = vec3()
	_cam_q = quat().rotate_z(_cam_angle.z).rotate_x(_cam_angle.x).rotate_y(_cam_angle.y)
	#if _cam_target:
	#	_cam_q = (o if o else _cam_target.orientation()) * _cam_q
	_cam_inv_q = _cam_q.inverse()
	if _cam_distance:
		_cam_pos += _cam_q * vec3(0,-_cam_distance,0)

def _get_aspect_ratio():
	if timeout(3,timer=-152,first_hit=True):
		global _aspect_ratio
		_aspect_ratio = gameapi.get_aspect_ratio()
	return _aspect_ratio

def _tryinit():
	if not _has_opened:
		trabant_init()
