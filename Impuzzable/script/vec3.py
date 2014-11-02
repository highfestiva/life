class vec3:
	def __init__(self, *args):
		self.x,self.y,self.z = [int(i) for i in args]

	def __add__(self,v):
		return vec3(self.x+v.x, self.y+v.y, self.z+v.z)

	def __sub__(self,v):
		return vec3(self.x-v.x, self.y-v.y, self.z-v.z)

	def __eq__(self,v):
		return self.x==v.x and self.y==v.y and self.z==v.z

	def __truediv__(self,divisor):
		return vec3(self.x//divisor, self.y//divisor, self.z//divisor)

	def __hash__(self):
		return hash((self.x,self.y,self.z))

	def __str__(self):
		return '(%i,%i,%i)' % (self.x,self.y,self.z)

	def __repr__(self):
		return 'vec3(%i,%i,%i)' % (self.x,self.y,self.z)
