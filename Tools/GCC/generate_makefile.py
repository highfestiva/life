#!/usr/bin/env python


from datetime import datetime
import os
import sys


cflags = """
CFLAGS = -O0 -ggdb -fPIC -D_POSIX_PTHREAD_SEMANTICS %(includes)s -DPOSIX -D_DEBUG -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdCYLINDER_ENABLED -D_STLP_NO_OWN_NAMESPACE"""

head_lib = cflags+""" -Wall
"""

head_lib_nowarn = cflags+"""
"""

foot_lib = """
all:\tlib%(lib)s.so
clean:
\t@rm -f $(OBJS)
depend:
\tmakedepend --$(CFLAGS) --$(SRCS)
lib%(lib)s.so:\t$(OBJS)
\tg++ -o $(OBJS) $@
.cpp.o:
\tg++ $(CFLAGS) -o $@ -c $<
"""

head_bin = head_lib+"""
LIBS = -lm -lpthread -lsocket -lnsl
CLDFLAGS = $(LIBS) -o $(CLIENT)
SLDFLAGS = $(LIBS) -o $(SERVER)
CLIENT= coma3d
SERVER= comaserver
"""

foot_bin = foot_lib+"""
Dunno!
"""

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

def generate_makefile(vcfile, makename, includedirs, header, footer, type):
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
    f.write(header % {"includes":includes})
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
        includedir1 = os.path.relpath(basedir+"ThirdParty/stlport/stlport/", projdir)
        includedir2 = os.path.relpath(basedir+"ThirdParty/ode-060223/include/", projdir)
        makename = os.path.join(os.path.dirname(vcfile), "makefile")
        sys.stdout.write(os.path.abspath(makename)+":\t")
        generate_makefile(vcfile, makename, (includedir1, includedir2), maketext["head_"+type], maketext["foot_"+type], type)
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
