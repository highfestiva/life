#!/usr/bin/env python3

from sys import argv

def doproj(proj_name):
	data = open(proj_name.replace('14.','10.')).read()
	data = data.replace('10.vcxp','14.vcxp').replace(' include=',' Include=').replace('|win32','|Win32').replace('>win32','>Win32').replace('Clinclude','ClInclude')
	newlines,drop_tag = [],''
	for line in data.split('\n'):
		if not drop_tag and 'Unicode ' in line:
			drop_tag = '</' + line.strip().split()[0][1:]
		if drop_tag:
			if drop_tag in line:
				drop_tag = ''
			continue
		newlines += [line]
	open(proj_name,'w').write('\n'.join(newlines))

def dosln(sln_name):
	for line in open(sln_name):
		if line.startswith('Project("'):
			proj_name = line.split()[-2].strip('",')
			while proj_name:
				try:
					doproj(proj_name)
					print(proj_name)
					try:
						doproj(proj_name+'.filters')
					except:
						pass
					break
				except FileNotFoundError as e:
					proj_name = '\\'.join(proj_name.split('\\')[1:])
					if not proj_name:
						raise e

dosln(argv[1])
