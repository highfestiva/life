
# Author: Jonas Byström
# Copyright (c) 2002-2009, Righteous Games

from __future__ import with_statement
import optparse
import os
import sys
import rgohelp


#appnames = [""UiCure/CureTestApp"", "Life", "SlimeVolleyball", "KillCutie", "TireFire"]
appnames = ["Push", "PushServer", "PushMaster"]
osname = rgohelp._getosname()
hwname = rgohelp._gethwname()
datename = rgohelp._getdatename()

bindir = "bin"
buildtypes = ["debug", "rc", "final"]
default_build_mode = buildtypes[0]
ziptype = buildtypes[1]
own_tt = {"debug":"Unicode Debug", "rc":"Unicode Release Candidate", "final":"Unicode Final"}
updates = 0
removes = 0
importscript = "Tools/Maya/import_chunky.py"
makefilescriptdir = "Tools/GCC"
makefilescript = "generate_makefile.py"

showed_result = False


def _buildstl():
	if os.path.exists("ThirdParty/stlport/lib") or os.path.exists("ThirdParty/stlport/build/lib/obj/gcc/so_stlg"):
		return
	print("Building STLport...")
	os.chdir("ThirdParty/stlport/")
	if rgohelp._getvcver():
		rgohelp._run(["configure.bat", "msvc"+str(rgohelp._getvcver())], "configuring STLport for MSVC")
	else:
		rgohelp._run(["./configure"], "configuring STLport")
	os.chdir("build/lib")
	"cd build/lib"
	make = rgohelp._getmake(rgohelp.NMAKE)
	if rgohelp._getvcver():
		rgohelp._run([make, "/fmsvc.mak", "install"], "building STLport for MSVC")
	else:
		rgohelp._run([make, "-f", "gcc.mak", "depend"], "building STLport dependancies")
		rgohelp._run([make, "-f", "gcc.mak", "install"], "building STLport installation")
	os.chdir("../../../..")


def _buildcode(command, buildtype):
	make = rgohelp._getmake(rgohelp.VCBUILD)
	ver = rgohelp._getvcver()
	projext = "900" if ver == 9 else "10";
	if command == "build":
		_buildstl()
		if osname == "Windows":	args = [make, "/useenv", "/M2", "Life"+projext+".sln", own_tt[buildtype]+"|Win32"]
		else:			args = [make]
		what = "incremental building code"
	elif command == "rebuild":
		_buildstl()
		if osname == "Windows": args = [make, "/useenv", "/M2", "/rebuild", "Life"+projext+".sln", own_tt[buildtype]+"|Win32"]
		else:			args = [make, "clean", "all"]
		what = "rebuilding code"
	elif command == "clean":
		if osname == "Windows": args = [make, "/useenv", "/clean", "Life"+projext+".sln", own_tt[buildtype]+"|Win32"]
		else:			args = [make, "clean"]
		what = "cleaning code"
	if osname == "Windows":
		os.chdir("Life")
	rgohelp._run(args, what)
	if osname == "Windows":
		os.chdir("..")


def _convertdata(filename):
	rgohelp._run([sys.executable, '-OO', importscript, filename], "importing "+filename)


def _incremental_build_data(sourcedir):
	import glob
	mas = glob.glob(os.path.join(sourcedir, "Data/*.ma"))
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


def _incremental_copy(filelist, targetdir, buildtype):
	global updates
	import shutil
	for filename in filelist:
		global default_build_mode
		if buildtype != default_build_mode and filename.lower().find("test") >= 0:
			print("Skipping test binary named '%s'." % filename)
			continue
		if os.path.isdir(filename):
			continue
		if not os.path.exists(targetdir):
			os.makedirs(targetdir)
		targetfile = os.path.join(targetdir, os.path.split(filename)[1])
		if not os.path.exists(targetfile) or rgohelp._filetime(filename) > rgohelp._filetime(targetfile):
			if os.name == "nt":
				#print("Copying %s." % filename)
				shutil.copyfile(filename, targetfile)
			else:
				rgohelp._run(["cp", filename, targetfile], "copying of file")
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
				if obj.startswith("ThirdParty/"):
					fl += glob.glob(os.path.join(obj, lgpl_tt[buildtype], "*.dll"))
				elif list(filter(lambda x: x, [obj.startswith(an) for an in appnames])):
					fl += glob.glob(os.path.join(obj, own_tt[buildtype], "*.exe"))
	_incremental_copy(fl, targetdir, buildtype)


def _incremental_copy_data(sourcedir, targetdir, buildtype):
	import glob
	datadir = os.path.join(sourcedir, "Data")
	fl = glob.glob(datadir+"/*.class") + glob.glob(datadir+"/*.mesh") + glob.glob(datadir+"/*.phys") + \
		 glob.glob(datadir+"/*.jpg") + glob.glob(datadir+"/*.png") + glob.glob(datadir+"/*.tga") + glob.glob(datadir+"/*.bmp") + \
		 glob.glob(datadir+"/*.wav") + glob.glob(datadir+"/*.ogg") + glob.glob(datadir+"/*.mp3") + glob.glob(datadir+"/*.xm")
	targetdata = os.path.join(targetdir, "Data")
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
	removes += _cleandata(os.path.join(sourcedir, "Data"))
	removes += _cleandir(os.path.join(targetdir, "Data"))


def _cleandir(da_dir):
	global removes
	import glob
	fl = glob.glob(da_dir + "/*")
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


def _printresult():
	global showed_result, updates, removes
	if showed_result:
		return
	showed_result = True
	if updates+removes:	print("Operation successful, %i resulting files updated(/removed)." % (updates+removes))
	else:			print("Build up-to-date.")


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


def _create_zip(targetdir, buildtype):
	_posix_no_lib_exes(targetdir)
	print("Building compressed archive.")
	if os.name == "nt":
		targetfile = targetdir+".zip"
		if buildtype != "final":
			targetfile = targetdir+".iszip"
		rgohelp._zipdir(targetdir, lambda x: True, targetfile)
	else:
		targetfile = targetdir+".tar.gz"
		if buildtype != "final":
			targetfile = targetdir+".tar.isgz"
		rgohelp._targzdir(targetdir, targetfile)
	return targetfile


def _buildzip(builder, buildtype=ziptype):
	rgohelp._verify_base_dir()
	#print(appname, osname, hwname, buildtype, datename)
	#print(type(appname), type(osname), type(hwname), type(buildtype), type(datename))
	targetdir=appnames[0]+"."+osname+"."+hwname+"."+buildtype+"."+datename
	if buildtype == "rc":
		targetdir = "PRE_ALPHA."+targetdir
	elif buildtype != "final":
		targetdir = "NO_RELEASE."+targetdir
	os.mkdir(targetdir)
	builder(targetdir, buildtype)
	targetfile = _create_zip(targetdir, buildtype)
	_cleandir(targetdir)
	os.rmdir(targetdir)
	print("Built and zipped into %s." % targetfile)


def _copybin(targetdir, buildtype):
	import glob
	fl = glob.glob("bin/*")
	_incremental_copy(fl, targetdir, buildtype)
	fl = glob.glob("bin/Data/*")
	_incremental_copy(fl, os.path.join(targetdir, "Data"), buildtype)


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
		_cleandir(targetdir+"/Data")
	_cleandata_source(targetdir)
	_builddata(sourcedir+"/Data", targetdir+"/Data", buildtype)


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
	shutil.copytree("../Tools/build/macosx", exe+".app")
	for f in fs:
		os.rename(f, os.path.join(exe+".app/Contents/MacOS", f))
		updates += 1
	try:
		os.rename("Data", exe+".app/Contents/Resources/Data")
		updates += 1
	except:
		pass
	plist = ".app/Contents/Info.plist"
	r = open(exe+plist, "rt")
	w = open(exe+plist+".tmp", "wt")
	for line in r:
		w.write(line.replace("@EXE_NAME@", exe).replace("@BUNDLE_NAME@", name))
	r.close()
	w.close()
	os.remove(exe+plist)
	os.rename(exe+plist+".tmp", exe+plist)
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
		fl += glob.glob(os.path.join(app, "Contents/Resources/Data"))
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


#-------------------- High-level build stuff below. --------------------


def macappify_life_client():
	_macappify("LifeClient", "Da Client")

def macappify_slime():
	_macappify("SlimeVolleyball", "Slime Volleyball")

def macappify_kc():
	_macappify("KillCutie", "Kill Cutie")

def macappify_push():
	_macappify("Push", "Push")

def macappify_ht():
	_macappify("HoverTank", "HoverTank")

def macappify_hf():
	_macappify("HeliForce", "HeliForce")

def macappify_tf():
	_macappify("TireFire", "Tire Fire")

def demacappify():
	_demacappify("*.app")

def cleandata_life():
	_cleandata_source("Life")

def cleandata_slime():
	_cleandata_source("SlimeVolleyball")

def cleandata_kc():
	_cleandata_source("KillCutie")

def cleandata_push():
	_cleandata_source("Push")

def cleandata_ht():
	_cleandata_source("HoverTank")

def cleandata_hf():
	_cleandata_source("HeliForce")

def cleandata_tf():
	_cleandata_source("TireFire")

def builddata_life():
	_builddata("Life", bindir, default_build_mode)

def builddata_slime():
	_builddata("SlimeVolleyball", bindir, default_build_mode)

def builddata_kc():
	_builddata("KillCutie", bindir, default_build_mode)

def zipdata_kc():
	os.chdir('KillCutie/Data')
	rgohelp._zipdir('', _include_data_files, "Data.pk3")
	os.chdir('../../')

def builddata_push():
	_builddata("Push", bindir, default_build_mode)

def zipdata_push():
	os.chdir('Push/Data')
	rgohelp._zipdir('', _include_data_files, "Data.pk3")
	os.chdir('../../')

def builddata_ht():
	_builddata("HoverTank", bindir, default_build_mode)

def zipdata_ht():
	os.chdir('HoverTank/Data')
	rgohelp._zipdir('', _include_data_files, "Data.pk3")
	os.chdir('../../')

def builddata_hf():
	_builddata("HeliForce", bindir, default_build_mode)

def zipdata_hf():
	os.chdir('HeliForce/Data')
	rgohelp._zipdir('', _include_data_files, "Data.pk3")
	os.chdir('../../')

def builddata_tf():
	_builddata("TireFire", bindir, default_build_mode)

def zipdata_tf():
	os.chdir('TireFire/Data')
	rgohelp._zipdir('', _include_data_files, "Data.pk3")
	os.chdir('../../')

def builddata_b():
	_builddata("Bounce", bindir, default_build_mode)

def zipdata_b():
	os.chdir('Bounce/Data')
	rgohelp._zipdir('', _include_data_files, "Data.pk3")
	os.chdir('../../')


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


def build_life():
	rgohelp._verify_base_dir()
	buildcode()
	builddata_life()


def rebuild_life():
	_rebuild("Life", bindir, default_build_mode)


def zipdata_dirty():
	os.chdir(bindir+'/Data')
	rgohelp._zipdir('', _include_data_files, "Data.pk3")
	files = os.listdir('.')
	for f in files:
		if _include_data_files(f):
			os.remove(f)
	os.chdir('../../')


def clean():
	targetdir=bindir
	buildtype=default_build_mode
	rgohelp._verify_base_dir()
	global removes
	if rgohelp._hasdevenv(verbose=True):
		removes += _cleandir(targetdir)
		_buildcode("clean", buildtype)
	else:
		removes += _cleandir(targetdir+"/Data")
	_cleandata("Life/Data")
	_cleandata("Life/Data")
	_cleandata("Life/Data")


#def buildzip():
#	_buildzip(_rebuild)
#
#
#def builddirtyzip():
#	build()
#	_buildzip(_copybin, default_build_mode)


def _prepare_run():
	os.chdir(bindir)
	pre = "./"
	post = ""
	if os.name == "nt":
		pre = ""
		post = ".exe"
	if not os.path.exists(appnames[0]+post) or not os.path.exists(appnames[0]+post):
		reason = "binaries not compiled" if rgohelp._hasdevenv() else "missing C++ build environment"
		print("Could not run %s due to %s." % (appnames[0], reason))
		sys.exit(2)
	return pre, post


def _bgrun(name):
	_printresult()
	pre, post = _prepare_run()
	import subprocess
	subprocess.Popen(pre+name+post, shell=True)
	os.chdir("..")
def _fgrun(name, app=""):
	_printresult()
	pre, post = _prepare_run()
	os.system(app+pre+name+post)
	os.chdir("..")
#def startclient():
#	_fgrun("LifeClient")
#def bgclient():
#	_bgrun("LifeClient")
#def startserver():
#	_fgrun("LifeServer")
#def bgserver():
#	_bgrun("LifeServer")
#def start():
#	bgclient()
#	startserver()
def start():
	_fgrun("KillCutie")
#def gdbtest():
#	_fgrun("CureTestApp", "gdb ")
#def gdbclient():
#	_fgrun("LifeClient", "gdb ")


def _getmethods():
	methods = [(m, callable(eval(m))) for m in dir(sys.modules["__main__"])]
	methods,_ = zip(*filter(lambda x: x[1], methods))
	methods = list(filter(lambda n: not (n.startswith('_') or n.startswith('get')), methods))
	return methods


def _main():
	usage = "usage: %prog [options] <filespec>\n" + \
		"Runs some type of build command. Try build, rebuild, clean, builddata, or something like that."
	parser = optparse.OptionParser(usage=usage, version="%prog 0.2")
	parser.add_option("-m", "--buildmode", dest="buildmode", default="debug", help="Pick one of the build modes: %s. Default is debug." % ", ".join(buildtypes))
	ismac = (rgohelp._getosname() == "Mac")
	parser.add_option("-a", "--demacappify", dest="demacappify", default=ismac, help="Quietly try to de-Mac-.App'ify the target before building; default is %s." % str(ismac))
	options, args = parser.parse_args()

	if len(args) < 1:
		print("Need arg! Pick one of:\n  %s\n" % "\n  ".join(_getmethods()))
		sys.exit(1)
	if not options.buildmode in buildtypes:
		print("Unknown build mode!")
		sys.exit(1)
	global default_build_mode
	default_build_mode = options.buildmode
	if buildtypes.index(options.buildmode) > 0:
		global ziptype
		ziptype = options.buildmode

	if options.demacappify:
		demacappify()

	_checkplatform()

	for arg in args:
		try:
			exec(arg+"()")
		except NameError as e:
			print("Error: no such command %s!" % arg)
			print(e)
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
