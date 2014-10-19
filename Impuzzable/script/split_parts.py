#!/usr/bin/env python3

import copy
import asc

s = asc.load_shape('level')

ts = [copy.deepcopy(s)]
for t in ts:
	asc.save_shape(t)
	print('--------------')
