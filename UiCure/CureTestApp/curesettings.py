"""Settings module that handles setting default, loading from, and saving
to, files.

The settings and their names are hardcoded in this file. See each class'
setDefault() method for their respective names."""



from __future__ import with_statement



def convertValue(value, settingsFilename, lineNumber):
	try:
		tmpVal = None
		firstChar = value[0:1]
		if firstChar == "[":
			values = filter(None, value[1:-1].replace(" ", "").split(","))
			tmpVal = []
			for v in values:
				tmpVal.append(convertValue(v, settingsFilename, lineNumber))
		elif firstChar == "(":
			values = filter(None, value[1:-1].replace(" ", "").split(","))
			tmpVal = []
			for v in values:
				tmpVal.append(convertValue(v, settingsFilename, lineNumber))
			tmpVal = tuple(tmpVal)
		elif firstChar == "\"" or firstChar == "'":
			tmpVal = value[1:-1]
		elif value == "True":
			tmpVal = True
		elif value == "False":
			tmpVal = False
		elif "." in value:
			tmpVal = float(value)
		elif value[0:1] in "-0123456789":
			tmpVal = int(value)
		else:
			# We'll assume string, but the type stinks.
			tmpVal = value
			import sys
			print >>sys.stderr, "Settings warning ("+settingsFilename+":"+str(lineNumber)+"): unknown type, using str!"
	except ValueError, e:
		import sys
		print >>sys.stderr, "Settings warning ("+settingsFilename+":"+str(lineNumber)+"):", e
	return tmpVal

def saveObject(fileHandle, object, prefix):
	"""Handles streaming object-oriented Python assignments
	(such as setting of default values) to file."""
	objectList = dir(object)
	for subObjectName in objectList:
		if subObjectName[0:2] == "__":
			continue
		subObject = getattr(object, subObjectName)
		t = str(type(subObject))[7:-2]
		if t == "instance":
			saveObject(fileHandle, subObject, prefix+subObjectName+".")
		elif t == "int" or t == "float" or t == "bool" or t == "list" or t == "tuple" or t == "dict":
			line = prefix+subObjectName+" = "+str(subObject)+"\n"
			fileHandle.write(line)
		elif t == "str" :
			line = prefix+subObjectName+" = '"+subObject+"'\n"
			fileHandle.write(line)
	fileHandle.write("\n")

class Settings:
	"""Class contains subclasses for each setting type, and also has
	load() and save() methods."""

	class Normalizer:
		"""Fixes some erronous settings."""
		def normalize(self):
			import copy, sys
			default = copy.copy(self)
			default.setDefault()
			defaultAttributeNames = dir(default)
			# TODO: extend to check type of every element
			# (in for instance a list) against default value.
			for defAttrName in defaultAttributeNames:
				# Check each member (on the defaultsetting) for wrong type.
				defAttr = getattr(default, defAttrName)
				defAttrType = str(type(defAttr))[7:-2]
				attr = getattr(self, defAttrName)
				attrType = str(type(attr))[7:-2]
				# Type correct?
				if defAttrType != attrType:
					print >>sys.stderr, "Settings warning: \"%s\" was of type %s, but should be of type %s." % \
						(defAttrName, attrType, defAttrType,)
					setattr(self, defAttrName, defAttr)
				else:
					# Type is the same, check if list/tuple lengths match.
					if defAttrType == "tuple":
						if len(attr) < len(defAttr):
							# Fill in missing with default data.
							print >>sys.stderr, "Settings warning: extending tuple \"%s\" from %i to %i elements." % \
								(defAttrName, len(attr), len(defAttr),)
							tmpCopy = list(attr)+list(defAttr[len(attr):])
							setattr(self, defAttrName, tuple(tmpCopy))
						elif len(attr) > len(defAttr):
							# Cutoff too long tuple.
							print >>sys.stderr, "Settings warning: cutting off tuple \"%s\" from %i to %i elements." % \
								(defAttrName, len(attr), len(defAttr),)
							setattr(self, defAttrName, attr[0:len(defAttr)])

	class Gfx(Normalizer):
		"""Contains gfx settings."""
		def __init__(self):
			self.setDefault()

		def setDefault(self):
			self.windowed = True
			self.mode = (800, 600, -1, -1)
			self.renderer = "OpenGL"

	class Sound(Normalizer):
		"""Contains sound settings."""
		def __init__(self):
			self.setDefault()

		def setDefault(self):
			self.soundVolume = 1.0
			self.musicVolume = 0.8

	class Control(Normalizer):
		"""Contains user control settings (keyboard, mouse...)."""
		def __init__(self):
			self.setDefault()

		def setDefault(self):
			self.primaryAction = ["Mouse1", "Joy1", "Control",]
			self.secondaryAction = ["Mouse2", "Joy2", "V",]
			self.forward = ["JoyUp", "E",]
			self.backward = ["JoyDown", "D",]
			self.strafeRight = ["F",]
			self.strafeLeft = ["S",]
			self.steerRight = ["F", "JoyRight",]
			self.steerLeft = ["S", "JoyLeft",]
			self.jump = ["Space", "Mouse3",]

	class Profile(Normalizer):
		"""Contains user data (username, avatar traits...)."""
		def __init__(self):
			self.setDefault()

		def setDefault(self):
			self.userName = "Username"
			import os
			try:
				self.userName = os.environ["USERNAME"]      # Windows has uses the USERNAME=... environment variable.
			except KeyError:
				pass
			try:
				self.userName = os.environ["USER"]      # Windows has uses the USERNAME=... environment variable.
			except KeyError:
				pass
			self.avatarName = self.userName
			self.miniGame = "MisileCruiser"
			self.screenSaver = "WhenTheBossWandersBy"

		def getUserName(self):
			return self.userName

	def __init__(self, settingsFilename = None):
		"""1. sets default settings; 2. Loads settings given by filename parameter."""
		self.setDefault()
		if settingsFilename != None:
			self.load(settingsFilename)

	def setDefault(self):
		"""Sets default settings."""
		self.gfx = self.Gfx()
		self.sound = self.Sound()
		self.control = self.Control()
		self.profile = self.Profile()

	def normalize(self):
		self.gfx.normalize()
		self.sound.normalize()
		self.control.normalize()
		self.profile.normalize()

	def load(self, settingsFilename):
		"""Loads a settings file, which may only contain Python comments or assignments such as
		gfx.mode = [1024, 768, 32, 0]
		or
		sound.masterVolume = 0.8"""

		try:
			settingsFile = open(settingsFilename, "rt")
			lineNumber = 1
			for line in settingsFile:
				line = line.replace(" ", "")
				line = line.replace("\r", "")
				line = line.replace("\n", "")
				if line[0:1] == "#":	# Skip comments.
					continue
				try:
					destination, value = line.split("=")
				except ValueError:      # Not an assignment, since it doesn't contain "=".
					continue
				if destination == "" or value == "":	# Skip non-assignments.
					continue
				# Convert value to correct type.
				value = convertValue(value, settingsFilename, lineNumber)
				# Convert destination to actual sub-object and assign.
				subObjectList = destination.split(".")	# Handle sub-object assignment.
				targetObject = self
				try:
					for objectName in subObjectList[0:-1]:
						targetObject = getattr(targetObject, objectName)
					getattr(targetObject, subObjectList[-1])
					setattr(targetObject, subObjectList[-1], value)
				except AttributeError, e:
					import sys
					print >>sys.stderr, "Settings warning ("+settingsFilename+":"+str(lineNumber)+"):", e
				lineNumber += 1
			self.normalize()
		except IOError:
			pass

	def save(self, settingsFilename):
		"""Saves the settings to a file. No comments are stored."""
		with open(settingsFilename, "wt") as settingsFile:
			saveObject(settingsFile, self, "")



if __name__ == "__main__":
	settings = Settings("Load.cfg")
	del settings
