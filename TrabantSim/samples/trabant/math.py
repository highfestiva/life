#!/usr/bin/env python3

from math import cos,sin,sqrt,pi,atan2
from random import random


def almosteq(a,b):
	return int(((a-b) if a>b else (b-a))*10000) == 0

def rndvec():
	return vec3(*[random()*2-1 for _ in range(3)])

def sin2(a):
	x = sin(a)
	return x*x

def cos2(a):
	x = cos(a)
	return x*x

def tovec3(v):
	if v == None:
		return v
	if type(v) == vec3:
		return v
	elif len(v) == 3:
		return vec3(*v)

def toquat(q):
	if q == None:
		return q
	if type(q) == quat:
		return q
	elif len(q) == 4:
		return quat(*q)


class vec3:
	def __init__(self, *args):
		if not args:
			self.x,self.y,self.z = 0,0,0
		elif type(args[0]) == vec3:
			v = args[0]
			self.x,self.y,self.z = v.x,v.y,v.z
		else:
			self.x,self.y,self.z = args

	def set(self,v):
		self.x,self.y,self.z = v.x,v.y,v.z

	def cross(self,v):
		return vec3(self.y*v.z-self.z*v.y, self.z*v.x-self.x*v.z, self.x*v.y-self.y*v.x)

	def normalize(self, length=1):
		l = self.length()
		if l < 0.0001: return self
		il = length/l
		return vec3(self.x*il, self.y*il, self.z*il)

	def length(self):
		return sqrt(self.x*self.x + self.y*self.y + self.z*self.z)

	def lengthsq(self):
		return self.x*self.x + self.y*self.y + self.z*self.z

	def abs(self):
		return vec3(abs(self.x), abs(self.y), abs(self.z))

	def with_x(self,x):
		return vec3(x,self.y,self.z)

	def with_y(self,y):
		return vec3(self.x,y,self.z)

	def with_z(self,z):
		return vec3(self.x,self.y,z)

	def angle_x(self,v):
		return atan2(v.z,v.y) - atan2(self.z,self.y)

	def angle_y(self,v):
		return atan2(v.z,v.x) - atan2(self.z,self.x)

	def angle_z(self,v):
		return atan2(v.y,v.x) - atan2(self.y,self.x)

	def __add__(self,v):
		return vec3(self.x+v.x, self.y+v.y, self.z+v.z)

	def __sub__(self,v):
		return vec3(self.x-v.x, self.y-v.y, self.z-v.z)

	def __mul__(self,v):
		if type(v) == vec3:
			return self.x*v.x + self.y*v.y + self.z*v.z
		return vec3(self.x*v, self.y*v, self.z*v)

	def __truediv__(self,divisor):
		return vec3(self.x/divisor, self.y/divisor, self.z/divisor)

	def __neg__(self):
		return vec3(-self.x, -self.y, -self.z)

	def __eq__(self,v):
		if v:
			return almosteq(self.x,v.x) and almosteq(self.y,v.y) and almosteq(self.z,v.z)
		return False

	def __hash__(self):
		return hash((self.x,self.y,self.z))

	def __str__(self):
		return '(%g,%g,%g)' % tuple([float(f) for f in iter(self)])

	def __repr__(self):
		return 'vec3(%g,%g,%g)' % tuple([float(f) for f in iter(self)])

	def __iter__(self):
		return iter([self.x,self.y,self.z])

	def __getitem__(self, i):
		return [self.x,self.y,self.z][i]


class quat:
	def __init__(self, *args):
		if not args:
			self.q = [1,0,0,0]
		elif type(args[0]) == quat:
			self.q = args[0].q[:]
		else:
			self.q = [float(f) for f in args]

	def inverse(self):
		l2 = self.norm()
		conjugate = self.q[:]
		conjugate[1],conjugate[2],conjugate[3] = -conjugate[1],-conjugate[2],-conjugate[3]
		return quat(*conjugate)/l2

	def norm(self):
		return self.q[0]*self.q[0] + self.q[1]*self.q[1] + self.q[2]*self.q[2] + self.q[3]*self.q[3]

	def rotate_x(self,a):
		q = quat(cos(a*0.5), sin(a*0.5), 0, 0);
		return self*q

	def rotate_y(self,a):
		q = quat(cos(a*0.5), 0, sin(a*0.5), 0);
		return self*q

	def rotate_z(self,a):
		q = quat(cos(a*0.5), 0, 0, sin(a*0.5));
		return self*q

	def __eq__(self,q):
		q = toquat(q)
		if q:
			return all(almosteq(e,f) for e,f in zip(self.q,q.q))
		return False

	def __str__(self):
		return 'quat(%g,%g,%g,%g)' % tuple(float(f) for f in self.q)

	def __repr__(self):
		return 'quat(%g,%g,%g,%g)' % tuple(float(f) for f in self.q)

	def __iter__(self):
		return iter(self.q)

	def __getitem__(self, i):
		return self.q[i]

	def __mul__(self,v):
		if type(v) == vec3:
			q = self * quat(0, v.x, v.y, v.z) * self.inverse()
			return vec3(q[1], q[2], q[3])
		elif type(v) == quat:
			a = self.q[0] * v.q[0] - self.q[1] * v.q[1] - self.q[2] * v.q[2] - self.q[3] * v.q[3];
			b = self.q[0] * v.q[1] + self.q[1] * v.q[0] + self.q[2] * v.q[3] - self.q[3] * v.q[2];
			c = self.q[0] * v.q[2] + self.q[2] * v.q[0] + self.q[3] * v.q[1] - self.q[1] * v.q[3];
			d = self.q[0] * v.q[3] + self.q[3] * v.q[0] + self.q[1] * v.q[2] - self.q[2] * v.q[1];
			return quat(a,b,c,d)

	def __truediv__(self,divisor):
		return quat(self.q[0]/divisor, self.q[1]/divisor, self.q[2]/divisor, self.q[3]/divisor)
