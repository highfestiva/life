#!/usr/bin/env python3

from math import cos,sin,sqrt,pi,atan2
from random import random


_axis = None


def almosteq(a,b):
	return int(((a-b) if a>b else (b-a))*1000) == 0

def rndvec():
	return vec3(*[random()*2-1 for _ in range(3)])

def rndquat():
	return quat(*[random()-0.5 for _ in range(4)]).normalize()

def intvec3(v):
	return vec3(int(v.x),int(v.y),int(v.z))

def sin2(a):
	x = sin(a)
	return x*x

def cos2(a):
	x = cos(a)
	return x*x

def lerp(a,b,t):
	return a*(1-t) + b*t

def clamp(val, minval, maxval):
	return max(minval, min(maxval, val))

def angmod(a):
    if a > pi:
        return a-2*pi
    if a < -pi:
        return a+2*pi
    return a

def tovec3(v):
	if v == None:
		return v
	if type(v) == vec3:
		return v
	try:
		return vec3(*v)
	except:
		return vec3(v,v,v)

def toaxis(v):
	global _axis
	if not _axis:
		_axis = (vec3(1,0,0),vec3(-1,0,0),vec3(0,1,0),vec3(0,-1,0),vec3(0,0,1),vec3(0,0,-1))
	return min(_axis, key=lambda a:(a-v).length2())

def toquat(q):
	if q == None:
		return q
	if type(q) == quat:
		return q
	elif len(q) == 4:
		return quat(*q)


# Helper lambdas.
rotx = lambda a: quat().rotate_x(a)
roty = lambda a: quat().rotate_y(a)
rotz = lambda a: quat().rotate_z(a)


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

	def project_on_vector(self, vec_normal):
		return vec_normal*(self*vec_normal)

	def project_on_plane(self, plane_normal):
		return self-self.project_on_vector(plane_normal)

	def normalize(self, length=1):
		l = self.length()
		if l < 0.0001: return self
		il = length/l
		return vec3(self.x*il, self.y*il, self.z*il)

	def reduce(self, length=1):
		l = self.length()
		if l < length: return self
		il = length/l
		return vec3(self.x*il, self.y*il, self.z*il)

	def length(self):
		return sqrt(self.x*self.x + self.y*self.y + self.z*self.z)

	def length2(self):
		return self.x*self.x + self.y*self.y + self.z*self.z

	def limit(self, maxlen):
		if self.length2() > maxlen*maxlen:
			return self.normalize(maxlen)
		return self

	def abs(self):
		return vec3(abs(self.x), abs(self.y), abs(self.z))

	def with_x(self,x):
		return vec3(x,self.y,self.z)

	def with_y(self,y):
		return vec3(self.x,y,self.z)

	def with_z(self,z):
		return vec3(self.x,self.y,z)

	def angle_x(self,v):
		return angmod(atan2(v.z,v.y) - atan2(self.z,self.y))

	def angle_y(self,v):
		return angmod(atan2(v.z,v.x) - atan2(self.z,self.x))

	def angle_z(self,v):
		return angmod(atan2(v.y,v.x) - atan2(self.y,self.x))

	def mulvec(self,v):
		return vec3(self.x*v.x, self.y*v.y, self.z*v.z)

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

	def normalize(self, length=1):
		l = self.length()
		if l < 0.0001: return self
		il = length/l
		return quat(*[q*il for q in self.q])

	def length(self):
		return sqrt(self.norm())

	def norm(self):
		return self.q[0]*self.q[0] + self.q[1]*self.q[1] + self.q[2]*self.q[2] + self.q[3]*self.q[3]

	def rotate_x(self,a):
		return self * quat(cos(a*0.5), sin(a*0.5), 0, 0)

	def rotate_y(self,a):
		return self * quat(cos(a*0.5), 0, sin(a*0.5), 0)

	def rotate_z(self,a):
		return self * quat(cos(a*0.5), 0, 0, sin(a*0.5))

	def yaw_pitch_roll(self, v):
		return self.rotate_z(v.x).rotate_x(v.y).rotate_y(v.z).normalize()

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
		return quat(*[f*v for f in self.q])

	def __truediv__(self,divisor):
		return quat(self.q[0]/divisor, self.q[1]/divisor, self.q[2]/divisor, self.q[3]/divisor)

	def __add__(self,q):
		return quat(*[qs+qo for qs,qo in zip(self.q,q)])
