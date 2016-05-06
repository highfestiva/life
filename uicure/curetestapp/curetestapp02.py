""" Test application that opens a screen and initializes a few game objects.

The game objects are self-sustained Python instances that may or may not have
3D-models and physical geometries attached to them. One of the game objects
control the camera.

Each game object's Python instance (context object) may have an onTick()
method that is called each frame."""



import cure, curesettings, math, random



class CureTestApplication(cure.ContextObject):
	""" Test application for Cure.
	Opens a screen and moves around a few objects on it."""
	def __init__(self, settingsFilename):
		"""Loads settings from a file."""
		cure.ContextObject.__init__(self)
		import os
		self.settingsFilenameBase, self.settingFileExtension = os.path.splitext(settingsFilename)
		self.settings = curesettings.Settings()
		self.settings.load(self.settings.profile.getUserName()+"-auto"+self.settingFileExtension)
		self.settings.load(self.settingsFilenameBase+self.settingFileExtension)
		self.settings.load(self.settings.profile.getUserName()+self.settingFileExtension)
		self.openScreen()
		self.createContent()

	def __del__(self):
		"""Saves settings to a file."""
		self.settings.save(self.settings.profile.getUserName()+"-auto"+self.settingFileExtension)

	def openScreen(self):
		"""Creates a screen, using properties from curesettings' gfx part."""
		renderer = self.settings.gfx.renderer.upper()
		if renderer == "SOFTWARE":	renderType = cure.Graphics.renderType.SOFTWARE
		elif renderer == "OPENGL":	renderType = cure.Graphics.renderType.OPENGL
		elif renderer == "DIRECT3D":	renderType = cure.Graphics.renderType.DIRECT3D
		else:
			renderType = cure.Graphics.renderType.OPENGL
			import sys
			print >>sys.stderr, "Warning: renderer context name \"gfx.%s\" incorrect." % (renderer,)
		# Make sure the mode is 4 pieces: width, height, BPP and Hz.
		mode = self.settings.gfx.mode
		print renderType
		print mode
		print self.settings.gfx.windowed
		self.screen = cure.Graphics.createScreen(renderType, mode, self.settings.gfx.windowed)

	def createContent(self):
		"""Creates some dummy content."""
		self.ground  = self.createGroupObject("simpleobjects.Ground", (), "ground_002", None, None)
		self.mainCam = self.createGroupObject("cameras.MainCamera", (self.screen,), None, (0, -25, 10,), (math.pi/2, math.pi*3/5, 0,))
		self.terrain = cure.createPythonObject("simpleobjects.Terrain", (3,), True)
		area = 25
		for x in range(20):
			x, y, z = random.uniform(-area, area), random.uniform(-area, area), random.uniform(5, area+5)
			theta, phi, gimbal = random.uniform(0, 2*math.pi), random.uniform(0, math.pi), random.uniform(0, 2*math.pi)
			self.createGroupObject("simpleobjects.JerkyObject", (),    "box_002", (x, y, z), (theta, phi, gimbal))
			x, y, z = random.uniform(-area, area), random.uniform(-area, area), random.uniform(5, area+5)
			theta, phi, gimbal = random.uniform(0, 2*math.pi), random.uniform(0, math.pi), random.uniform(0, 2*math.pi)
			self.createGroupObject("simpleobjects.JerkyObject", (), "sphere_002", (x, y, z), (theta, phi, gimbal))

	def createGroupObject(self, pythonName, constructorParameters, model, coordinate, orientation = None, scale = 1.0):
		"""Creates a replicated game object with the given properties."""
		# Create a game object and a context (python) object and attach them.
		gameObject = cure.createPythonObject(pythonName, constructorParameters, True)
		if model != None:
			# Load model and/or physics object from file. The file may contain
			# more than one object type, and references to other files, such
			# as textures, pixel shaders, filters, etc.
			gameObject.loadGroup(scale, model)
		if coordinate != None:
			# Set the pivot point.
			gameObject.setPosition(*coordinate)
		if orientation != None:
			gameObject.setOrientation(*orientation)
		return gameObject
