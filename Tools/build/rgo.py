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
ziptype = "rc"
importscript = "Tools/Maya/import_chunky.py"
own_tt = {"debug":"Unicode Debug", "rc":"Unicode Release Client", "final":"Unicode Final"}
updates = 0
removes = 0


def buildstl():
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


def buildcode(command, buildtype):
        make = getmake(VCBUILD)
        if command == "build":
                buildstl()
                if osname == "Windows": args = [make, "/M2", "UiCure.sln", own_tt[buildtype]+"|Win32"]
                else:                   args = [make]
                what = "incremental building code"
        elif command == "rebuild":
                buildstl()
                if osname == "Windows": args = [make, "/M2", "/rebuild", "UiCure.sln", own_tt[buildtype]+"|Win32"]
                else:                   args = [make, "clean", "all"]
                what = "rebuilding code"
        elif command == "clean":
                if osname == "Windows": args = [make, "/clean", "UiCure.sln", own_tt[buildtype]+"|Win32"]
                else:                   args = [make, "clean"]
                what = "cleaning code"
        if osname == "Windows":
                os.chdir("UiCure")
        run(args, what)
        if osname == "Windows":
                os.chdir("..")


def convertdata(filename):
        python_exe = sys.executable
        run([python_exe, importscript, filename], "importing "+filename)


def incremental_build_data():
        import glob
        mas = glob.glob("Data/*.ma")
        for ma in mas:
                ft = filetime(ma)
                basename = os.path.splitext(ma)[0]
                ini = basename+".ini"
                ftini = filetime(ini)
                if ftini > ft:
                        ft = ftini
                fs = glob.glob(basename+"*")
                fs.remove(ma)
                fs.remove(ini)
                if not fs:
                        print("Converting %s as no converted files exist!" % (basename,))
                        convertdata(ma)
                for f in fs:
                        if filetime(f) < ft:
                                print("Converting %s since %s has an older timestamp!" % (basename, f))
                                for f in fs:
                                     os.remove(f)
                                convertdata(ma)
                                break


def incremental_copy(filelist, targetdir):
        global updates
        import shutil
        for filename in filelist:
                if filename.lower().find("test") >= 0:
                        print("Skipping test binary named '%s'.", filename)
                        continue
                if not os.path.exists(targetdir):
                        os.makedirs(targetdir)
                targetfile = os.path.join(targetdir, os.path.split(filename)[1])
                if not os.path.exists(targetfile) or filetime(filename) > filetime(targetfile):
                        shutil.copyfile(filename, targetfile)
                        updates += 1


def incremental_copy_code(target, buildtype):
        import glob
        if osname != "Windows":
                if target == bindir:
                        # This is all handled by make, don't do jack.
                        fl = []
                else:
                        fl = glob.glob("bin/*")
        else:
                lgpl_tt = {"debug":"Debug", "rc":"Release", "final":"Release"}
                # Gather binaries from makefile.
                fl = []
                with open(makefile, "rt") as rm:
                        for line in rm:
                                obj = line.strip().split()[0]
                                if obj.startswith("ThirdParty/"):
                                        fl += glob.glob(os.path.join(obj, lgpl_tt[buildtype], "*.dll"))
                                elif obj.startswith(appname+"/"):
                                        fl += glob.glob(os.path.join(obj, own_tt[buildtype], "*.exe"))
        incremental_copy(fl, target)


def incremental_copy_data(target):
        import glob
        fl = glob.glob("Data/*.class") + glob.glob("Data/*.mesh") + glob.glob("Data/*.phys")
        targetdata = os.path.join(target, "Data")
        incremental_copy(fl, targetdata)


def cleandata(da_dir):
        removes = 0
        import glob
        fl = glob.glob(da_dir+"/*.class") + glob.glob(da_dir+"/*.mesh") + glob.glob(da_dir+"/*.phys")
        for filename in fl:
                os.remove(filename)
                removes += 1
        return removes


def cleandir(da_dir):
        removes = 0
        import glob
        fl = glob.glob(da_dir + "/*")
        for filename in fl:
                if os.path.isdir(filename):
                        removes += cleandir(filename)
                        os.rmdir(filename)
                        removes += 1
                else:
                        if filename.startswith("/"):
                                print("FATAL INTERNAL ERROR!")
                                sys.exit(1)
                        os.remove(filename)
                        removes += 1
        return removes


#-------------------- High-level build stuff below. --------------------


def builddata(target=bindir):
        incremental_build_data()
        incremental_copy_data(target)



def buildall(target=bindir, buildtype=defaulttype):
        verify_base_dir()
        if hasdevenv(True):
                buildcode("build", buildtype)
                incremental_copy_code(target, buildtype)
        builddata(target)


def rebuildall(target=bindir, buildtype=defaulttype):
        verify_base_dir()
        if hasdevenv(True):
                cleandir(target)
                buildcode("rebuild", buildtype)
                incremental_copy_code(target, buildtype)
        else:
                cleandir(target+"/Data")
        cleandata("Data")
        builddata(target)


def cleanall(target=bindir, buildtype=defaulttype):
        verify_base_dir()
        global removes
        if hasdevenv(True):
                removes += cleandir(target)
                buildcode("clean", buildtype)
        else:
                removes += cleandir(target+"/Data")
        cleandata("Data")


def buildzip():
        verify_base_dir()
        buildtype = ziptype
        target=appname+"."+osname+"."+hwname+"."+buildtype+"."+datename
        if buildtype != "final":
                target = "NO_RELEASE."+target
        mkdir(target)
        cleandir(target)
        rebuildall(target, buildtype)
        targetfile = target+".zip"
        if buildtype != "final":
                targetfile = target+".iszip"
        zipdir(target, targetfile)
        rmdir(target)


def start():
        buildall()
        os.chdir(bindir)
        if not os.path.exists("LifeClient") or not os.path.exists("LifeServer"):
                reason = "internal error" if hasdevenv() else "missing C++ build environment"
                print("Could not build %s due to %s." % (appname, reason))
                sys.exit(2)
        import subprocess
        subprocess.Popen("./LifeClient", shell=True)
        os.execl("./LifeServer")


if __name__ == "__main__":
        if len(sys.argv) != 2:
                print("Need arg!")
                sys.exit(1)
        exec(sys.argv[1]+"()")
        if updates+removes:
                print("Operation successful, %i resulting files updated(/removed)." % (updates+removes))
        else:
                print("Already up-to-date.")
