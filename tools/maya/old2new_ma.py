#!/usr/bin/env python3

import sys


for fn in sys.argv[1:]:
	inside = False
	lines = [line.rstrip('\n') for line in open(fn)]
	olines = []
	for line in lines:
		if len([1 for word in ['setAttr','"vectorArray"','".rgvtx"','".rgn"','".rguv'] if word in line]) == 3:
			inside = True
			startline,line = line.split('"vectorArray" ')
			nums = []
		if inside:
			nums += line.rstrip(';').split()
			if ';' not in line:
				continue
			nums = nums[1:]
			if '".rguv' in startline:	# Ignore 3D coord in UV
				nums = [n for i,n in enumerate(nums) if i%3!=2]
			line = startline + '"string" "[' + ', '.join(nums) + ']";'
			inside = False
		else:
			line = line.replace('"vectorArray"', '"string"')
		olines += [line]
	if lines != olines:
		w = open(fn, 'w')
		for line in olines:
			print(line, file=w)
