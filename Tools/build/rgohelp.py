
# Author: Jonas Byström
# Copyright (c) 2002-2009, Righteous Games


import os
import sys


vcver = None
NMAKE = "bin/nmake.exe"
VCBUILD = "vcpackages/vcbuild.exe"


def getosname():
        if sys.platform == "win32":             return "Windows"
        if sys.platform == "cygwin":            return "Cygwin"
        if sys.platform.startswith("linux"):    return "Linux"
        if sys.platform == "darwin":            return "Mac"
        if os.name == "nt":                     return "Windows"
        if os.name == "posix":                  return "Posix"
        if os.name == "mac":                    return "Mac"
        return sys.platform


def gethwname():
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


def getdatename():
        import datetime
        now = datetime.datetime.isoformat(datetime.datetime.now())
        return now.split("T")[0].replace("-", "")


def filetime(filename):
        return os.stat(filename).st_mtime


def verify_base_dir():
        if not os.path.exists(".git") or not os.path.exists("Data"):
                print("Must be in base dir to build!")
                sys.exit(1)


def getmake(builder):
        global vcver

        winprogs = os.getenv("PROGRAMFILES")
        if winprogs:
                dirname = os.getenv("VCINSTALLDIR")
                if not dirname:
                        import glob
                        names = sorted(glob.glob(os.path.join(winprogs, "Microsoft Visual*")))
                        if len(names) == 0:
                                return None     # No Visual Studio installed.
                        dirname = os.path.join(names[-1], "VC")
                        if not os.path.exists(dirname):
                                return None     # Visual Studio might be installed, but not VC++.
                make_exe = os.path.join(dirname, builder)
                if dirname.find("8") > 0:     vcver = 8
                elif dirname.find("9") > 0:   vcver = 9
                else:
                        print("Unknown MSVC version!")
                        sys.exit(1)
        elif os.path.exists("/bin/make"):
                make_exe = "/bin/make"
        elif os.path.exists("/usr/bin/make"):
                make_exe = "/usr/bin/make"
        if not os.path.exists(make_exe):
                make_exe = None
        #if not make_exe:
        #        print("Warning: no build environment detected!")
        return make_exe


def getvcver():
        return vcver


def hasdevenv(verbose=False):
        hasit = getmake(NMAKE) and getmake(VCBUILD)
        if verbose and not hasit:
                print("Warning: no C++ development environment detected.")
        return hasit


def run(cmdlist, when):
        path = None
        if os.name == "nt":
                path = os.getenv("PATH")
                winpath = path.replace(":/c/", ";C:\\").replace("/", "\\")
                os.environ["PATH"] = winpath
        import subprocess
        #print("Running %s..." % str(cmdlist))
        rc = subprocess.call(cmdlist)
        if path:
                os.environ["PATH"] = path
        if rc != 0:
                print("Error %i when %s!" % (rc, when))
                sys.exit(1)


def _zipfiles(zf, filenames):
        import glob
        for filename in filenames:
                if os.path.isdir(filename):
                        fs = glob.glob(os.path.join(filename, "*"))
                        _zipfiles(zf, fs)
                else:
                        zf.write(filename)


def zipdir(dirname, arcname):
        import glob
        import zipfile
        zf = zipfile.ZipFile(arcname, "w", zipfile.ZIP_DEFLATED)
        fs = glob.glob(os.path.join(dirname, "*"))
        _zipfiles(zf, fs)
        zf.close()


def targzdir(dirname, arcname):
	run(["tar","-czf",arcname,dirname], "taring archive")
