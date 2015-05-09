#!/usr/bin/env python3

import os
import sys

# your xcode project directory
directory = sys.argv[1]

for root, _, files in os.walk(directory):
	for f in files:
		edited = False
		fullpath = os.path.join(root, f)
		with open(fullpath, 'r') as open_file:
			try:	contents = open_file.read()
			except:	continue
			if contents[:3] == "'''" or contents[:3] == '"""':
				edited = True
				contents = "#dummycomment\n" + contents
			if contents[:2] == '#!':
				edited = True
				contents = contents.splitlines(True)
				contents[0] = '#dummycomment\n'
				contents = ''.join(contents)
		if edited:
			with open(fullpath, 'w') as open_file:
				open_file.write(contents)

