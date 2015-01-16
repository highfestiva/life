#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from math import radians,tan
from trabant.math import *
import trabant.asc2obj
import trabant.gameapi
import trabant.objgen
import time


roll_turn_engine,roll_engine,push_abs_engine,push_rel_engine,push_abs_turn_engine,push_rel_turn_engine,gyro_engine,rotor_engine,tilt_engine = 'roll_turn roll push_abs push_rel push_turn_abs push_turn_rel gyro rotor tilt'.split()
hinge_joint,suspend_hinge_joint,turn_hinge_joint = 'hinge suspend_hinge turn_hinge'.split()
sound_explosion,sound_ping,sound_bang,sound_engine_hizz,sound_engine_wobble,sound_engine_combustion,sound_engine_rotor = 'explosion ping bang hizz wobble combustion rotor'.split()

_wait_until_loaded = True
_has_opened = False
_last_ascii_top_left_offset = None
asc2obj_lookup = []
_aspect_ratio = 1.33333
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
		try:
			gameapi.set_engine_force(self.oid, self.eid, iter(f))
		except TypeError:
			gameapi.set_engine_force(self.oid, self.eid, (f,0,0))
	def addsound(self, sound, intensity=1):
		if sound == sound_engine_rotor:
			gameapi.addtag(self.oid, 'engine_sound', [0, 0.001,0.8*intensity,1.4, 0,20,1, 0,1,1, 1], [sound+'.wav'], [0], [self.eid], [])
		else:
			gameapi.addtag(self.oid, 'engine_sound', [1, 1,5*intensity,1, 1,20,1, 0,0.5,1, 1], [sound+'.wav'], [0], [self.eid], [])

class Obj:
	def __init__(self, id):
		self.id = id
		self.engine = []
	def pos(self, pos=None, orientation=None):
		if pos:
			gameapi.pos(self.id, pos)
		if orientation:
			gameapi.orientation(self.id, orientation)
		if not pos and not orientation:
			return gameapi.pos(self.id, None)
	def orientation(self, orientation=None):
		return gameapi.orientation(self.id, orientation)
	def vel(self, vel=None, avel=None):
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
		return gameapi.col(self.id, col)
	def bounce_in_rect(self,ltn,rbf):
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
		target_efcts = [(t.id,efct) for t,efct in targets] if targets else []
		eid = gameapi.create_engine(self.id, engine_type, max_velocity, offset, strength, friction, target_efcts)
		self.engine += [Engine(self.id, eid)]
		if sound:
			self.engine[-1].addsound(sound, 1)
		return self.engine[-1]
	def create_joint(self, joint_type, obj2, axis=None, stop=None, spring=None):
		return gameapi.create_joint(self.id, joint_type, obj2.id, axis, stop, spring)
	def add_stabilizer(self, force=1):
		gameapi.addtag(self.id, 'upright_stabilizer', [force], [], [0], [], [])
	def release(self):
		gameapi.releaseobj(self.id)
		global _objects
		del _objects[self.id]
		self.id = None
	def released(self):
		return self.id == None


class Tap:
	def __init__(self, x, y, startx, starty):
		self.x,self.y = x,y
		self.startx,self.starty = startx,starty
	def pos3d(self, z=None):
		if not z:
			z = _cam_distance
		return _screen2world(self.x,self.y, z)
	def vel3d(self, z=None):
		if not z:
			z = _cam_distance
		return vec3()	# TODO
	def invalidate(self):
		global _invalidated_taps
		_invalidated_taps.add((self.startx,self.starty))
	def _distance2(self, x, y):
		return (self.x-x)**2+(self.y-y)**2


class Joystick:
	def __init__(self, id, sloppy):
		'''Screen X,Y in [0,1].'''
		self.id = id
		self.sloppy = sloppy
		self.x,self.y = 0,0


def open(**kwargs):
	global _joysticks,_timers,_has_opened,_accelerometer_calibration
	_joysticks,_timers,_has_opened = {},{},True
	if not gameapi.open(**kwargs):
		raise Exception('unable to connect to simulator')
	cam(angle=(0,0,0), distance=10, target=None, fov=45)
	loop(delay=0)	# Resets taps+collisions.
	_accelerometer_calibration = accelerometer()

def debug(enable=True):
	gameapi.debug(enable)

def userinfo(message=''):
	gameapi.userinfo(message)

def loop(delay=0.1):
	_tryopen()
	sleep(delay)
	global _taps,_collisions,_cam_pos
	_taps,_collisions,_cam_pos = None,None,None
	_poll_joysticks()
	return gameapi.opened()

def sleep(t):
	time.sleep(t)

def timeout(t, timer=0, first_hit=False):
	global _timers
	if not timer in _timers:
		_timers[timer] = time.time()
		if first_hit:
			return True
	if time.time() - _timers[timer] > t:
		_timers[timer] = time.time()
		return True
	return False

def cam(angle=None, distance=None, target=None, pos=None, fov=None, use_relative_angle=None):
	_tryopen()
	angle = tovec3(angle)
	gameapi.cam(angle, distance, target.id if target else None, tovec3(pos), fov, use_relative_angle)
	# Update shadows for screen<-->world space transformations.
	global _cam_angle,_cam_distance,_cam_target,_cam_lookat,_cam_fov_radians
	if angle: 	_cam_angle = angle
	if distance:	_cam_distance = distance
	if target:	_cam_target = target
	if pos:		_cam_lookat = tovec3(pos)
	if fov:		_cam_fov_radians = radians(fov)
	if use_relative_angle != None:	_cam_relative = use_relative_angle

def fog(distance):
	_tryopen()
	gameapi.fog(distance)

def gravity(g, bounce=None, friction=None):
	_tryopen()
	gameapi.gravity(tovec3(g))
	if bounce:
		gameapi.bounce(bounce)
	if friction:
		gameapi.friction(friction)

def create_ascii_object(ascii, pos=None, vel=None, avel=None, mass=None, col=None, mat='flat', static=False, physmesh=False):
	global _last_ascii_top_left_offset,asc2obj_lookup
	# Keep a small cache of generated objects. Most small prototypes will reuse shapes.
	gfx = None
	for s,g,p,lo in asc2obj_lookup:
		if s == ascii+str(physmesh):
			gfx,phys,_last_ascii_top_left_offset = g,p,lo
	if not gfx:
		gfx,phys = asc2obj.str2obj(ascii, force_phys_mesh=physmesh)
		_last_ascii_top_left_offset = -vec3(min(gfx.vertices, key=lambda v:v.x).x, min(gfx.vertices, key=lambda v:v.z).z, min(gfx.vertices, key=lambda v:v.y).y)
		asc2obj_lookup.append((ascii+str(physmesh),gfx,phys,_last_ascii_top_left_offset))
		if len(asc2obj_lookup) > 10:
			del asc2obj_lookup[0]
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_mesh_object(vertices, triangles, pos=None, vel=None, avel=None, mass=None, col=None, mat='smooth', static=False):
	gfx,phys = objgen.createmesh(vertices,triangles)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def create_cube_object(pos=None, side=1, vel=None, avel=None, mass=None, mat='checker', static=False):
	gfx,phys = objgen.createcube(side)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=None, mat=mat)

def create_sphere_object(pos=None, radius=1, vel=None, avel=None, mass=None, col=None, mat='smooth', static=False):
	gfx,phys = objgen.createsphere(radius)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, avel=avel, mass=mass, col=col, mat=mat)

def release_all_objects():
	gameapi.release_all_objects()
	global _objects
	_objects = {}

def last_ascii_top_left_offset():
	return _last_ascii_top_left_offset

def wait_until_loaded(wait=True):
	global _wait_until_loaded
	_wait_until_loaded = wait

def explode(pos, vel=vec3()):
	_tryopen()
	gameapi.explode(tovec3(pos),tovec3(vel))

def sound(snd, pos=vec3(), vel=vec3()):
	_tryopen()
	gameapi.playsound(snd, tovec3(pos), tovec3(vel))

def rect_bound(pos,ltn,rbf):
	pos,ltn,rbf = tovec3(pos),tovec3(ltn),tovec3(rbf)
	if pos.x < ltn.x: pos.x = ltn.x
	if pos.x > rbf.x: pos.x = rbf.x
	if pos.y < ltn.y: pos.y = ltn.y
	if pos.y > rbf.y: pos.y = rbf.y
	if pos.z < ltn.z: pos.z = ltn.z
	if pos.z > rbf.z: pos.z = rbf.z
	return pos

def collided_objects():
	return set(o for o,_,_,_ in collisions())

def collisions():
	global _collisions
	if _collisions == None:
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

def taps():
	global _taps
	if _taps != None:
		return _taps
	taps_coords = [[float(w) for w in line.split()[:4]] for line in gameapi.taps().split('\n') if line]
	_taps = []
	used_invalidations = set()
	for x,y,startx,starty in taps_coords:
		if (startx,starty) not in _invalidated_taps:
			_taps.append(Tap(x,y,startx,starty))
		else:
			used_invalidations.add((startx,starty))
	for tapstart in set(_invalidated_taps):
		if not tapstart in used_invalidations:
			_invalidated_taps.remove(tapstart)
	return _taps

def closest_tap(pos):
	if not taps():
		return None
	x,y = _world2screen(pos)
	tap = min(taps(), key=lambda t: t._distance2(x,y))
	tap.close_pos = pos
	return tap

def create_joystick(xy, sloppy=False):
	_tryopen()
	jid = gameapi.create_joystick(*xy, sloppy=sloppy)
	global _joysticks
	j = Joystick(jid, sloppy)
	_joysticks[j.id] = j
	return j

def accelerometer(relative=False):
	_tryopen()
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
	_tryopen()
	objpos = tovec3(pos) if pos else vec3()
	# if mat != 'smooth':
		# gfx = objgen.flatten_mesh(gfx)
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

def _tryopen():
	if not _has_opened:
		open()
