#!/usr/bin/env python

import random
import sys

def obfuscate(ph):
	new_ph = ""
	x = 0
	for c in ph:
		new_ph += chr(126-ord(c)+32)
		if (x&11) == 0:
			new_ph += chr(random.randint(32, 126))
		x += 1
	return new_ph

def deobfuscate(d):
	ph = ""
	x = 0
	while x < len(d):
		c = d[x]
		ph += chr(126-(ord(c)-32))
		if (x&11) == 0:
			d = d[:x] + d[x+1:]
		x += 1
	return ph

random.seed()
for ph in sys.argv[1:]:
	print('ph:')
	print('  "' + ph + '"')
	print('  "' + deobfuscate(obfuscate(ph)) + '"')
	print('  "' + obfuscate(ph) + '"')
	print()
