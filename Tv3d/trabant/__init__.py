#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from math import radians,tan
from trabant.math import *
import trabant.asc2obj
import trabant.gameapi
import trabant.objgen
import time


turn_engine,roll_engine,push_engine,rotor_engine,collective_engine,tilt_engine = 'turn roll push rotor collective tilt'.split()
hinge_joint,hinge2_joint = 'hinge hinge2'.split()
sound_explosion,sound_ping,sound_bang,sound_engine_hizz,sound_engine_wobble,sound_engine_combustion = 'explosion ping bang hizz wobble combustion'.split()
_last_ascii_top_left_offset = None
asc2obj_lookup = []
_aspect_ratio = 1.33333
_taps = None
_collisions = None
_joysticks = {}
_timers = {}
_objects = {}
_cam_angle,_cam_distance,_cam_target,_cam_lookat,_cam_fov_radians = vec3(0,0,0),10,None,vec3(0,0,0),0.5
_cam_pos,_cam_q,_cam_inv_q = vec3(0,-10,0),quat(),quat()


class Engine:
	def __init__(self, oid, eid):
		self.oid,self.eid = oid,eid
	def force(self,f):
		try:
			gameapi.set_engine_force(self.oid, self.eid, iter(f))
		except TypeError:
			gameapi.set_engine_force(self.oid, self.eid, (f,0,0))

class Obj:
	def __init__(self, id):
		self.id = id
		self.engine = []
	def pos(self, pos=None, orientation=None):
		if not self.id:
			return
		if orientation:
			gameapi.orientation(self.id, orientation)
		else:
			return gameapi.pos(self.id, pos)
	def orientation(self, orientation=None):
		return gameapi.orientation(self.id, orientation)
	def vel(self, vel=None):
		return gameapi.vel(self.id, vel)
	def avel(self, avel=None):
		'''Angular velocity.'''
		return gameapi.avel(self.id, avel)
	def weight(self, w):
		return gameapi.weight(self.id, (w,))
	def col(self, col=None):
		return gameapi.col(self.id, col)
	def bounce_in_rect(self,ltn,rbf):
		p,v = self.pos(),self.vel()
		ltn,rbf = tovec3(ltn),tovec3(rbf)
		if p.x < ltn.x: v.x = +abs(v.x)
		if p.x > rbf.x: v.x = -abs(v.x)
		if p.y < ltn.y: v.y = +abs(v.y)
		if p.y > rbf.y: v.y = -abs(v.y)
		if p.z < ltn.z: v.z = +abs(v.z)
		if p.z > rbf.z: v.z = -abs(v.z)
		self.vel(v)
	def create_engine(self, engine_type, max_velocity=None, offset=None, sound=None):
		eid = gameapi.create_engine(self.id, engine_type, max_velocity, offset, sound)
		self.engine += [Engine(self.id, eid)]
		return self.engine[-1]
	def create_joint(self, joint_type, obj2, axis):
		return gameapi.create_joint(self.id, joint_type, obj2.id, axis)
	def release(self):
		gameapi.releaseobj(self.id)
		global _objects
		del _objects[self.id]
		self.id = None


class Tap:
	def __init__(self, x, y):
		self.x,self.y = x,y
	def pos3d(self, z):
		return _screen2world(self.x,self.y)*z
	def vel3d(self, z):
		return vec3(0,0,0)
	def _distance2(self, x, y):
		return (self.x-x)**2+(self.y-y)**2


class Joystick:
	def __init__(self, id):
		'''Screen X,Y in [0,1].'''
		self.id = id
		self.x,self.y = 0,0


def loop():
	sleep(0.1)
	global _taps,_collisions
	_taps,_collisions,_cam_pos = None,None,None
	_poll_joysticks()
	return gameapi.opened()

def sleep(t):
	time.sleep(t)

def timeout(t, timer=0):
	global _timers
	if not timer in _timers:
		_timers[timer] = time.time()
	if time.time() - _timers[timer] > t:
		_timers[timer] = time.time()
		return True
	return False

def cam(angle=None, distance=None, target=None, pos=None, fov=None):
	angle = tovec3(angle)
	gameapi.cam(angle, distance, target.id if target else -1, tovec3(pos), fov)
	# Update shadows for screen<-->world space transformations.
	global _cam_angle,_cam_distance,_cam_target,_cam_lookat,_cam_fov_radians
	if angle: 	_cam_angle = angle
	if distance:	_cam_distance = distance
	if target:	_cam_target = target
	if pos:		_cam_lookat = pos
	if fov:		_cam_fov_radians = radians(fov)

def fog(distance):
	gameapi.fog(distance)

def gravity(g):
	gameapi.gravity(tovec3(g))

def create_ascii_object(ascii, pos=None, vel=None, angular_velocity=None, col=None, static=False):
	global _last_ascii_top_left_offset,asc2obj_lookup
	# Keep a small cache of generated objects. Most small prototypes will reuse shapes.
	gfx = None
	for s,g,p,lo in asc2obj_lookup:
		if s == ascii:
			gfx,phys,_last_ascii_top_left_offset = g,p,lo
	if not gfx:
		gfx,phys = asc2obj.str2obj(ascii)
		_last_ascii_top_left_offset = asc2obj.last_centering_offset()
		asc2obj_lookup.append((ascii,gfx,phys,_last_ascii_top_left_offset))
		if len(asc2obj_lookup) > 10:
			del asc2obj_lookup[0]
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=col)

def create_mesh_object(vertices, triangles, pos=None, vel=None, angular_velocity=None, col=None, static=False):
	gfx,phys = objgen.createmesh(vertices,triangles)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=col)

def create_cube_object(pos=None, side=1, vel=None, angular_velocity=None, static=False):
	gfx,phys = objgen.createcube(side)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=None)

def create_sphere_object(pos=None, radius=1, vel=None, angular_velocity=None, col=None, static=False):
	gfx,phys = objgen.createsphere(radius)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=col)

def release_all_objects():
	gameapi.release_all_objects()
	global _objects
	_objects = {}

def last_ascii_top_left_offset():
	return _last_ascii_top_left_offset

def explode(pos, vel=None):
	gameapi.explode(tovec3(pos),tovec3(vel))

def sound(snd, pos, vel=None):
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

def collisions():
	global _collisions
	if _collisions == None:
		oids = [int(line.split()[0]) for line in gameapi.pop_collisions().split('\n') if line]
		_collisions = [_objects[oid] for oid in oids]
	return _collisions

def taps():
	global _taps
	if _taps == None:
		taps_coords = [[float(w) for w in line.split()[:4]] for line in gameapi.taps().split('\n') if line]
		_taps = [Tap(x,y) for x,y,startx,starty in taps_coords]
	return _taps

def closest_tap(pos):
	if not taps():
		return None
	x,y = _world2screen(pos)
	return min(taps(), key=lambda t: t._distance2(x,y))

def create_joystick(xy):
	jid = gameapi.create_joystick(*xy)
	global _joysticks
	j = Joystick(jid)
	_joysticks[j.id] = j
	return j

def accelerometer():
	return tovec3([float(a) for a in gameapi.accelerometer().split()])


########################################


def _poll_joysticks():
	if not _joysticks:
		return
	jdata = []
	for line in [l for l in gameapi.joystick_data().split('\n') if l]:
		w = line.split()
		jdata += (int(w[0]),float(w[1]),float(w[2]))
	for jid,x,y in jdata:
		j = _joysticks[jid]
		j.x,j.y = x,y

def _create_object(gfx, phys, static, pos, vel, angular_velocity, col):
	objpos = tovec3(pos) if pos else vec3(0,0,0)
	gameapi.initgfxmesh(gfx.q, gfx.pos, gfx.vertices, gfx.indices)
	gameapi.clearprepphys()
	for p in phys:
		if 'Box' in str(type(p)):
			gameapi.initphysbox(p.q, p.pos+objpos, p.size)
		elif 'Sphere' in str(type(p)):
			gameapi.initphysshpere(p.q, p.pos+objpos, p.radius)
		elif 'Mesh' in str(type(p)):
			gameapi.initphysmesh(p.q, p.pos+objpos, p.vertices, p.indices)
	oid = gameapi.createobj(static)
	gameapi.waitload(oid)
	o = Obj(oid)
	global _objects
	_objects[oid] = o
	if vel:
		o.vel(tovec3(vel))
	if angular_velocity:
		o.avel(tovec3(angular_velocity))
	if col:
		o.col(col)
	return o

def _world2screen(crd):
	'''Return screen X,Y in [0,1].'''
	_update_cam_shadow()
	crd = _cam_inv_q*(crd-_cam_pos)
	iar = 1/_get_aspect_ratio()
	itana = 1/tan(_cam_fov_radians*0.5);
	x,z = crd.x/crd.y,crd.z/crd.y
	return x*itana*iar+1, -z*itana+1

def _screen2world(x,y):
	'''Return world X,Z scaled to Y=1.'''
	_update_cam_shadow()
	tana = tan(_cam_fov_radians*0.5);
	dx = tana * (x*2-1) * _get_aspect_ratio()
	dy = tana * (1-y*2);
	return (_cam_q * vec3(dx, 1, dy).normalize()) + _cam_pos

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
		_cam_pos = vec3(0,0,0)
	_cam_q = quat().rotate_y(_cam_angle.y).rotate_x(_cam_angle.x).rotate_z(_cam_angle.z)
	_cam_inv_q = _cam_q.inverse()
	if _cam_distance:
		_cam_pos += _cam_q * vec3(0,-_cam_distance,0)

def _get_aspect_ratio():
	if timeout(10,timer=-152):
		global _aspect_ratio
		_aspect_ratio = gameapi.get_aspect_ratio()
	return _aspect_ratio

def _open():
	global _joysticks,_timers
	_joysticks,_timers = {},{}
	gameapi.open()
	gameapi.reset()	# Kill all joysticks. Set some default values.
	gameapi.release_all_objects()
	cam(angle=(0,0,0), distance=10, target=None, fov=45)
	loop()	# Resets taps+collisions.

_open()
