#!/usr/bin/env python


from datetime import datetime
import os
import sys


cflags_1 = """
CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS %(includes)s -DPOSIX -D_XOPEN_SOURCE=600 -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdSingle -DHAVE_CONFIG_H=1 -DLEPRA_WITHOUT_FMOD"""
cflags_2 = "-Wno-unknown-pragmas"

head_lib = cflags_1+" -Wall "+cflags_2+"\n"

head_lib_nowarn = cflags_1+" "+cflags_2+"\n"

foot_rules = """
depend:
\tmakedepend -- $(CFLAGS) -- $(SRCS)
\t#@grep -Ev "\\.o: .*([T]hirdParty|/[u]sr)/" makefile >.tmpmake
\t#@mv .tmpmake makefile
.c.o:
\tgcc $(CFLAGS) -o $@ -c $<
.cpp.o:
\tg++ $(CFLAGS) -o $@ -c $<

"""

foot_lib = """
all:\tlib%(lib)s.so $(OBJS)
clean:
\t@rm -f lib%(lib)s.so $(OBJS)
lib%(lib)s.so:\t$(OBJS)
\tg++ -shared -o $@ $(OBJS)
"""+foot_rules

foot_lib_nowarn = foot_lib

head_bin = head_lib+"""
LIBS = -lLife -lCure -lTBC -lLepra -lThirdParty -lstlport -lpthread -ldl -lrt %(libs)s
"""

head_gfx_bin = head_bin+"""
LIBS = -lLife -lUiCure -lUiTBC -lUiLepra -lOpenAL -lalut -lCure -lTBC -lLepra -lThirdParty -lstlport -lpthread -ldl -lrt -lGL %(libs)s
"""

foot_bin = """
all:\t%(lib)s $(OBJS)
clean:
\t@rm -f %(lib)s $(OBJS)
%(lib)s:\t$(OBJS)
\tg++ $(LIBS) -o $@ $(OBJS)
"""+foot_rules

foot_gfx_bin = foot_bin

foot_base = """

.PHONY:\t$(BINS) $(OBJS) $(SRCS) all clean depend

all:\t$(OBJS) $(SRCS) $(BINS)

clean:
\t@rm -f bin/*
\t@for SUBDIR in $(SRCS); do \\
\t\t$(MAKE) -C $$SUBDIR clean; \\
\tdone
\t@for SUBDIR in $(OBJS); do \\
\t\t$(MAKE) -C $$SUBDIR clean; \\
\tdone

depend:
\t@for SUBDIR in $(SRCS); do \\
\t\t$(MAKE) -C $$SUBDIR depend; \\
\tdone
\t@for SUBDIR in $(OBJS); do \\
\t\t$(MAKE) -C $$SUBDIR depend; \\
\tdone

$(BINS):\t$(OBJS)
\t@cp ThirdParty/stlport/build/lib/obj/gcc/so/libstlport.so.5.2 bin/
\t@cp $@ bin/

$(OBJS):\t$(SRCS)
\t$(MAKE) -C $@

$(SRCS):
\t$(MAKE) -C $@
\t@rm -f $(BINS)
\t@cp $@/*.so bin/
"""


def printstart(makename):
    sys.stdout.write(os.path.abspath(makename)+":\t")

def printend(type):
    sys.stdout.write("done (type="+type+").\n")

def issrc(fname):
    ext = os.path.splitext(fname)[1]
    return (ext == ".cpp" or ext == ".c")

def create_makefile(makename, srcname, type):
    f = open(makename, "wt")
    if srcname:
	srcname = "from "+srcname+", "
    f.write("# 'Twas generated %s%s, type='%s'.\n" % (srcname, datetime.now().isoformat()[:10], type))
    f.write("# Don't edit manually. See 'generate_makefile.py' for info.\n")
    return f

def write_contents(f, srcs, objs, bins=None):
    f.write("\nSRCS=\t\\\n")
    f.write("\t\\\n".join(srcs))
    f.write("\n\nOBJS=\t\\\n")
    f.write("\t\\\n".join(objs))
    if bins:
	f.write("\n\nBINS=\t\\\n")
	f.write("\t\\\n".join(bins))
    f.write("\n")

def convert_out_name(vcfile):
    libname = os.path.splitext(os.path.basename(vcfile))[0]
    libname = libname.split("_")[0]
    if libname.endswith("Lib"):
        libname = libname[:-3]
    return libname

def linux_bin_name(type, vcfile):
    rawname = convert_out_name(vcfile)
    if type == "lib":
    	rawname = "lib"+rawname+".so"
    pathname = os.path.join(os.path.dirname(vcfile), rawname)
    #print pathname
    return pathname

def generate_makefile(vcfile, makename, includedirs, libdirs, header, footer, type):
    libname = convert_out_name(vcfile)
    projbasedir = os.path.dirname(vcfile)
    cpps = os.popen("./vcprojfiles.py -i "+vcfile+" -f Win32\\;win32 --rel-path --base-path="+projbasedir).read()
    cpps = cpps.split()
    cpps = filter(lambda x: issrc(x), cpps)
    objs = map(lambda x: os.path.splitext(x)[0]+".o", cpps)
    f = create_makefile(makename, vcfile, type)
    includes = " ".join(["-I%s" % i for i in includedirs])
    libs = " ".join(["-L%s" % i for i in libdirs])
    f.write(header % {"includes":includes, "libs":libs})
    write_contents(f, cpps, objs)
    f.write(footer % {"lib":libname})
    f.close()

def generate_base_make(makename, binlist, liblist):
    f = create_makefile(makename, "", "base")
    bindirlist = [os.path.dirname(bin) for bin in binlist]
    libdirlist = [os.path.dirname(lib) for lib in liblist]
    write_contents(f, libdirlist, bindirlist, binlist)
    f.write(eval("foot_base"))
    f.close()


def generate_makefiles(basedir, vcfileinfolist):
    files = {"bin":[], "gfx_bin":[], "lib":[]}

    for type, vcfile in vcfileinfolist:
	basetype = "lib" if type.startswith("lib") else type
	files[basetype] += [linux_bin_name(basetype, vcfile)]

        vcfile = os.path.join(basedir, vcfile)
        projdir = os.path.dirname(vcfile)
        includedirs = [os.path.relpath(basedir+"ThirdParty/stlport/stlport/", projdir),
		os.path.relpath(basedir+"ThirdParty/utf8cpp", projdir),
		os.path.relpath(basedir+"ThirdParty/openal-soft-1.10.622/OpenAL32/Include/", projdir),
		os.path.relpath(basedir+"ThirdParty/openal-soft-1.10.622/include/", projdir),
		os.path.relpath(basedir+"ThirdParty/freealut-1.1.0/include/", projdir),
		os.path.relpath(basedir+"ThirdParty/ode-0.11.1/include", projdir),
		os.path.relpath(basedir+"ThirdParty/ode-0.11.1/ode/src", projdir),
		os.path.relpath(basedir+"ThirdParty/ode-0.11.1/ode/src/joints", projdir),
		os.path.relpath(basedir+"ThirdParty/ode-0.11.1/OPCODE", projdir),
		os.path.relpath(basedir+"ThirdParty/ode-0.11.1/GIMPACT/include", projdir),
		os.path.relpath(basedir+"ThirdParty/ode-0.11.1/ou/include", projdir)]
	libdirs = [os.path.relpath(basedir+"ThirdParty/stlport/build/lib/obj/gcc/so", projdir),
		os.path.relpath(basedir+"ThirdParty", projdir),
		os.path.relpath(basedir+"ThirdParty/openal-soft-1.10.622", projdir),
		os.path.relpath(basedir+"ThirdParty/freealut-1.1.0/admin/VisualStudioDotNET/alut", projdir),
		os.path.relpath(basedir+"Lepra", projdir),
		os.path.relpath(basedir+"TBC", projdir),
		os.path.relpath(basedir+"Cure", projdir),
		os.path.relpath(basedir+"UiLepra", projdir),
		os.path.relpath(basedir+"UiTBC", projdir),
		os.path.relpath(basedir+"UiCure", projdir),
		os.path.relpath(basedir+"Life", projdir)]
        makename = os.path.join(os.path.dirname(vcfile), "makefile")
	printstart(makename)
        generate_makefile(vcfile, makename, includedirs, libdirs, eval("head_"+type), eval("foot_"+type), type)
	printend(type)

    makename = os.path.join(basedir, "makefile")
    printstart(makename)
    generate_base_make(makename, files["bin"]+files["gfx_bin"], files["lib"])
    printend("base")


def main():
    basedir = "../../"
    projects = [["lib_nowarn", "ThirdParty/ThirdPartyLib_800.vcproj"],
		["lib_nowarn", "ThirdParty/openal-soft-1.10.622/OpenAL_800.vcproj"],
		["lib_nowarn", "ThirdParty/freealut-1.1.0/admin/VisualStudioDotNET/alut/alut.vcproj"],
                ["lib",        "Lepra/Lepra.vcproj"],
                ["lib",        "TBC/TBC.vcproj"],
                ["lib",        "Cure/Cure.vcproj"],
                ["lib",        "UiLepra/UiLepra.vcproj"],
                ["lib",        "UiTBC/UiTBC.vcproj"],
                ["lib",        "UiCure/UiCure.vcproj"],
                ["lib",        "Life/Life.vcproj"],
                ["bin",        "Life/LifeServer/LifeServer.vcproj"],
                ["gfx_bin",    "Life/LifeClient/LifeClient.vcproj"],
                ["gfx_bin",    "UiCure/CureTestApp/CureTestApp.vcproj"]]
    generate_makefiles(basedir, projects)

if __name__ == '__main__':
    main()
