# Normal print (like printf).
print "Hello World (from Python)"

# Get access to the Cure interface
import cure

# Get some data (resolutions available) through the Cure C++ interface.
res = cure.Graphics.getFullscreenResolutions()
for mode in res:
	print mode[0], "x", mode[1], "x", mode[2], " @ ", mode[3], "Hz."
