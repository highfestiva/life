#!/usr/bin/env python3

from os import remove
from os.path import abspath, splitext, split as splitpath
from shutil import rmtree
from subprocess import call
from sys import argv, executable as pythonexe

py_filename = argv[1].replace('\\','/')
path,fname = splitpath(py_filename)
name,ext = splitext(fname)
is_console = (ext != '.pyw')

s = open('export_exe.spec','r').read()
s = s.format(py_filename=py_filename, name=name, is_console=is_console)
open('_export_exe.spec','w').write(s)
try:
	rmtree('dist')
except:
	pass
call(pythonexe +' ../python35/Scripts/pyinstaller-script.py _export_exe.spec')
remove('_export_exe.spec')
rmtree('build')
call('explorer '+abspath('dist'), shell=True)
print('EXE export done.')
