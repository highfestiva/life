#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Author: Jonas Byström
# Copyright (c) 2002-2009, Righteous Games

from __future__ import with_statement
import optparse
import os
import sys
import rgohelp


#appnames = ["uicure/curetestapp", "life", "slimevolleyball", "killCutie", "tirefire"]
appnames = ["bound"]
fullname = "Bound"
osname = rgohelp._getosname()
hwname = rgohelp._gethwname()
datename = rgohelp._getdatename()
ismac = (osname == "Mac")

args = []
bindir = "bin"
buildtypes = ["debug", "rc", "final"]
default_build_mode = buildtypes[0]
ziptype = default_build_mode
builddir_ansi = {"debug":"Debug", "rc":"Release Candidate", "final":"Final"}
builddir_unicode = {"debug":"Unicode Debug", "rc":"Unicode Release Candidate", "final":"Unicode Final"}
builddir_types = {"ansi":builddir_ansi, "unicode":builddir_unicode}
own_tt = builddir_types["ansi"]
verbose = False
updates = 0
removes = 0
importscript = "tools/maya/import_chunky.py"
makefilescriptdir = "tools/gcc"
makefilescript = "generate_makefile.py"

showed_result = False

exclude_demacappify=['archive']


def _load_target_app():
	try:
		home = os.path.expanduser("~")
		with open(home+"/.rgoapp", "rt") as appfile:
			for line in appfile:
				words = eval(line)
				if len(words) >= 2:
					global appnames
					global fullname
					appnames = words[:-1]
					fullname = words[-1]
	except FileNotFoundError:
		print("Error: you have not yet set_target, do that first!")
		sys.exit(1)

def _save_target_app(words):
	if len(words) < 2:
		print("Error:   insczane method and app names!")
		print("Example: should be something like \"KillCutie 'Kill Cutie'\"")
		sys.exit(1)
	home = os.path.expanduser("~")
	with open(home+"/.rgoapp", "wt") as appfile:
		appfile.write(str(words) + "\n")
		global updates
		updates += 1
	_load_target_app()


def _buildext():
	pass	# Yey - STLport gone!


def _buildcode(command, buildtype):
	make = rgohelp._getmake(rgohelp.VCBUILD)
	ver = rgohelp._getvcver()
	projext = "900" if ver == 9 else "10";
	if command == "build":
		_buildext()
		if osname == "Windows":	args = [make, "/useenv", "/M4", "life"+projext+".sln", own_tt[buildtype]+"|Win32"]
		else:			args = [make]
		what = "incremental building code"
	elif command == "rebuild":
		_buildext()
		if osname == "Windows": args = [make, "/useenv", "/M4", "/rebuild", "life"+projext+".sln", own_tt[buildtype]+"|Win32"]
		else:			args = [make, "clean", "all"]
		what = "rebuilding code"
	elif command == "clean":
		if osname == "Windows": args = [make, "/useenv", "/clean", "life"+projext+".sln", own_tt[buildtype]+"|Win32"]
		else:			args = [make, "clean"]
		what = "cleaning code"
	print(args)
	args = rgohelp.fixbuildcmd(args)
	print(args)
	if osname == "Windows":
		os.chdir("life")
	rgohelp._run(args, what)
	if osname == "Windows":
		os.chdir("..")


def _convertdata(filename):
	printout = ['--verbose'] if verbose else []
	rgohelp._run([sys.executable, '-OO', importscript] + printout + [filename], "importing "+filename)


def _incremental_build_data(sourcedir):
	import glob
	mas = glob.glob(os.path.join(sourcedir, "data/*.ma"))
	for ma in mas:
		ft = rgohelp._filetime(ma)
		basename = os.path.splitext(ma)[0]
		ini = basename+".ini"
		if not os.path.exists(ini):
			print("Warning: file %s missing..." % ini)
			continue
		ftini = rgohelp._filetime(ini)
		if ftini > ft:
			ft = ftini
		fs = glob.glob(basename+"*")
		real_fs = []
		for f in fs:
			fn = f.lower()
			if fn.endswith(".class") or fn.endswith(".mesh") or fn.endswith(".phys"):
				real_fs += [f]
		fs = real_fs
		if not fs:
			#print("Converting %s as no converted files exist!" % (basename,))
			_convertdata(ma)
		for f in fs:
			if rgohelp._filetime(f) < ft:
				#print("Converting %s since %s has an older timestamp!" % (basename, f))
				for f in fs:
					os.remove(f)
				_convertdata(ma)
				break


def _incremental_copy(filelist, targetdir, buildtype, recursive=False):
	global updates
	import shutil
	for filename in filelist:
		global default_build_mode
		if buildtype != default_build_mode and filename.lower().find("test") >= 0:
			print("Skipping test binary named '%s'." % filename)
			continue
		if os.path.isdir(filename) and not recursive:
			continue
		if not os.path.exists(targetdir):
			os.makedirs(targetdir)
		targetfile = os.path.join(targetdir, os.path.split(filename)[1])
		if not os.path.exists(targetfile) or rgohelp._filetime(filename) > rgohelp._filetime(targetfile):
			print("Copying %s -> %s." % (filename, targetfile))
			if os.path.isdir(filename):
				shutil.copytree(filename, targetfile)
			else:
				shutil.copy2(filename, targetfile)
			updates += 1


def _incremental_copy_code(targetdir, buildtype):
	import glob
	if osname != "Windows":
		if targetdir == bindir:
			fl = [] # bin/ is all handled by make, don't do jack.
		else:
			fl = glob.glob("bin/*")
	else:
		lgpl_tt = {"debug":"Debug", "rc":"Release", "final":"Release"}
		# Gather binaries from makefile.
		fl = []
		with open("makefile", "rt") as rm:
			for line in rm:
				obj = line.strip()
				if obj:
					obj = obj.split()[0]
				if obj.startswith("thirdparty/"):
					fl += glob.glob(os.path.join(obj, lgpl_tt[buildtype], "*.dll"))
				elif list(filter(lambda x: x, [obj.startswith(an) for an in appnames])):
					fl += glob.glob(os.path.join(obj, own_tt[buildtype], "*.exe"))
	_incremental_copy(fl, targetdir, buildtype)


def _incremental_copy_data(sourcedir, targetdir, buildtype):
	import glob
	datadir = os.path.join(sourcedir, "data")
	fl = glob.glob(datadir+"/*.class") + glob.glob(datadir+"/*.mesh") + glob.glob(datadir+"/*.phys") + \
		 glob.glob(datadir+"/*.jpg") + glob.glob(datadir+"/*.png") + glob.glob(datadir+"/*.tga") + glob.glob(datadir+"/*.bmp") + \
		 glob.glob(datadir+"/*.wav") + glob.glob(datadir+"/*.ogg") + glob.glob(datadir+"/*.mp3") + glob.glob(datadir+"/*.xm")
	targetdata = os.path.join(targetdir, "data")
	_incremental_copy(fl, targetdata, buildtype)


def _cleandata(da_dir):
	global removes
	import glob
	fl = glob.glob(da_dir+"/*.class") + glob.glob(da_dir+"/*.mesh") + glob.glob(da_dir+"/*.phys")
	for filename in fl:
		os.remove(filename)
		removes += 1
	return removes


def _cleandata_source(sourcedir):
	targetdir=bindir
	global removes
	removes += _cleandata(os.path.join(sourcedir, "data"))
	removes += _cleandir(os.path.join(targetdir, "data"))


def _cleandir(da_dir):
	global removes
	import glob
	fl = glob.glob(da_dir + "/*") + glob.glob(da_dir + '/.*')
	for filename in fl:
		if os.path.isdir(filename):
			removes += _cleandir(filename)
			os.rmdir(filename)
			removes += 1
		else:
			if filename.startswith("/"):
				print("FATAL INTERNAL ERROR!")
				sys.exit(1)
			os.remove(filename)
			removes += 1
	return removes


def _checkplatform():
	if sys.platform == 'darwin':
		ios = os.environ.get('PD_BUILD_IOS')
		if ios == None:
			print("You must set env var PD_BUILD_IOS to 0 or 1 depending on your target. Exiting.")
			sys.exit(1)


def _printresult(print_no_work=True):
	global showed_result, updates, removes, fullname
	if showed_result:
		return
	showed_result = True
	if updates+removes:	print("%s operation successful, %i resulting files updated(/removed)." % (fullname, updates+removes))
	elif print_no_work:	print("%s build up-to-date." % fullname)


def _createmakes(force=False):
	if os.path.exists("makefile") and not force:
		return
	os.chdir(makefilescriptdir)
	r = [sys.executable, makefilescript]
	if default_build_mode != 'debug':
		r += ['--release']
	rgohelp._run(r, "generating makefiles")
	cnt = len(makefilescriptdir.split("/"))
	os.chdir("/".join([".."]*cnt))


def _posix_no_lib_exes(targetdir):
	if os.name == "nt":
		return
	# Only executables are executable... Hurm...
	import glob
	libs = glob.glob(os.path.join(targetdir, "lib*.so*"))
	for lib in libs:
		rgohelp._run(["chmod", "-x", lib], "changing .so +x status to -x")


def _create_zip(targetdir, targetfile, buildtype):
	_posix_no_lib_exes(targetdir)
	print("Building compressed archive.")
	if sys.platform in ("win32", "darwin"):
		targetfile += ".zip" if buildtype == "final" else ".iszip"
		rgohelp._zipdir(targetdir, lambda x: True, targetfile)
	else:
		targetfile += ".tar.gz" if buildtype == "final" else ".tar.isgz"
		rgohelp._targzdir(targetdir, targetfile)
	return targetfile


def _buildzip(builder, buildtype):
	rgohelp._verify_base_dir()
	bestname = fullname if osname != 'Linux' else fullname.replace(' ','')
	subdir = bestname if not ismac else '.'
	targetdir = bestname
	if buildtype == "rc":
		targetdir = "PRE_ALPHA."+targetdir
	elif buildtype != "final":
		targetdir = "NO_RELEASE."+targetdir
	targetdir = 'tmp/'+targetdir
	os.makedirs(targetdir)
	builder(targetdir, buildtype)
	for app in appnames:
		if bestname != app and os.path.exists(targetdir+'/'+app):
			os.rename(targetdir+'/'+app, targetdir+'/'+bestname)
	tmpdirs = ('tmp', '..') if not ismac else (targetdir, '../..')
	os.chdir(tmpdirs[0])
	targetfile = _create_zip(subdir, bestname, buildtype)
	os.chdir(tmpdirs[1])
	nicefile = bestname+"."+osname+"."+hwname+"."+buildtype+"."+datename+'.'+targetfile.split('.',1)[1]
	os.rename(tmpdirs[0]+'/'+targetfile, nicefile)
	_cleandir('tmp')
	os.rmdir('tmp')
	print("Built and zipped into %s." % nicefile)


def _copybin(targetdir, buildtype):
	import glob
	fl = glob.glob("bin/*")
	_incremental_copy(fl, targetdir, buildtype, True)
	fl = glob.glob("bin/data/*")
	_incremental_copy(fl, os.path.join(targetdir, "data"), buildtype)


def _builddata(sourcedir, targetdir, buildtype):
	_incremental_build_data(sourcedir)
	_incremental_copy_data(sourcedir, targetdir, buildtype)


def _rebuild(sourcedir, targetdir, buildtype):
	rgohelp._verify_base_dir()
	if rgohelp._hasdevenv(verbose=True):
		_createmakes(force=True)
		_cleandir(targetdir)
		_buildcode("rebuild", buildtype)
		_incremental_copy_code(targetdir, buildtype)
	else:
		_cleandir(targetdir+"/data")
	_cleandata_source(targetdir)
	_builddata(sourcedir+"/data", targetdir+"/data", buildtype)


def _macappify(exe, name):
	global updates
	os.chdir("bin")
	import glob
	fl = glob.glob("*")
	fs = []
	for f in fl:
		if os.path.isfile(f):
			fs += [f]
	for i in fs:
		for o in fs:
			#print("install_name_tool -change %s @executable_path/%s %s" % (o, o, i))
			os.system("install_name_tool -change %s @executable_path/%s %s" % (o, o, i))
	import shutil
	shutil.copytree("../tools/build/macosx", name+".app")
	for f in fs:
		os.rename(f, os.path.join(name+".app/Contents/MacOS", f))
		updates += 1
	try:
		os.rename("data", name+".app/Contents/Resources/data")
		updates += 1
		shutil.copy("../"+name+"/Icons/Main.icns", exe+".app/Contents/Resources")
		updates += 1
	except:
		pass
	plist = ".app/Contents/Info.plist"
	r = open(name+plist, "rt")
	w = open(name+plist+".tmp", "wt")
	for line in r:
		w.write(line.replace("@EXE_NAME@", exe).replace("@BUNDLE_NAME@", name))
	r.close()
	w.close()
	os.remove(name+plist)
	os.rename(name+plist+".tmp", name+plist)
	updates += 1
	os.chdir("..")


def _demacappify(wildcard):
	try:
		os.mkdir("bin")
	except:
		pass
	os.chdir("bin")
	import glob
	import shutil
	apps = glob.glob(wildcard)
	for app in apps:
		fl  = glob.glob(os.path.join(app, "Contents/MacOS/*"))
		fl += glob.glob(os.path.join(app, "Contents/Resources/data"))
		for f in fl:
			os.rename(f, os.path.split(f)[1])
		shutil.rmtree(app)
	os.chdir("..")


def _include_data_files(fn):
	fn = fn.lower()
	return fn.endswith(".class") or \
		fn.endswith(".mesh") or \
		fn.endswith(".phys") or \
		fn.endswith(".wav") or \
		fn.endswith(".png") or \
		fn.endswith(".jpg") or \
		fn.endswith(".tga") or \
		fn.endswith(".tif") or \
		fn.endswith(".tiff") or \
		fn.endswith(".bmp") or \
		fn.endswith(".xm")


def _prepare_run():
	os.chdir(bindir)
	pre = "./"
	post = ""
	if os.name == "nt":
		pre = ""
		post = ".exe"
	if not os.path.exists(appnames[0]+post):
		reason = ("binaries (%s) not compiled" % (bindir+'/'+appnames[0]+post)) if rgohelp._hasdevenv() else "missing C++ build environment"
		print("Could not run %s due to %s." % (appnames[0], reason))
		sys.exit(2)
	return pre, post
def _bgrun(name):
	_printresult(False)
	pre, post = _prepare_run()
	import subprocess
	subprocess.Popen(pre+name+post, shell=True)
	os.chdir("..")
def _fgrun(name, app=""):
	_printresult(False)
	pre, post = _prepare_run()
	app = [app] if app else []
	rgohelp._run(app+[pre+name+post], "run")
	os.chdir("..")


def _getmethods():
	methods = [(m, callable(eval(m))) for m in dir(sys.modules["__main__"])]
	methods,_ = zip(*filter(lambda x: x[1], methods))
	methods = list(filter(lambda n: not (n.startswith('_') or n.startswith('get')), methods))
	return methods

def _runlocal(script, args):
	if os.path.exists(appnames[0]+'/script/'+script+'.py'):
		os.chdir(appnames[0]+'/data')
		rgohelp._run([sys.executable, '-OO', '../script/'+script+'.py'], ' '.join([script]+args))
		os.chdir('../..')
		return True
	return False


#-------------------- High-level build stuff below. --------------------


def macappify():
	global appnames
	global fullname
	_macappify(appnames[0], fullname)

def demacappify():
	_demacappify("*.app")

def cleandata():
	global appnames
	_cleandata_source(appnames[0])

def builddata():
	global appnames
	_builddata(appnames[0], bindir, default_build_mode)

def zipdata():
	global appnames, updates
	datadir = appnames[0] + '/data'
	os.chdir(datadir)
	rgohelp._zipdir('', _include_data_files, "data.pk3")
	os.chdir('../..')
	updates += 1
	# Replace bin/data too.
	_cleandata_source(bindir)
	import glob
	fl = [datadir+"/data.pk3"] + glob.glob(datadir+"/*.ogg") + glob.glob(datadir+"/*.mp3")	# Music goes outside of the .zip.
	targetdata = os.path.join(bindir, "data")
	_incremental_copy(fl, targetdata, default_build_mode)

def buildcode():
	targetdir=bindir
	buildtype=default_build_mode
	if rgohelp._hasdevenv(verbose=True):
		_createmakes()
		_buildcode("build", buildtype)
		_incremental_copy_code(targetdir, buildtype)

def copycode():
	targetdir=bindir
	buildtype=default_build_mode
	_createmakes()
	_incremental_copy_code(targetdir, buildtype)

def clean():
	targetdir=bindir
	buildtype=default_build_mode
	rgohelp._verify_base_dir()
	cleandata()
	if rgohelp._hasdevenv(verbose=True):
		global removes
		removes += _cleandir(targetdir)
		_buildcode("clean", buildtype)

def run():
	copycode()
	_fgrun(appnames[0])

def go():
	builddata()
	copycode()
	run()

def archive():
	if default_build_mode != "final":
		print("Warning: archiving should probably be run on final (=release) artifacts.")
	_buildzip(_copybin, ziptype)
	sys.exit(0)

def set_target():
	global args
	appdata = args
	args = []
	_save_target_app(appdata)


def _main():
	usage = "usage: %prog [options] <filespec>\n" + \
		"Runs some type of build command. Try build, rebuild, clean, builddata, or something like that."
	parser = optparse.OptionParser(usage=usage, version="%prog 0.2")
	parser.add_option("-m", "--buildmode", dest="buildmode", default="debug", help="Pick one of the build modes: %s. Default is debug." % ", ".join(buildtypes))
	parser.add_option("-c", "--chartype", dest="chartype", default="ansi", help="Pick char type: ansi/unicode (i.e. char/wchar_t). Default is ansi.")
	parser.add_option("-a", "--demacappify", dest="demacappify", default=ismac, action="store_true", help="Quietly try to de-Mac-.App'ify the target before building; default is %s." % str(ismac))
	parser.add_option("-v", "--verbose", dest="verbose", default=False, action="store_true", help="Verbose mode; default is False.")
	parser.add_option("-e", "--pause-on-error", dest="pause_on_error", default=False, action="store_true", help="Pause on error; default is False.")
	global args
	options, args = parser.parse_args()

	if len(args) < 1:
		print("Need arg! Pick one of:\n  %s\n" % "\n  ".join(_getmethods()))
		sys.exit(1)
	if not options.buildmode in buildtypes:
		print("Unknown build mode!")
		sys.exit(1)
	global default_build_mode
	default_build_mode = options.buildmode
	global ziptype
	ziptype = default_build_mode
	global own_tt
	own_tt = builddir_types[options.chartype]
	global verbose
	verbose = options.verbose
	rgohelp.pause_on_error = options.pause_on_error

	if options.demacappify and not any(a in exclude_demacappify for a in args):
		demacappify()

	_checkplatform()

	if args[0] != "set_target":
		_load_target_app()

	while args:
		try:
			arg = args[0]
			args = args[1:]
			if not _runlocal(arg, args):
				exec(arg+"()")
		except NameError as e:
			print("Error: no such command %s!" % arg)
			suggestions = []
			import difflib
			for name in _getmethods():
				match = difflib.SequenceMatcher(None, arg, name).ratio()
				if match > 0.5:
					suggestions += [(match, name)]
			if suggestions:
				suggestions = sorted(suggestions, reverse=True)
				if suggestions[0][0] > 0.9:
					print("Did you mean %s?" % suggestions[0][1])
				else:
					suggestions = list(map(lambda mn: mn[1], suggestions))
					print("Perhaps: %s?" % ", ".join(suggestions))
			sys.exit(1)
	_printresult()


if __name__ == "__main__":
	_main()
