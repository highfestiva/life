#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from trabant.math import *
import time


turn_engine,roll_engine,push_engine,rotor_engine,collective_engine,tilt_engine = 'turn roll push rotor collective tilt'.split()
hinge_joint,hinge2_joint = 'hinge hinge2'.split()
sound_explosion,sound_ping,sound_bang,sound_engine_hizz,sound_engine_wobble,sound_engine_combustion = 'explosion ping bang hizz wobble combustion'.split()
last_ascii_top_left_offset = vec3(0,0,0)
_taps = None
_collisions = None
_joysticks = {}
_timers = {}


class Engine:
	def __init__(self, id):
		self.id = id
	def force(self,f):
		try:
			gameapi.set_engine_force(self.id, iter(f))
		except TypeError:
			gameapi.set_engine_force(self.id, [f])

class Obj:
	def __init__(self, id):
		self.id = id
		self.engine = []
	def pos(self, pos=None, orientation=None):
		if pos:
			gameapi.setpos(self.id, pos)
		if oirientation:
			gameapi.setorientation(self.id, orientation)
		if not pos and not orientation:
			return gameapi.getpos(self.id)
	def vel(self, vel=None):
		if vel:
			gameapi.setvel(self.id, vel)
		else:
			return gameapi.getvel(self.id)
	def avel(self, avel=None):
		'''Angular velocity.'''
		if avel:
			gameapi.setavel(self.id, avel)
		else:
			return gameapi.getavel(self.id)
	def weight(self, w):
		if w:
			gameapi.setweight(self.id, w)
		else:
			return gameapi.getweight(self.id)
	def col(self, col=None):
		if col:
			gameapi.setcol(self.id, col)
		else:
			return gameapi.getcol(self.id)
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
		eid = gameapi.create_engine(self.id, engine_type, max_velocity, offset, sound):
		self.engine += [Engine(eid)]
		return self.engine[-1]
	def create_joint(self, joint_type, obj2, axis):
		gameapi.create_joint(self.id, joint_type, obj2, axis)
	def release(self):
		gameapi.releaseobj(self.id)


class Tap:
	def __init__(self, xy):
		self.x,self.y = xy
	def pos3d(self, z):
		return _screen2world(x,y)*z
	def vel3d(self,z):
		return vec3(0,0,0)
	def _distance2(self, x, y):
		return (self.x-x)**2+(self.y-y)**2


class Joystick:
	def __init__(self, crdx, crdy):
		'''Screen X,Y in [0,1].'''
		self.crdx,self.crdy = crdx,crdy
		self.x,self.y = 0,0


def loop():
	sleep(0.1)
	global _taps,_collisions
	_taps,_collisions = None,None
	_poll_joysticks()
	return gameapi.isopen()

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

def cam(angle=None, distance=None, target=None, fov=None):
	gameapi.cam(angle, distance, target, fov)

def fog(distance):
	gameapi.fog(distance)

def gravity(g):
	gameapi.gravity(g)

def create_ascii_object(ascii, pos=None, vel=None, angular_velocity=None, col=None, static=False):
	global last_ascii_top_left_offset
	gfx,phys = asc2obj.asc2obj(ascii)
	last_ascii_top_left_offset = asc2obj.last_ascii_top_left_offset
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=col)

def create_mesh_object(vertices, triangles, pos=None, vel=None, angular_velocity=None, static=False):
	gfx,phys = objgen.createmesh(vertices,triangles)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=col)

def create_cube_object(pos=None, side=1, vel=None, angular_velocity=None, static=False):
	gfx,phys = objgen.createcube(side)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=None)

def create_sphere_object(pos=None, radius=1, vel=None, angular_velocity=None, col=None, static=False):
	gfx,phys = objgen.createsphere(radius)
	return _create_object(gfx, phys, static, pos=pos, vel=vel, angular_velocity=angular_velocity, col=col)

def explode(pos, vel=None):
	gameapi.explode(pos,vel)

def sound(snd, pos, vel=None):
	gameapi.playsound(snd, pos, vel)

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
		_collisions = gameapi.pop_collisions()
	return _collisions

def taps():
	global _taps
	if _taps == None:
		_taps = gameapi.taps()
	return _taps

def closest_tap(pos):
	if not taps():
		return None
	x,y = _world2screen(pos)
	return min(taps(), key=lambda t: t._distance2(x,y))

def create_joystick(xy):
	global _joysticks
	j = Joystick(*xy)
	_joysticks[j.id] = j
	return j

def accelerometer():
	return gameapi.accelerometer()

def _poll_joysticks():
	if not _joysticks:
		return
	for jid,x,y in gameapi.joystick_data():
		j = _joysticks[jid]
		j.x,j.y = x,y

def _create_object(gfx, phys, static, pos, vel, angular_velocity, col)
	oid = gameapi.createobj(gfx,phys,static,pos)
	gameapi.waitload(oid)
	o = Obj(oid)
	if vel:
		o.vel(vel)
	if angular_velocity:
		o.avel(angular_velocity)
	if col:
		o.col(col)
	return o

def _world2screen(crd):
	# Project according to current FoV and scale [0,1].
	return (0,0)

def _screen2world(x,y):
	# Project according to current FoV and scale from X and Y in [0,1] to Z=1.
	return vec3(x,y,1)

def _open():
	global _joysticks,_timers
	_joysticks,_timers = {},{}
	gameapi.open()
	gameapi.reset()	# Kill all objects and joysticks. Set some default values.
	cam(angle=(0,0,0), distance=10, target=None, fov=45)
	loop()	# Resets taps+collisions.

_open()
