#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from trabant.math import *
import time


turn_engine,roll_engine,push_engine,rotor_engine,collective_engine,tilt_engine = 'turn roll push rotor collective tilt'.split()
hinge_joint,hinge2_joint = 'hinge hinge2'.split()
sound_explosion,sound_ping,sound_bang,sound_engine_hizz,sound_engine_wobble,sound_engine_combustion = 'explosion ping bang hizz wobble combustion'.split()
_taps = []


class Joint:
	pass


class Engine:
	def force(self,f):
		pass


class Obj:
	def __init__(self):
		self.engine,self.joint = [],[]
	def pos(self, pos=None, orientation=None):
		return vec3(0,0,0)
	def vel(self, vel=None):
		return vec3(0,0,0)
	def weight(self, w):
		pass
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
		self.engine += [Engine()]
		return self.engine[-1]
	def create_joint(self, joint_type, obj2, axis):
		self.joint += [Joint()]
		return self.joint[-1]
	def release(self):
		pass


class Tap:
	def __init__(self, xy):
		self.x,self.y = xy
	def pos3d(self, z):
		return vec3(0,0,z)
	def vel3d(self,z):
		return vec3(0,0,0)


class Joystick:
	def __init__(self):
		self.x,self.y = 0,0


def loop():
	global _taps
	sleep(0.1)
	_taps = [Tap((0.5,0.5))]
	return True

def sleep(t):
	time.sleep(t)

def timeout(t, timer=0):
	return False

def cam(angle=None, distance=None, target=None):
	pass

def fog(distance):
	pass

def gravity(g):
	pass

def create_ascii_object(ascii, pos=None, vel=None, angular_velocity=None, col=None, static=False):
	return Obj()

def create_mesh_object(vertices, indices, pos=None, vel=None, angular_velocity=None, static=False):
	return Obj()

def create_cube_object(pos=None, side=1, vel=None, angular_velocity=None, static=False):
	return Obj()

def create_sphere_object(pos=None, radius=1, vel=None, angular_velocity=None, col=None, static=False):
	return Obj()

def last_ascii_top_left_offset():
	return vec3(0,0,0)

def explode(pos, vel=None):
	pass

def sound(snd, pos, vel=None):
	pass

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
	return []

def taps():
	return _taps

def closest_tap(pos):
	return _taps[0] if _taps else None

def tap_above_plane(pos, normal):
	return False

def create_joystick(xy):
	return Joystick()

def accelerometer():
	return vec3(0,0,-9.8)
