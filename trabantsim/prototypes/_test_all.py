#!/usr/bin/env python3

from glob import glob
from imp import load_module, PY_SOURCE
from os.path import abspath
from time import time
import trabant

# Replace the loop function.
simloop = trabant.loop
def loop_hook(*args, **kwargs):
	goon = simloop(*args, **kwargs)
	if time()-t > 4:
		return False
	return goon
trabant.loop = loop_hook

# Run through all files a couple of seconds each.
d = abspath('.')
for file in glob('*.py'):
	if 'test_all' in file:
		continue
	mod = file.replace('.py', '')
	print(mod)
	t = time()
	with open(d+'/'+file, 'rb') as fp:
		load_module(mod, fp, file, ('.py', 'rb', PY_SOURCE))
	assert trabant.gameapi.cmd('get-platform-name') in ('Win','Mac')
	trabant.release_objects()
	trabant.gameapi.reset()
	trabant.trabant_init()

print('Done.')
