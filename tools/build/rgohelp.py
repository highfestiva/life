
# Author: Jonas Byström
# Copyright (c) 2002-2009, Righteous Games


import os
import sys


vcver = 10
NMAKE = "bin/nmake.exe"
VCBUILD = "vcpackages/vcbuild.exe"
NETBUILD = r'C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe'
pause_on_error = False


def _getosname():
	if sys.platform == "win32":		return "Windows"
	if sys.platform == "cygwin":		return "Cygwin"
	if sys.platform.startswith("linux"):	return "Linux"
	if sys.platform == "darwin":		return "Mac"
	if os.name == "nt":			return "Windows"
	if os.name == "posix":			return "Posix"
	if os.name == "mac":			return "Mac"
	return sys.platform


def _gethwname():
	if os.name == "nt":
		ccver = sys.version.split("[")[1][:-1]
		if ccver.find("32 bit") >= 0:
			return "x86"
		if ccver.find("64 bit") >= 0:
			return "x64"
	import subprocess
	machine = subprocess.Popen(["uname", "-m"], stdout=subprocess.PIPE).communicate()[0]
	machine = str(machine.strip())
	if machine.startswith("b'") and machine.endswith("'"):
		machine = machine[2:-1]
	return machine


def _getdatename():
	import datetime
	now = datetime.datetime.isoformat(datetime.datetime.now())
	return now.split("T")[0].replace("-", "")


def _filetime(filename):
	return os.stat(filename).st_mtime


def _verify_base_dir():
	if not os.path.exists(".git"):
		print("Must be in base dir to build (currently in %s)!" % os.path.abspath(os.path.curdir))
		raise BaseException("Shit hit the fan!")


def _sorted_natural(l):
	import re
	convert = lambda text: int(text) if text.isdigit() else text
	alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ]
	return sorted(l, key=alphanum_key)

def _getmake(builder):
	global vcver

	winprogs = os.getenv("PROGRAMFILES")
	if winprogs:
		if os.path.exists(winprogs+" Files"):
			winprogs += " Files"
		if os.path.exists(winprogs+" (x86)"):
			winprogs += " (x86)"
		dirname = os.getenv("VCINSTALLDIR")
		if not dirname:
			import glob
			names = _sorted_natural(glob.glob(os.path.join(winprogs, "Microsoft Visual*")))
			if len(names) == 0:
				print("Visual Studio not installed?")
				return None	# No Visual Studio installed.
			dirname = os.path.join(names[-1], "VC")
			if not os.path.exists(dirname):
				print("GOT HERE!", dirname)
				return None	# Visual Studio might be installed, but not VC++.
		make_exe = os.path.join(dirname, builder)
		if dirname.find("Studio 10.0") > 0:	vcver = 10
		elif dirname.find("Studio 9") > 0:	vcver = 9
		elif dirname.find("Studio 8") > 0:	vcver = 8
		else:
			print("Unknown MSVC version!")
			sys.exit(1)
		if vcver >= 10 and builder == VCBUILD:
			make_exe = NETBUILD
		if not os.path.exists(make_exe):
			print('Could not find %s.' % make_exe)
			make_exe = None
		#else:
		#	make_exe = '"'+make_exe+'"'
	elif os.path.exists("/bin/make"):
		make_exe = "/bin/make"
	elif os.path.exists("/usr/bin/make"):
		make_exe = "/usr/bin/make"
	else:
		make_exe = None
	#if not make_exe:
	#	print("Warning: no build environment detected!")
	return make_exe


def _getvcver():
	return vcver


def _hasdevenv(verbose=False):
	hasit = _getmake(NMAKE) and _getmake(VCBUILD)
	if verbose and not hasit:
		print("Warning: no C++ development environment detected.")
	return hasit

def fixbuildcmd(args):
	if vcver >= 10:
		vc10a = { '/useenv':'/p:useenv=true', '/rebuild':'/t:rebuild', '/clean':'/t:clean', '/M2':'/M:2', '/M4':'/M:4' }
		def r(a):
			try:	return vc10a[a]
			except: return a
		args,a = [],[r(a) for a in args]
		for b in a:
			if '|' in b:
				args += '/p:configuration={} /p:platform={}'.format(*b.split('|')).split()
			else:
				args += [b]
	return args

def _run(cmdlist, when):
	path = None
	if os.name == "nt":
		path = os.getenv("PATH")
		winpath = path.replace(":/c/", ";C:\\").replace("/", "\\")
		os.environ["PATH"] = winpath
		cmdlist[0] = cmdlist[0].replace("c:\\", "C:\\")
	import subprocess
	#print("Running %s..." % str(cmdlist))
	rc = subprocess.call(cmdlist)
	if path:
		os.environ["PATH"] = path
	if rc != 0:
		err = "Error %i when %s!" % (rc, when)
		input(err+"\nPress enter to continue.") if pause_on_error else print(err)
		sys.exit(1)


def _zipfiles(zf, include, filenames):
	import glob
	for filename in filenames:
		if not include(filename):
			continue
		if os.path.isdir(filename):
			fs = glob.glob(os.path.join(filename, "*"))
			_zipfiles(zf, include, fs)
		else:
			zf.write(filename)


def _zipdir(dirname, include, arcname):
	import glob
	import zipfile
	zf = zipfile.ZipFile(arcname, "w", zipfile.ZIP_DEFLATED)
	fs = glob.glob(os.path.join(dirname, "*"))
	def arc_no_inc(filename):
		if filename.endswith(arcname):
			return False
		return include(filename)
	_zipfiles(zf, arc_no_inc, fs)
	zf.close()

def _shzipdir(dirname, arcname):
	_run(["zip","-r9",arcname,dirname], "zipping archive")

def _targzdir(dirname, arcname):
	_run(["tar","--numeric-owner","--owner=0","--group=0","-cpf",arcname.replace('.gz',''),dirname], "taring archive")
	_run(["gzip","-9",arcname.replace('.gz','')], "gzipping archive")

def _7zdir(dirname, arcname):
	_run(["7z","a",arcname,dirname], "7zipping archive")
