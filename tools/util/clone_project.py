#!/usr/bin/env python

import glob
import os
import shutil
import subprocess
import uuid

def renameFiles(tooldir, dadir, fromnames, toname):
	"Recursively replace file names and contents."
	tooldir = os.path.join('..', tooldir)
	os.chdir(dadir)
	for fromname in fromnames:
		fromspaced = "".join([x if x.islower() else ' '+x for x in fromname]).strip()
		tospaced   = "".join([x if x.islower() else ' '+x for x in toname]).strip()
		fromunder  = fromspaced.lower().replace(' ', '_')
		tounder    = tospaced.lower().replace(' ', '_')
		fromlo     = fromname.lower()
		tolo       = toname.lower()
		print("Renaming '%s' to '%s' in dir '%s'." % (fromname, toname, dadir))
		subprocess.call(['python', tooldir+'/renamefiles.py', '*'+fromname, '*'+toname])
		subprocess.call(['python', tooldir+'/renamefiles.py', fromname+'*', toname+'*'])
		subprocess.call(['python', tooldir+'/regexpfiles.py', fromname, toname, '*'])
		subprocess.call(['python', tooldir+'/regexpfiles.py', fromspaced, tospaced, '*'])
		subprocess.call(['python', tooldir+'/regexpfiles.py', fromunder, tounder, '*'])
		subprocess.call(['python', tooldir+'/regexpfiles.py', fromlo, tolo, '*'])
	files = glob.glob('*')
	for fn in files:
		if os.path.isdir(fn):
			renameFiles(tooldir, fn, fromnames, toname)
	os.chdir('..')

def add_makefile_generator(filename, fromname, toname):
	outfn = filename+".tmp"
	with open(filename, "rt") as r:
		with open(outfn, "wt") as w:
			for line in r:
				w.write(line)
				if fromname in line:
					toline = line.replace(fromname, toname)
					w.write(toline)
	os.remove(filename)
	os.rename(outfn, filename)

def add_builders(filename, fromname, toname):
	if len(fromname) <= 5:
		fromsuffix = fromname.lower()
	else:
		fromsuffix = "".join(filter(str.isupper, fromname)).lower()
	fromsuffix = '_'+fromsuffix+'():'
	if len(toname) <= 5:
		tosuffix = toname.lower()
	else:
		tosuffix   = "".join(filter(str.isupper, toname)).lower()
	tosuffix   = '_'+tosuffix+'():'
	fromspaced = "".join([x if x.islower() else ' '+x for x in fromname]).strip()
	tospaced   = "".join([x if x.islower() else ' '+x for x in toname]).strip()
	outfn = filename+".tmp"
	with open(filename, "rt") as r:
		with open(outfn, "wt") as w:
			inblock = False
			block = ""
			for line in r:
				wasinblock = inblock
				if line.startswith("def "):
					inblock = line.strip().endswith(fromsuffix)
				if inblock:
					block += line
				if wasinblock and not inblock:
					block = block.replace(fromname, toname)
					block = block.replace(fromspaced, tospaced)
					block = block.replace(fromsuffix, tosuffix)
					w.write(block)
					block = ""
				w.write(line)
	os.remove(filename)
	os.rename(outfn, filename)

def fix_guids(toname):
	for filename in glob.glob(toname+'/'+toname+'*.vc*proj*'):
		outfn = filename+".tmp"
		with open(filename, "rt") as r:
			with open(outfn, "wt") as w:
				prefixes = ('ProjectGUID="{', '<ProjectGuid>{', '<UniqueIdentifier>{')
				for line in r:
					for prefix in prefixes:
						if prefix in line:
							new_guid = str(uuid.uuid1()).upper()
							index1 = line.index(prefix) + len(prefix)
							index2 = index1 + len(new_guid)
							line = line[:index1] + new_guid + line[index2:]
							break
					w.write(line)
		os.remove(filename)
		os.rename(outfn, filename)

def clone_project(fromnames, toname):
	print("Copying files...")
	fromname = fromnames[0]
	pat = ('*.user', 'makefile', '*.mesh', '*.class', '*.phys', 'Unicode Debug', 'Unicode Release Candiate', 'Unicode Final', 'Final', 'Debug', 'Release')
	shutil.copytree(fromname, toname, ignore=shutil.ignore_patterns(*pat))
	todir = toname
	renameFiles('Tools/Util', todir, fromnames, toname)
	print("Files and contents renamed successfully.")
	add_makefile_generator('Tools/GCC/generate_makefile.py', fromname, toname)
	print("Makefile generation added successfully.")
	add_builders('Tools/Build/rgo.py', fromname, toname)
	print("Builders added successfully.")
	fix_guids(toname)
	print("GUIDs changed.")
	print(fromname, "->", toname, "done!")

if __name__ == "__main__":
	import sys
	if len(sys.argv) < 3:
		print("Usage:   %s <fromname> [fromname...]  <toname>" % sys.argv[0])
		print("Example: %s KillCutie GrenadeRun TireFire" % sys.argv[0])
		sys.exit(1)
	clone_project(sys.argv[1:-1], sys.argv[-1])
