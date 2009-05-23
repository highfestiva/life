#!/usr/bin/env python


from datetime import datetime
import os
import sys


cflags_1 = """
CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS %(includes)s -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED"""
cflags_2 = "-Wno-unknown-pragmas"

head_lib = cflags_1+" -Wall "+cflags_2+"\n"

head_lib_nowarn = cflags_1+" "+cflags_2+"\n"

foot_rules = """
depend:
\tmakedepend -- $(CFLAGS) -- $(SRCS)
.c.o:
\tgcc $(CFLAGS) -o $@ -c $<
.cpp.o:
\tg++ $(CFLAGS) -o $@ -c $<
"""

foot_lib = """
all:\tlib%(lib)s.so
clean:
\t@rm -f lib%(lib)s.so $(OBJS)
lib%(lib)s.so:\t$(OBJS)
\tg++ -shared -o $@ $(OBJS)
"""+foot_rules

head_bin = head_lib+"""
LIBS = -lLife -lCure -lTBC -lLepra -lThirdParty -lm -lpthread -lnsl -lncurses %(libs)s
"""

foot_bin = """
all:\t%(lib)s
clean:
\t@rm -f %(lib)s $(OBJS)
%(lib)s:\t$(OBJS)
\tg++ $(LIBS) -o $@ $(OBJS)
"""+foot_rules

maketext = \
{
"head_lib": head_lib,
"head_lib_nowarn": head_lib_nowarn,
"foot_lib": foot_lib,
"foot_lib_nowarn": foot_lib,
"head_bin": head_bin,
"foot_bin": foot_bin,
}

def isSrc(fname):
    ext = os.path.splitext(fname)[1]
    return (ext == ".cpp" or ext == ".c")

def generate_makefile(vcfile, makename, includedirs, libdirs, header, footer, type):
    libname = os.path.splitext(os.path.basename(vcfile))[0]
    libname = libname.split("_")[0]
    if libname.endswith("Lib"):
        libname = libname[:-3]

    projbasedir = os.path.dirname(vcfile)
    cpps = os.popen("./vcprojfiles.py -i "+vcfile+" -f Win32\\;win32 --rel-path --base-path="+projbasedir).read()
    cpps = cpps.split()
    cpps = filter(lambda x: isSrc(x), cpps)
    objs = map(lambda x: os.path.splitext(x)[0]+".o", cpps)
    f = open(makename, "wt")
    f.write("# 'Twas generated from %s, %s, type='%s'.\n" % (vcfile, datetime.now().isoformat()[:10], type))
    f.write("# Don't edit manually. See 'generate_makefile.py' for info.\n")
    includes = " ".join(["-I%s" % i for i in includedirs])
    libs = " ".join(["-L%s" % i for i in libdirs])
    f.write(header % {"includes":includes, "libs":libs})
    f.write("\nSRCS=\t\\\n")
    f.write("\t\\\n".join(cpps))
    f.write("\n\nOBJS=\t\\\n")
    f.write("\t\\\n".join(objs))
    f.write("\n")
    f.write(footer % {"lib":libname})
    f.close()

def generate_makefiles(basedir, vcfileinfolist):
    global maketext
    for type, vcfile in vcfileinfolist:
        vcfile = os.path.join(basedir, vcfile)
        projdir = os.path.dirname(vcfile)
        #bindir = os.path.relpath(os.path.join(projdir, "bin/"), projdir)
        includedirs = [os.path.relpath(basedir+"ThirdParty/stlport/stlport/", projdir),
        	os.path.relpath(basedir+"ThirdParty/ode-060223/include/", projdir)]
	libdirs = [os.path.relpath(basedir+"ThirdParty", projdir),
		os.path.relpath(basedir+"Lepra", projdir),
		os.path.relpath(basedir+"TBC", projdir),
		os.path.relpath(basedir+"Cure", projdir),
		os.path.relpath(basedir+"Life", projdir)]
        makename = os.path.join(os.path.dirname(vcfile), "makefile")
        sys.stdout.write(os.path.abspath(makename)+":\t")
        generate_makefile(vcfile, makename, includedirs, libdirs, maketext["head_"+type], maketext["foot_"+type], type)
        sys.stdout.write("done (type="+type+").\n")

def main():
    basedir = "../../"
    projects = [["lib_nowarn", "ThirdParty/ThirdPartyLib_800.vcproj"],
                ["lib", "Lepra/Lepra.vcproj"],
                ["lib", "TBC/TBC.vcproj"],
                ["lib", "Cure/Cure.vcproj"],
                ["lib", "Life/Life.vcproj"],
                ["bin", "Life/LifeServer/LifeServer.vcproj"]]
    generate_makefiles(basedir, projects)

if __name__ == '__main__':
    main()
