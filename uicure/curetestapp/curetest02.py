""" Test module 2 for Cure.
Starts the 2nd test application."""

import cure

print __name__

if __name__ == "__main__":
	inst = cure.createPythonObject("curetestapp02.CureTestApplication", \
		("base.cfg",), False)
