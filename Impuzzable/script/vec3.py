from math import sqrt

def almosteq(a,b):
	return int(((a-b) if a>b else (b-a))*10000) == 0

class vec3:
	def __init__(self, *args):
		if type(args[0]) == vec3:
			v = args[0]
			self.x,self.y,self.z = v.x,v.y,v.z
		else:
			self.x,self.y,self.z = args

	def set(self,v):
		self.x,self.y,self.z = v.x,v.y,v.z

	def cross(self,v):
		return vec3(self.y*v.z-self.z*v.y, self.z*v.x-self.x*v.z, self.x*v.y-self.y*v.x)

	def normalize(self):
		l = self.length()
		if l < 0.0001: return self
		il = 1/l
		return vec3(self.x*il, self.y*il, self.z*il)

	def length(self):
		return sqrt(self.x*self.x + self.y*self.y + self.z*self.z)

	def lengthsq(self):
		return self.x*self.x + self.y*self.y + self.z*self.z

	def abs(self):
		return vec3(abs(self.x), abs(self.y), abs(self.z))

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
		return almosteq(self.x,v.x) and almosteq(self.y,v.y) and almosteq(self.z,v.z)

	def __hash__(self):
		return hash((self.x,self.y,self.z))

	def __str__(self):
		return '(%g,%g,%g)' % tuple([float(f) for f in iter(self)])

	def __repr__(self):
		return 'vec3(%g,%g,%g)' % tuple([float(f) for f in iter(self)])

	def __iter__(self):
		return iter([self.x,self.y,self.z])
