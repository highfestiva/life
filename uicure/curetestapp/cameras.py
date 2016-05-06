"""Module contains camera stuff."""

import cure, math, time

class MainCamera(cure.ContextObject):
	"""Runs the main camera (FPS view) in Cure test 02."""
	def __init__(self, screen):
		cure.ContextObject.__init__(self)
		self.x = 0
		self.y = 0
		self.z = 0
		self.azimuth = 0
		self.zenith = 0
		self.gimbal = 0
                self.screen = screen


	def onCreate(self):
		"""Called upon creation by the game engine, but only after
		the game engine's context object has been created.
		The game engine's context object is required to be able to
		attach game data to us, such as alarms, physics and graphics."""
		self.enableTick()
		self.lastFpsUpdate = 0.0
		self.fpsInfo = ""
		self.createTime = time.time()


	def onTick(self, frameTime):
		# Render everything on this camera.
		self.screen.moveCamera((self.x, self.y, self.z), (self.azimuth, self.zenith, self.gimbal))
		self.screen.render()

		# Update microphone.
		cure.Sound.setMicrophonePosition(self.x, self.y, self.z)
		# TODO: add camera/microphone velocity (for doppler effects).
		#cure.Sound.SetMicrophoneVelocity(...)
		cure.Sound.setMicrophoneOrientation(self.azimuth, self.zenith, self.gimbal)

		# Print FPS on screen.
		now = time.time()
		if now-self.lastFpsUpdate > 0.5:
			self.fpsInfo = "FPS: "+str(int(1.0/frameTime))
			self.lastFpsUpdate = now
		screenSize = self.screen.getClippingRectangle()
		self.screen.printText(10, screenSize[3]-screenSize[0]-40, self.fpsInfo)

		# Update cam position.
		angle = (now-self.createTime)*0.1
		self.x = -25*math.sin(angle)
		self.y = -25*math.cos(angle)
		self.azimuth = math.pi/2-angle


	def setPosition(self, x, y, z):
		self.x = x
		self.y = y
		self.z = z


	def setOrientation(self, azimuth, zenith, gimbal):
		self.azimuth = azimuth
		self.zenith = zenith
		self.gimbal = gimbal
