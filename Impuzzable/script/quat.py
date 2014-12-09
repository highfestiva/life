from vec3 import almosteq

class quat:
	def __init__(self, *args):
		if type(args[0]) == quat:
			self.q = args[0].q[:]
		else:
			self.q = [float(f) for f in args]

	def __eq__(self,v):
		return all(almosteq(e,f) for e,f in zip(self.q,v.q))

	def __str__(self):
		return 'quat(%g,%g,%g,%g)' % tuple(float(f) for f in self.q)

	def __repr__(self):
		return 'quat(%g,%g,%g,%g)' % tuple(float(f) for f in self.q)

	def __iter__(self):
		return iter(self.q)

	def __getitem__(self, i):
		return self.q[i]
