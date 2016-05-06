"""Module contains silly object for testing."""

import cure, math, random, time

class JerkyObject(cure.ContextObject):
	"""3D object with physics that jumps and jerks."""
	def __init__(self):
		cure.ContextObject.__init__(self)

	def onCreate(self):
		"""Called upon creation by the game engine, but only after
		the game engine's context object has been created.
		The game engine's context object is required to be able to
		attach game data to us, such as alarms, physics and graphics."""
		self.setAlarm(1, random.uniform(5, 30))

	def onAlarm(self, alarmId):
		# A random force.
		force = random.uniform(10000, 30000)
		phi = 0.0
		# If object far from origo: seek back. Otherwise go "forward"=+y.
		x, y, z = self.getPosition()
		distance = math.sqrt(x*x+y*y)
		if x != 0 and distance > 20:
			theta = math.atan(y/x)
			if x > 0:
				theta += math.pi
			phi += 0.3
		else:
			theta = random.uniform(1.2, 1.8)
		phi += random.uniform(0.2, 0.9)
		# Add some torque.
		xo, yo, zo = random.uniform(-0.2, 0.2), random.uniform(-0.2, 0.2), random.uniform(-0.2, 0.2)
		self.applyForce(force, theta, phi, xo, yo, zo)
		# Call again in a few, randomly selected, number of seconds.
		if distance > 90:
			callbackTime = random.uniform(1, 3)
		elif distance > 50:
			callbackTime = random.uniform(2, 10)
		else:
			callbackTime = random.uniform(10, 25)
		callbackTime /= force/30000
		self.setAlarm(1, callbackTime)

	def onCollision(self, coordinate, forceDirection, area):
		volume = forceDirection[0]/100000.0
		if volume > 0.3:
			volume *= 2
			volume = min(volume, 1.0)
			pitch = 1.0-area/10.0
			pitch = max(pitch, 0.7)
			pitch = min(pitch, 1.4)
			print "?"
			self.playSound("bounce.wav", volume, pitch)

class Ground(cure.ContextObject):
	"""3D cube, which top consitutes ground."""
	def __init__(self):
		cure.ContextObject.__init__(self)

class Terrain(cure.ContextObject):
	"""A complete 3D LOD:ed terrain that consitutes the future ground."""
	def __init__(self, sideCount):
		cure.ContextObject.__init__(self)
		resolution = cure.TerrainPatch.getResolution()
		multiplyResolution = 2.0*math.pi/float(resolution)
		halfCount = sideCount/2
		self.terrainPatches = []
		for y in xrange(-halfCount, halfCount+1):
			row = []
			self.terrainPatches.append(row)
			for x in xrange(-halfCount, halfCount+1):
				patch = cure.TerrainPatch()
				patch.create(x, y)
				print "Creating patch on", x, ",", y
				coords = []
				for u in xrange(resolution):
					for v in xrange(resolution):
						coords.append(0.070*math.sin(4*u*multiplyResolution))	# x
						coords.append(0.040*math.cos(3*v*multiplyResolution))	# y
						coords.append(0.050*math.sin(v*multiplyResolution))		# z
				patch.setSurfaceCoordinates(tuple(coords))
				row.append(patch)
