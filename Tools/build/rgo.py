
# Author: Jonas Byström
# Copyright (c) 2002-2009, Righteous Games

from __future__ import with_statement
import os
import sys
from rgohelp import *


appname = "Life"
osname = getosname()
hwname = gethwname()
datename = getdatename()

bindir = "bin"
buildtypes = ["debug", "rc", "final"]
defaulttype = buildtypes[0]
ziptype = buildtypes[1]
own_tt = {"debug":"Unicode Debug", "rc":"Unicode Release Candidate", "final":"Unicode Final"}
updates = 0
removes = 0
importscript = "Tools/Maya/import_chunky.py"
makefilescriptdir = "Tools/GCC"
makefilescript = "generate_makefile.py"


def _buildstl():
        if os.path.exists("ThirdParty/stlport/lib") or os.path.exists("ThirdParty/stlport/build/lib/obj/gcc/so"):
                return
        print("Building STLport...")
        os.chdir("ThirdParty/stlport/")
        if getvcver():
                run(["configure.bat", "msvc"+str(getvcver())], "configuring STLport for MSVC")
        else:
                run(["./configure"], "configuring STLport")
        os.chdir("build/lib")
        "cd build/lib"
        make = getmake(NMAKE)
        if getvcver():
                run([make, "/fmsvc.mak", "install"], "building STLport for MSVC")
        else:
                run([make, "-f", "gcc.mak", "depend"], "building STLport dependancies")
                run([make, "-f", "gcc.mak", "install"], "building STLport installation")
        os.chdir("../../../..")


def _buildcode(command, buildtype):
        make = getmake(VCBUILD)
        ver = getvcver()
        projext = "900" if ver == 9 else "";
        if command == "build":
                _buildstl()
                if osname == "Windows": args = [make, "/useenv", "/M2", "Life"+projext+".sln", own_tt[buildtype]+"|Win32"]
                else:                   args = [make]
                what = "incremental building code"
        elif command == "rebuild":
                _buildstl()
                if osname == "Windows": args = [make, "/useenv", "/M2", "/rebuild", "Life"+projext+".sln", own_tt[buildtype]+"|Win32"]
                else:                   args = [make, "clean", "all"]
                what = "rebuilding code"
        elif command == "clean":
                if osname == "Windows": args = [make, "/useenv", "/clean", "Life"+projext+".sln", own_tt[buildtype]+"|Win32"]
                else:                   args = [make, "clean"]
                what = "cleaning code"
        if osname == "Windows":
                os.chdir("Life")
        run(args, what)
        if osname == "Windows":
                os.chdir("..")


def _convertdata(filename):
        run([sys.executable, importscript, filename], "importing "+filename)


def _incremental_build_data():
        import glob
        mas = glob.glob("Data/*.ma")
        for ma in mas:
                ft = filetime(ma)
                basename = os.path.splitext(ma)[0]
                ini = basename+".ini"
                if not os.path.exists(ini):
                        print("Warning: file %s missing..." % ini)
                        continue
                ftini = filetime(ini)
                if ftini > ft:
                        ft = ftini
                fs = glob.glob(basename+"*")
                fs.remove(ma)
                fs.remove(ini)
                if not fs:
                        print("Converting %s as no converted files exist!" % (basename,))
                        _convertdata(ma)
                for f in fs:
                        if filetime(f) < ft:
                                print("Converting %s since %s has an older timestamp!" % (basename, f))
                                for f in fs:
                                     os.remove(f)
                                _convertdata(ma)
                                break


def _incremental_copy(filelist, targetdir):
        global updates
        import shutil
        for filename in filelist:
                if filename.lower().find("test") >= 0:
                        print("Skipping test binary named '%s'." % filename)
                        continue
                if os.path.isdir(filename):
                        continue
                if not os.path.exists(targetdir):
                        os.makedirs(targetdir)
                targetfile = os.path.join(targetdir, os.path.split(filename)[1])
                if not os.path.exists(targetfile) or filetime(filename) > filetime(targetfile):
                        if os.name == "nt":
                                shutil.copyfile(filename, targetfile)
                        else:
                                run(["cp", filename, targetfile], "copying of file")
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
                                elif obj.startswith(appname+"/"):
                                        fl += glob.glob(os.path.join(obj, own_tt[buildtype], "*.exe"))
                if hwname.find("64") >= 0:
                        fl += ["ThirdParty/fmod/api/fmod64.dll"]
                else:
                        fl += ["ThirdParty/fmod/api/fmod.dll"]
        _incremental_copy(fl, targetdir)


def _incremental_copy_data(targetdir):
        import glob
        fl = glob.glob("Data/*.class") + glob.glob("Data/*.mesh") + glob.glob("Data/*.phys") + \
                 glob.glob("Data/*.jpg") + glob.glob("Data/*.png") + glob.glob("Data/*.tga") + glob.glob("Data/*.bmp") + \
                 glob.glob("Data/*.wav") + glob.glob("Data/*.ogg") + glob.glob("Data/*.mp3")
        targetdata = os.path.join(targetdir, "Data")
        _incremental_copy(fl, targetdata)


def _cleandata(da_dir):
        removes = 0
        import glob
        fl = glob.glob(da_dir+"/*.class") + glob.glob(da_dir+"/*.mesh") + glob.glob(da_dir+"/*.phys")
        for filename in fl:
                os.remove(filename)
                removes += 1
        return removes


def _cleandir(da_dir):
        removes = 0
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


def _printresult():
        if updates+removes:     print("Operation successful, %i resulting files updated(/removed)." % (updates+removes))
        else:                   print("Already up-to-date.")


def _createmakes(force=False):
        if os.path.exists("makefile") and not force:
                return
        os.chdir(makefilescriptdir)
        run([sys.executable, makefilescript], "generating makefiles")
        cnt = len(makefilescriptdir.split("/"))
        os.chdir("/".join([".."]*cnt))


def _posix_no_lib_exes(targetdir):
        if os.name == "nt":
                return
        # Only executables are executable... Hurm...
        import glob
        libs = glob.glob(os.path.join(targetdir, "lib*.so*"))
        for lib in libs:
                run(["chmod", "-x", lib], "changing .so +x status to -x")


#-------------------- High-level build stuff below. --------------------


def cleandata(targetdir=bindir):
        global removes
        removes += _cleandata("Data")
        removes += _cleandir(os.path.join(targetdir, "Data"))


def builddata(targetdir=bindir):
        _incremental_build_data()
        _incremental_copy_data(targetdir)



def buildcode(targetdir=bindir, buildtype=defaulttype):
        if hasdevenv(verbose=True):
                _createmakes()
                _buildcode("build", buildtype)
                _incremental_copy_code(targetdir, buildtype)


def buildall(targetdir=bindir, buildtype=defaulttype):
        verify_base_dir()
        buildcode(bindir, defaulttype)
        builddata(targetdir)


def rebuildall(targetdir=bindir, buildtype=defaulttype):
        verify_base_dir()
        if hasdevenv(verbose=True):
                _createmakes(force=True)
                _cleandir(targetdir)
                _buildcode("rebuild", buildtype)
                _incremental_copy_code(targetdir, buildtype)
        else:
                _cleandir(targetdir+"/Data")
        _cleandata("Data")
        builddata(targetdir)


def cleanall(targetdir=bindir, buildtype=defaulttype):
        verify_base_dir()
        global removes
        if hasdevenv(verbose=True):
                removes += _cleandir(targetdir)
                _buildcode("clean", buildtype)
        else:
                removes += _cleandir(targetdir+"/Data")
        _cleandata("Data")


def buildzip():
        verify_base_dir()
        buildtype = ziptype
        #print(appname, osname, hwname, buildtype, datename)
        #print(type(appname), type(osname), type(hwname), type(buildtype), type(datename))
        targetdir=appname+"."+osname+"."+hwname+"."+buildtype+"."+datename
        if buildtype == "rc":
                targetdir = "PRE_RELEASE."+targetdir
        elif buildtype != "final":
                targetdir = "NO_RELEASE."+targetdir
        os.mkdir(targetdir)
        rebuildall(targetdir, buildtype)
        _posix_no_lib_exes(targetdir)
        print("Building compressed archive.")
        if os.name == "nt":
                targetfile = targetdir+".zip"
                if buildtype != "final":
                        targetfile = targetdir+".iszip"
                zipdir(targetdir, targetfile)
        else:
                targetfile = targetdir+".tar.gz"
                if buildtype != "final":
                        targetfile = targetdir+".tar.isgz"
                targzdir(targetdir, targetfile)
        _cleandir(targetdir)
        os.rmdir(targetdir)
        print("Built and zipped into %s." % targetfile)


def startnobuild():
        os.chdir(bindir)
        pre = "./"
        post = ""
        if os.name == "nt":
                pre = ""
                post = ".exe"
        if not os.path.exists("LifeClient"+post) or not os.path.exists("LifeServer"+post):
                reason = "internal error" if hasdevenv() else "missing C++ build environment"
                print("Could not build %s due to %s." % (appname, reason))
                sys.exit(2)
        import subprocess
        subprocess.Popen(pre+"LifeClient"+post, shell=True)
        os.system(pre+"LifeServer"+post)


def start():
        buildall()
        _printresult()
        startnobuild()


if __name__ == "__main__":
        if len(sys.argv) != 2:
                print("Need arg!")
                sys.exit(1)
        exec(sys.argv[1]+"()")
        _printresult()
