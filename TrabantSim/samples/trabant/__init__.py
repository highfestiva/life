#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from math import radians,tan
from trabant.math import *
import trabant.asc2obj
import trabant.gameapi
import trabant.objgen
import time


roll_turn_engine,roll_engine,push_abs_engine,push_rel_engine,push_turn_abs_engine,push_turn_rel_engine,gyro_engine,rotor_engine,tilt_engine = 'roll_turn roll push_abs push_rel push_turn_abs push_turn_rel gyro rotor tilt'.split()
hinge_joint,suspend_hinge_joint,turn_hinge_joint = 'hinge suspend_hinge turn_hinge'.split()
sound_clank,sound_bang,sound_engine_hizz,sound_engine_wobble,sound_engine_combustion,sound_engine_rotor = 'clank bang hizz wobble combustion rotor'.split()

_wait_until_loaded = True
_fast_ascii_generate = True
_has_opened = False
_last_ascii_top_left_offset = None
asc2obj_lookup = []
_aspect_ratio = 1.33333
_keys = None
_taps = None
_invalidated_taps = set()
_collisions = None
_joysticks = {}
_accelerometer_calibration = vec3()
_timers = {}
_objects = {}
_cam_angle,_cam_distance,_cam_target,_cam_lookat,_cam_fov_radians,_cam_relative = vec3(),10,None,vec3(),0.5,False
_cam_pos,_cam_q,_cam_inv_q = vec3(0,-10,0),quat(),quat()


class Engine:
	def __init__(self, oid, eid):
		self.oid,self.eid = oid,eid
	def force(self,f):
		'''Force parameter can either be a number or a 3-tuple controlling force in X, Y and Z.'''
		try:
			gameapi.set_engine_force(self.oid, self.eid, iter(f))
		except TypeError:
			gameapi.set_engine_force(self.oid, self.eid, (f,0,0))
	def addsound(self, sound, intensity=1, volume=20):
		'''Intensity controls pitch, volume controls audible distance.'''
		if sound == sound_engine_rotor:
			gameapi.addtag(self.oid, 'engine_sound', [0, 0.001*intensity,0.8*intensity,1.4, 0,volume,1, 0,1,1, 1], [sound+'.wav'], [0], [self.eid], [])
		else:
			gameapi.addtag(self.oid, 'engine_sound', [1, 1*intensity,5*intensity,1, 0.05*volume,volume,1, 0,0.5,1, 1], [sound+'.wav'], [0], [self.eid], [])

class Obj:
	def __init__(self, id):
		self.id = id
		self.engine = []
		self.last_joint_axis = None
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
		return gameapi.mass(self.id, (w,))
	def col(self, col=None):
		'''Set color, input is either a 3-tuple (R,G,B) or an html string color such as #ff3 or #304099.'''
		return gameapi.col(self.id, col)
	def bounce_in_rect(self,ltn,rbf):
		'''Change velocity if position goes outside box defined by left-top-near corner (ltn)
		   and right-bottom-far corner (rbf).'''
		p,v = self.pos(),self.vel()
		_v = vec3(v)
		ltn,rbf = tovec3(ltn),tovec3(rbf)
		if p.x < ltn.x: v.x = +abs(v.x)
		if p.x > rbf.x: v.x = -abs(v.x)
		if p.y < ltn.y: v.y = +abs(v.y)
		if p.y > rbf.y: v.y = -abs(v.y)
		if p.z < ltn.z: v.z = +abs(v.z)
		if p.z > rbf.z: v.z = -abs(v.z)
		if v != _v:
			self.vel(v)
	def create_engine(self, engine_type, max_velocity=None, offset=None, strength=1, friction=0, targets=None, sound=None):
		'''Offset is only used in a few engines (such as rotor tilt). Friction is used for engine brake, friction=1 means
		   "try to stop at once" in push and roll engines. The targets parameter is used when adding multiple controlled
		   objects from a single engine, for example a roll engine controlling two hinge joints on the rear wheels in a
		   rear-wheel drive car. Each targets tuple contains a controlled object and a factor.'''
		topmounted_gyro = self.last_joint_axis and self.last_joint_axis.normalize()*vec3(0,0,1) > 0.8
		max_velocity, strength, friction = _normalize_engine_values(engine_type, max_velocity, offset, strength, friction, topmounted_gyro)
		target_efcts = [(t.id,efct) for t,efct in targets] if targets else []
		eid = gameapi.create_engine(self.id, engine_type, max_velocity, strength, friction, target_efcts)
		self.engine += [Engine(self.id, eid)]
		if sound:
			self.engine[-1].addsound(sound, 1)
		return self.engine[-1]
	def create_joint(self, joint_type, obj2, axis=None, stop=None, spring=None):
		'''Create a joint between two objects. The stop parameter contains low and high stops, in hinge-type joints this
		   is the low and high angle (in radians).'''
		self.last_joint_axis = tovec3(axis)
		return gameapi.create_joint(self.id, joint_type, obj2.id, axis, stop, spring)
	def add_stabilizer(self, force=0.5):
		'''Adds a stabilizer to the object, this is useful if you're building a helicopter or similar.'''
		gameapi.addtag(self.id, 'upright_stabilizer', [force], [], [0], [], [])
	def release(self):
		gameapi.releaseobj(self.id)
		global _objects
		del _objects[self.id]
		self.id = None
	def released(self):
		return self.id == None


class Tap:
	def __init__(self, x, y, startx, starty, vx, vy):
		self.x,self.y = x,y
		self.startx,self.starty = startx,starty
		self.vx,self.vy = vx,vy
	def pos3d(self, z=None):
		'''Converts the tap on-screen 2D position to a world 3D position.'''
		if not z:
			z = _cam_distance
		return _screen2world(self.x,self.y, z)
	def vel3d(self, z=None):
		'''Converts the swipe/drag on-screen 2D velocity to a world 3D velocity.'''
		if not z:
			z = _cam_distance
		return _relscreen2world(self.vx,self.vy, z)
	def invalidate(self):
		'''This tap won't be returned again from the taps() function.'''
		global _invalidated_taps
		_invalidated_taps.add((self.startx,self.starty))
	def _distance2(self, x, y):
		return (self.x-x)**2+(self.y-y)**2
	def __hash__(self):
		return self.starty*35797+self.startx	# Prime larger than screen width is ok.


class Joystick:
	def __init__(self, id, sloppy):
		'''Screen X,Y in [0,1].'''
		self.id = id
		self.sloppy = sloppy
		self.x,self.y = 0,0


def trabant_init(**kwargs):
	config = {}
	try:
		import os.path
		configfile = os.path.join(os.path.expanduser("~"), '.trabant')
		for line in open(configfile, 'rt'):
			words = line.split()
			if len(words) == 3 and words[1] == '=':
				config[words[0]] = words[2]
	except FileNotFoundError:
		pass
	config.update(kwargs)
	global _joysticks,_timers,_has_opened,_accelerometer_calibration
	_joysticks,_timers,_has_opened,online = {},{},True,False
	exc = None
	for _ in range(2):
		try:
			if gameapi.init(**config):
				online = True
				break
		except Exception as e:
			exc = e
	if not online:
		raise exc if exc else Exception('unable to connect to simulator')
	cam(angle=(0,0,0), distance=10, target=None, fov=45, light_angle=(-0.8,0,0.1))
	loop(delay=0)	# Resets taps+collisions.
	_accelerometer_calibration = accelerometer()

def simdebug(enable=True):
	'''Turns on/off simulation debug mode, which by default renders physics shapes.'''
	gameapi.simdebug(enable)

def userinfo(message=''):
	'''Shows a message dialog to the user. Dismiss dialog by calling without parameters.'''
	gameapi.userinfo(message)

def loop(delay=0.1):
	'''Call this every loop, check return value if you should continue looping.'''
	_tryinit()
	sleep(delay)
	global _keys,_taps,_collisions,_cam_pos
	_keys,_taps,_collisions,_cam_pos = None,None,None,None
	_poll_joysticks()
	return gameapi.opened()

def sleep(t):
	time.sleep(t)

def timeout(t, timer=0, first_hit=False):
	'''Will check if time t elapsed since first called. If first_hit is true, it will elapse
	   immediately on first call. You can run several simultaneous timers, use the timer parameter
	   to select which one.'''
	global _timers
	if not timer in _timers:
		_timers[timer] = time.time()
		if first_hit:
			return True
	if time.time() - _timers[timer] > t:
		_timers[timer] = time.time()
		return True
	return False

def cam(angle=None, distance=None, target=None, pos=None, fov=None, target_relative_angle=None, light_angle=None):
	'''Set camera angle, distance, target object, position, fov. target_relative_angle=True means that the angle
	   is relative to your target object rather than absolute. light_angle is used to change the direction of the
	   directional light in the scene.'''
	_tryinit()
	angle = tovec3(angle)
	gameapi.cam(angle, distance, target.id if target else target, tovec3(pos), fov, target_relative_angle)
	gameapi.light(tovec3(light_angle))
	# Update shadow variables for screen<-->world space transformations.
	global _cam_angle,_cam_distance,_cam_target,_cam_lookat,_cam_fov_radians
	if angle: 	_cam_angle = angle
	if distance:	_cam_distance = distance
	if target:	_cam_target = target
	if pos:		_cam_lookat = tovec3(pos)
	if fov:		_cam_fov_radians = radians(fov)
	if target_relative_angle != None: _cam_relative = target_relative_angle

def bgcol(col):
	_tryinit()
	gameapi.setbgcolor(col)

def fog(near,far):
	_tryinit()
	gameapi.fog(near,far)

def gravity(g, bounce=None, friction=None):
	'''Sets physical gravity force. bounce factor [0,1] is used to control how much new objects created
	   will bounce, 0=low bounce and 1=preserve energy. friction factor [0,1] controls how much friction
	   new objects created will have.'''
	_tryinit()
	gameapi.gravity(tovec3(g))
	if bounce:
		gameapi.bounce(bounce)
	if friction:
		gameapi.friction(friction)

def create_ascii_object(ascii, pos=None, vel=None, avel=None, mass=None, col=None, mat='flat', static=False, physmesh=False):
	'''static=True means object if fixed in absolute space. Only three types of materials exist: flat, smooth and checker.'''
	global _last_ascii_top_left_offset,asc2obj_lookup
	physmesh = True if physmesh==True else False
	# Keep a small cache of generated objects. Most small prototypes will reuse shapes.
	gfx = None
	for s,g,p,lo in asc2obj_lookup:
		if s == ascii+str(physmesh):
			gfx,phys,_last_ascii_top_left_offset = g,p,lo
	if not gfx:
		gfx,phys = asc2obj.str2obj(ascii, fast=_fast_ascii_generate, force_phys_mesh=physmesh)
		_last_ascii_top_left_offset = -vec3(min(gfx.vertices, key=lambda v:v.x).x, min(gfx.vertices, key=lambda v:v.z).z, min(gfx.vertices, key=lambda v:v.y).y)
		asc2obj_lookup.append((ascii+str(physmesh),gfx,phys,_last_ascii_top_left_offset))
		if len(asc2obj_lookup) > 10:
			del asc2obj_lookup[0]
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_mesh_object(vertices, triangles, pos=None, vel=None, avel=None, mass=None, col=None, mat='smooth', static=False):
	'''static=True means object if fixed in absolute space. Only three types of materials exist: flat, smooth and checker.'''
	gfx,phys = objgen.createmesh(vertices,triangles)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_cube_object(pos=None, side=1, vel=None, avel=None, mass=None, mat='checker', static=False):
	'''static=True means object if fixed in absolute space. Only three types of materials exist: flat, smooth and checker.'''
	gfx,phys = objgen.createcube(side)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=None, mat=mat)

def create_sphere_object(pos=None, radius=1, vel=None, avel=None, mass=None, col=None, mat='smooth', static=False):
	'''static=True means object if fixed in absolute space. Only three types of materials exist: flat, smooth and checker.'''
	gfx,phys = objgen.createsphere(radius)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def release_all_objects():
	'''Delete all objects.'''
	gameapi.release_all_objects()
	global _objects
	_objects = {}

def last_ascii_top_left_offset():
	'''Returns the distance from the center of the last create ASCII object to the top-left-front corner of its AABB.'''
	return _last_ascii_top_left_offset

def explode(pos, vel=vec3(), strength=1):
	'''Show a graphical explosion and play an explosion sound.'''
	_tryinit()
	if timeout(0.5,timer=-153,first_hit=True):
		gameapi.explode(tovec3(pos),tovec3(vel),strength)

def sound(snd, pos=vec3(), vel=vec3(), volume=5):
	'''Play a sound using the given position, velocity and volume. Volume controls audible distance.'''
	_tryinit()
	gameapi.playsound(snd+'.wav', tovec3(pos), tovec3(vel), volume)

def collided_objects():
	'''Returns all objects that collided last loop.'''
	return set([o for o,_,_,_ in collisions()] + [o2 for _,o2,_,_ in collisions()])

def collisions():
	'''Returns all collisions that occured last loop. Each collision is a 4-tuple:
	   (object1,object2,force,position).'''
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
	taps_coords = [[float(w) for w in line.split()[:6]] for line in gameapi.taps().split('\n') if line]
	_taps = []
	used_invalidations = set()
	for x,y,startx,starty,vx,vy in taps_coords:
		if (startx,starty) not in _invalidated_taps:
			_taps.append(Tap(x,y,startx,starty,vx,vy))
		else:
			used_invalidations.add((startx,starty))
	for tapstart in set(_invalidated_taps):
		if not tapstart in used_invalidations:
			_invalidated_taps.remove(tapstart)
	return _taps

def closest_tap(pos):
	'''The parameter pos is a 3D coordinate. Returns an instance of the Tap class or None.'''
	if not taps():
		return None
	x,y = _world2screen(pos)
	tap = min(taps(), key=lambda t: t._distance2(x,y))
	tap.close_pos = pos
	return tap

clicks = taps
closest_click = closest_tap

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
	rel.pitch = -_accelerometer_calibration.angle_x(acc)
	rel.roll = -_accelerometer_calibration.angle_y(acc)
	return rel


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
	for j in [joy for jid,joy in _joysticks.items() if jid not in used_joys and not joy.sloppy]:
		j.x = j.y = 0.0

def _create_object(gfx, phys, static, pos, vel, avel, mass, col, mat):
	_tryinit()
	objpos = tovec3(pos) if tovec3(pos) else vec3()
	gameapi.initgfxmesh(gfx.q, gfx.pos, gfx.vertices, gfx.indices)
	gameapi.clearprepphys()
	for p in phys:
		if 'Box' in str(type(p)):
			gameapi.initphysbox(p.q, p.pos+objpos, p.size)
		elif 'Sphere' in str(type(p)):
			gameapi.initphyssphere(p.q, p.pos+objpos, p.radius)
		elif 'Mesh' in str(type(p)):
			gameapi.initphysmesh(p.q, p.pos+objpos, p.vertices, p.indices)
		objpos = vec3()	# Only root should be moved, the rest have relative positions.
	if col:
		gameapi.setpencolor(col)
	oid = gameapi.createobj(static, mat)
	if _wait_until_loaded:
		gameapi.waitload(oid)
	o = Obj(oid)
	global _objects
	_objects[oid] = o
	if vel:
		o.vel(tovec3(vel))
	if avel:
		o.avel(tovec3(avel))
	if mass:
		o.mass(mass)
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
	elif engine_type in (push_abs_engine, push_rel_engine):
		max_velocity[0] = 30 if not max_velocity[0] else max_velocity[0]
	elif engine_type in (push_turn_abs_engine, push_turn_rel_engine):
		max_velocity[0] = 0.1 if not max_velocity[0] else max_velocity[0]
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
		_cam_pos = _cam_target.pos()
		if not _cam_pos:
			_cam_target = None
			return _update_cam_shadow()
	elif _cam_lookat:
		_cam_pos = _cam_lookat
	else:
		_cam_pos = vec3()
	_cam_q = quat().rotate_y(_cam_angle.y).rotate_x(_cam_angle.x).rotate_z(_cam_angle.z)
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
