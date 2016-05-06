#!/usr/bin/env python3
# Recursively traverses all C++ files (including subdirs) and adds #include "pch.h" if it doesn't exist.
# Also creates file pch.h in all Source directories, except in ThirdParty.

inc = '''#include "_dir_/Include/Lepra.h"
#include "_dir_/Include/LepraAssert.h"
#include "_dir_/Include/Log.h"
'''

basedir = '.'

import os, re

def check_rewrite(_file):
	s = open(_file).read()
	if '#include "pch.h"' not in s:
		idx = s.find('#include')
		if idx >= 0:
			s = s[:idx] + '#include "pch.h"\n' + s[idx:]
			open(_file,'wt').write(s)
			print('Added pch.h to %s.' % _file)

for root,directory,files in os.walk(basedir, '*'):
	root = root.replace('\\', '/')
	if '/ThirdParty' in root:
		continue
	need_pch = False
	for _file in files:
		if _file.endswith('.cpp') or _file.endswith('.cxx'):
			need_pch = True
			check_rewrite(os.path.join(root, _file))
	if need_pch:
		root = os.path.abspath(root).replace('\\', '/')
		dircnt = len(root.split('/pd/',1)[1].split('/'))
		lepra_relative = '/'.join(['..' for x in range(dircnt)]) + '/Lepra'
		incs = inc.replace('_dir_', lepra_relative)
		f = open(os.path.join(root, 'pch.h'), 'wt')
		f.write(incs)
		f.close()
		print('Created/overwrote pch.h in %s' % root)
