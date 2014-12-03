class quat:
	def __init__(self, *args):
		if type(args[0]) == quat:
			self.q = args[0].q[:]
		else:
			self.q = [float(f) for f in args]

	# def __add__(self,v):
		# return quat(self.x+v.x, self.y+v.y, self.z+v.z)

	# def __sub__(self,v):
		# return quat(self.x-v.x, self.y-v.y, self.z-v.z)

	# def __eq__(self,v):
		# return self.x==v.x and self.y==v.y and self.z==v.z

	# def __truediv__(self,divisor):
		# return quat(self.x//divisor, self.y//divisor, self.z//divisor)

	# def __hash__(self):
		# return hash((self.x,self.y,self.z))

	def __str__(self):
		return 'quat(%g,%g,%g,%g)' % self.q

	def __repr__(self):
		return 'quat(%g,%g,%g,%g)' % self.q

	def __iter__(self):
		return iter(self.q)

	def __getitem__(self, i):
		return self.q[i]
