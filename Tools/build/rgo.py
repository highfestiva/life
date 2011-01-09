
# Author: Jonas Byström
# Copyright (c) 2002-2009, Righteous Games

from __future__ import with_statement
import optparse
import os
import sys
from rgohelp import *


appname = "Life"
testappname = "UiCure/CureTestApp"
osname = getosname()
hwname = gethwname()
datename = getdatename()

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
        run([sys.executable, '-OO', importscript, filename], "importing "+filename)


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
                real_fs = []
                for f in fs:
                        fn = f.lower()
                        if fn.endswith(".class") or fn.endswith(".mesh") or fn.endswith(".phys"):
                                real_fs += [f]
                fs = real_fs
                if not fs:
                        print("Converting %s as no converted files exist!" % (basename,))
                        _convertdata(ma)
                for f in fs:
                        if filetime(f) < ft:
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
                if not os.path.exists(targetfile) or filetime(filename) > filetime(targetfile):
                        if os.name == "nt":
                                #print("Copying %s." % filename)
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
                                elif obj.startswith(appname+"/") or obj.startswith(testappname):
                                        fl += glob.glob(os.path.join(obj, own_tt[buildtype], "*.exe"))
                if hwname.find("64") >= 0:
                        fl += ["ThirdParty/fmod/api/fmod64.dll"]
                else:
                        fl += ["ThirdParty/fmod/api/fmod.dll"]
        _incremental_copy(fl, targetdir, buildtype)


def _incremental_copy_data(targetdir, buildtype):
        import glob
        fl = glob.glob("Data/*.class") + glob.glob("Data/*.mesh") + glob.glob("Data/*.phys") + \
                 glob.glob("Data/*.jpg") + glob.glob("Data/*.png") + glob.glob("Data/*.tga") + glob.glob("Data/*.bmp") + \
                 glob.glob("Data/*.wav") + glob.glob("Data/*.ogg") + glob.glob("Data/*.mp3")
        targetdata = os.path.join(targetdir, "Data")
        _incremental_copy(fl, targetdata, buildtype)


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
        global showed_result
        if showed_result:
                return
        showed_result = True
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


def _create_zip(targetdir, buildtype):
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
        return targetfile


def _buildzip(builder, buildtype=ziptype):
        verify_base_dir()
        #print(appname, osname, hwname, buildtype, datename)
        #print(type(appname), type(osname), type(hwname), type(buildtype), type(datename))
        targetdir=appname+"."+osname+"."+hwname+"."+buildtype+"."+datename
        if buildtype == "rc":
                targetdir = "PRE_RELEASE."+targetdir
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


def _macappify(exe, name):
        os.chdir("bin")
        import glob
        fl = glob.glob("*")
        fs = []
        for f in fl:
                if os.path.isfile(f):
                        fs += [f]
        for i in fs:
                for o in fs:
                        os.system("install_name_tool -change %s @executable_path/%s %s" % (o, o, i))
        import shutil
        shutil.copytree("../Tools/build/macosx", exe+".app")
        for f in fs:
                os.rename(f, os.path.join(exe+".app/Contents/MacOS", f))
        os.rename("Data", exe+".app/Contents/Resources/Data")
        plist = ".app/Contents/Info.plist"
        r = open(exe+plist, "rt")
        w = open(exe+plist+".tmp", "wt")
        for line in r:
                w.write(line.replace("@EXE_NAME@", exe).replace("@BUNDLE_NAME@", name))
        r.close()
        w.close()
        os.remove(exe+plist)
        os.rename(exe+plist+".tmp", exe+plist)
        os.chdir("..")


def _demacappify(wildcard):
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


#-------------------- High-level build stuff below. --------------------


def macappify_client():
        _macappify("LifeClient", "Da Client")

def demacappify():
        _demacappify("*.app")

def cleandata():
        targetdir=bindir
        global removes
        removes += _cleandata("Data")
        removes += _cleandir(os.path.join(targetdir, "Data"))


def builddata():
        targetdir=bindir
        buildtype=default_build_mode
        _incremental_build_data()
        _incremental_copy_data(targetdir, buildtype)



def buildcode():
        targetdir=bindir
        buildtype=default_build_mode
        if hasdevenv(verbose=True):
                _createmakes()
                _buildcode("build", buildtype)
                _incremental_copy_code(targetdir, buildtype)


def build():
        verify_base_dir()
        buildcode()
        builddata()


def rebuild():
        targetdir=bindir
        buildtype=default_build_mode
        verify_base_dir()
        if hasdevenv(verbose=True):
                _createmakes(force=True)
                _cleandir(targetdir)
                _buildcode("rebuild", buildtype)
                _incremental_copy_code(targetdir, buildtype)
        else:
                _cleandir(targetdir+"/Data")
        _cleandata("Data")
        builddata()


def clean():
        targetdir=bindir
        buildtype=default_build_mode
        verify_base_dir()
        global removes
        if hasdevenv(verbose=True):
                removes += _cleandir(targetdir)
                _buildcode("clean", buildtype)
        else:
                removes += _cleandir(targetdir+"/Data")
        _cleandata("Data")


def buildzip():
        _buildzip(rebuild)


def builddirtyzip():
        build()
        _buildzip(_copybin, default_build_mode)


def _prepare_run():
        os.chdir(bindir)
        pre = "./"
        post = ""
        if os.name == "nt":
                pre = ""
                post = ".exe"
        if not os.path.exists("LifeClient"+post) or not os.path.exists("LifeServer"+post):
                reason = "binaries not compiled" if hasdevenv() else "missing C++ build environment"
                print("Could not run %s due to %s." % (appname, reason))
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
def startclient():
	_fgrun("LifeClient")
def bgclient():
	_bgrun("LifeClient")
def startserver():
	fgrun("LifeServer")
def bgserver():
	bgrun("LifeServer")
def start():
        bgclient()
        startserver()
def gdbtest():
        _fgrun("CureTestApp", "gdb ")
def gdbclient():
        _fgrun("LifeClient", "gdb ")


if __name__ == "__main__":
        usage = "usage: %prog [options] <filespec>\n" + \
                "Runs some type of build command. Try build, rebuild, clean, builddata, or something like that."
        parser = optparse.OptionParser(usage=usage, version="%prog 0.2")
        parser.add_option("-m", "--buildmode", dest="buildmode", default="debug", help="Pick one of the build modes: %s. Default is debug." % ", ".join(buildtypes))
        ismac = (getosname() == "Mac")
        parser.add_option("-a", "--demacappify", dest="demacappify", default=ismac, help="Quietly try to de-Mac-.App'ify the target before building; default is %s." % str(ismac))
        options, args = parser.parse_args()

        if len(args) < 1:
                print("Need arg!")
                sys.exit(1)
        if not options.buildmode in buildtypes:
                print("Unknown build mode!")
                sys.exit(1)
        default_build_mode = options.buildmode
        if buildtypes.index(options.buildmode) > 0:
                ziptype = options.buildmode

        if options.demacappify:
                demacappify()

        for arg in args:
                exec(arg+"()")
        _printresult()
