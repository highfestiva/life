#!/usr/bin/env python


from datetime import datetime
import os
import sys


cextraflags = ""
if sys.platform == 'darwin':
    cextraflags = "-D_DARWIN_C_SOURCE -D_STLP_THREADS"

cflags_1 = """
CFLAGS = -O0 -ggdb -fPIC """+cextraflags+""" -D_STLP_DEBUG -D_POSIX_PTHREAD_SEMANTICS %(includes)s -DPOSIX -D_XOPEN_SOURCE=600 -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdSingle -DdTLS_ENABLED=1 -DHAVE_CONFIG_H=1 -DLEPRA_WITHOUT_FMOD"""
cflags_2 = "-Wno-unknown-pragmas"
ldflags = ""

librt = '-lrt'
libgl = '-lGL'
openal_noui = ''
openal_ui = '-lOpenAL'
stllibfile = '.so.5.2'

if sys.platform == 'darwin':
    librt = ''
    openal_noui = '-framework OpenAL'
    openal_ui = '-framework OpenAL'
    libgl = '-framework OpenGL'
    stllibfile = '.5.2.dylib'
    cflags_1 += ' -framework OpenGL -framework CoreServices -framework OpenAL -DMAC_OS_X_VERSION=1050'
    ldflags += ' -framework OpenGL -framework AppKit -framework Cocoa -lobjc -lstlportstlg -framework CoreServices %(libs)s %(deplibs)s '

head_lib = cflags_1+" -Wall "+cflags_2+"\n"

head_lib_nowarn = cflags_1+" "+cflags_2+"\n"

foot_rules = """
.SUFFIXES: .o .mm
depend:
\tmakedepend -- $(CFLAGS) -- $(SRCS)
\t#@grep -Ev "\\.o: .*([T]hirdParty|/[u]sr)/" makefile >.tmpmake
\t#@mv .tmpmake makefile
.c.o:
\tgcc $(CFLAGS) -o $@ -c $<
.cpp.o:
\tg++ $(CFLAGS) -o $@ -c $<
.mm.o:
\tg++ $(CFLAGS) -o $@ -c $<

"""

foot_lib = """
all:\tlib%(lib)s.so $(OBJS)
clean:
\t@rm -f lib%(lib)s.so $(OBJS)
lib%(lib)s.so:\t$(OBJS)
\tg++ -shared """ + ldflags + ' ' + openal_noui + """ -o $@ $(OBJS)
"""+foot_rules

foot_lib_nowarn = foot_lib

head_bin = head_lib+"""
LIBS = -lLife -lCure -lTBC -lLepra -lThirdParty -lstlportstlg -lpthread -ldl """ + librt + """ %(libs)s
"""

head_gfx_bin = head_bin+"""
LIBS = -lLife -lUiCure -lUiTBC -lUiLepra """ + openal_ui + """ -lalut -lCure -lTBC -lLepra -lThirdParty -lstlportstlg -lpthread -ldl """ + librt + " " + libgl + """ %(libs)s
"""

    

foot_bin = """
all:\t%(lib)s $(OBJS)
clean:
\t@rm -f %(lib)s $(OBJS)
%(lib)s:\t$(OBJS)
\tg++ $(LIBS) """ + ldflags + """ -o $@ $(OBJS)
"""+foot_rules

foot_gfx_bin = foot_bin

foot_base = """

.PHONY:\t$(BINS) $(OBJS) $(SRCS) all clean depend

all:\t$(OBJS) $(SRCS) $(BINS)

clean:
\t@rm -Rf bin/*
\t@mkdir bin/Data
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
\t@cp ThirdParty/stlport/build/lib/obj/gcc/so_stlg/libstlportstlg""" + stllibfile + """ bin/
\t@cp $@ bin/

$(OBJS):\t$(SRCS)
\t$(MAKE) -C $@

$(SRCS):
\t$(MAKE) -C $@
\t@rm -f $(BINS)
\t@mkdir -p bin
\t@cp $@/*.so bin/
"""


def printstart(makename):
    sys.stdout.write(os.path.abspath(makename)+":\t")

def printend(type):
    sys.stdout.write("done (type="+type+").\n")

def issrc(fname):
    ext = os.path.splitext(fname)[1]
    return (ext == ".cpp" or ext == ".c" or ext == ".mm")

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
    import re
    libname = re.sub("[0-9]*$", "", libname);
    return libname

def linux_bin_name(type, vcfile):
    rawname = convert_out_name(vcfile)
    if type == "lib":
        rawname = "lib"+rawname+".so"
    pathname = os.path.join(os.path.dirname(vcfile), rawname)
    #print pathname
    return pathname

def generate_makefile(vcfile, makename, includedirs, libdirs, deplibs, header, footer, type):
    libname = convert_out_name(vcfile)
    projbasedir = os.path.dirname(vcfile)
    extrafilter = ""
    if sys.platform == "darwin":
        extrafilter = "\\;X11\\;x11"
    else:
        extrafilter = "\\;Mac\\;mac"
    cpps = os.popen(sys.executable+" vcprojfiles.py -i "+vcfile+" -f Win32\\;win32"+extrafilter+" --rel-path --base-path="+projbasedir).read()
    #print("CPPs:\n"+cpps)
    cpps = cpps.split()
    cpps = [x for x in cpps if issrc(x)]
    objs = [os.path.splitext(x)[0]+".o" for x in cpps]
    f = create_makefile(makename, vcfile, type)
    includes = " ".join(["-I%s" % i for i in includedirs])
    libs = " ".join(["-L%s" % i for i in libdirs])
    f.write(header % {"includes":includes, "libs":libs})
    write_contents(f, cpps, objs)
    f.write(footer % {"lib":libname, "libs":libs, "deplibs":' '.join(deplibs)})
    f.close()

def generate_base_make(makename, binlist, liblist):
    f = create_makefile(makename, "", "base")
    bindirlist = [os.path.dirname(bin) for bin in binlist]
    libdirlist = [os.path.dirname(lib) for lib in liblist]
    write_contents(f, libdirlist, bindirlist, binlist)
    f.write(eval("foot_base"))
    f.close()


def get_dep_libs(vcfileinfolist, depnames):
    deps = []
    depnames = depnames.split()
    for depname in depnames:
        for name, type, vcfile, vcdeps in vcfileinfolist:
            if name == depname:
                deps += ["-l"+name]
                deps += get_dep_libs(vcfileinfolist, vcdeps)
    deps = list(set(deps))
    return deps

def generate_makefiles(basedir, vcfileinfolist):
    files = {"bin":[], "gfx_bin":[], "lib":[]}

    for name, type, vcfile, deps in vcfileinfolist:
        basetype = "lib" if type.startswith("lib") else type
        files[basetype] += [linux_bin_name(basetype, vcfile)]

        vcfile = os.path.join(basedir, vcfile)
        projdir = os.path.dirname(vcfile)
        includedirs = [os.path.relpath(basedir+"ThirdParty/stlport/stlport/", projdir),
        os.path.relpath(basedir+"ThirdParty/utf8cpp", projdir),
        os.path.relpath(basedir+"ThirdParty/freealut-1.1.0/include/", projdir),
        os.path.relpath(basedir+"ThirdParty/ode-0.11.1/include", projdir),
        os.path.relpath(basedir+"ThirdParty/ode-0.11.1/ode/src", projdir),
        os.path.relpath(basedir+"ThirdParty/ode-0.11.1/ode/src/joints", projdir),
        os.path.relpath(basedir+"ThirdParty/ode-0.11.1/OPCODE", projdir),
        os.path.relpath(basedir+"ThirdParty/ode-0.11.1/GIMPACT/include", projdir),
        os.path.relpath(basedir+"ThirdParty/ode-0.11.1/ou/include", projdir)]
        libdirs = [os.path.relpath(basedir+"ThirdParty/stlport/build/lib/obj/gcc/so_stlg", projdir),
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

        if sys.platform != 'darwin':
            includedirs = [os.path.relpath(basedir+"ThirdParty/openal-soft-1.10.622/OpenAL32/Include/", projdir),
                os.path.relpath(basedir+"ThirdParty/openal-soft-1.10.622/include/", projdir)]+includedirs

        makename = os.path.join(os.path.dirname(vcfile), "makefile")
        printstart(makename)
        deplibs = get_dep_libs(vcfileinfolist, deps)
        generate_makefile(vcfile, makename, includedirs, libdirs, deplibs, eval("head_"+type), eval("foot_"+type), type)
        printend(type)

    makename = os.path.join(basedir, "makefile")
    printstart(makename)
    generate_base_make(makename, files["bin"]+files["gfx_bin"], files["lib"])
    printend("base")


def main():
    basedir = "../../"
    projects = [
        ["ThirdParty", "lib_nowarn", "ThirdParty/ThirdPartyLib_900.vcproj", ""],
        ["alut",       "lib_nowarn", "ThirdParty/freealut-1.1.0/admin/VisualStudioDotNET/alut/alut900.vcproj", ""],
        ["Lepra",      "lib",        "Lepra/Lepra900.vcproj", "ThirdParty"],
        ["TBC",        "lib",        "TBC/TBC900.vcproj", "Lepra"],
        ["Cure",       "lib",        "Cure/Cure900.vcproj", "TBC"],
        ["UiLepra",    "lib",        "UiLepra/UiLepra900.vcproj", "Lepra alut"],
        ["UiTBC",      "lib",        "UiTBC/UiTBC900.vcproj", "UiLepra TBC"],
        ["UiCure",     "lib",        "UiCure/UiCure900.vcproj", "UiTBC Cure"],
        ["Life",       "lib",        "Life/Life900.vcproj", "Cure"],
        ["LifeServer", "bin",        "Life/LifeServer/LifeServer900.vcproj", "Life"],
        ["LifeClient", "gfx_bin",    "Life/LifeClient/LifeClient900.vcproj", "Life UiCure"],
        ["CureTest",   "gfx_bin",    "UiCure/CureTestApp/CureTestApp900.vcproj", "UiCure"]]

    if sys.platform != 'darwin':
        projects += [["OpenAL", "lib_nowarn", "ThirdParty/openal-soft-1.10.622/OpenAL_900.vcproj", ""]]
    generate_makefiles(basedir, projects)

if __name__ == '__main__':
    main()
