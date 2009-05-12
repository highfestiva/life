#!/usr/bin/env python


from datetime import datetime
import os
import sys

head_lib = """
.SUFFIXES: .cpp
CFLAGS = -O0 -ggdb -c -D_POSIX_PTHREAD_SEMANTICS -IInclude -DPOSIX -D_DEBUG -D_CONSOLE -Wall
"""

head_lib_nowarn = """
.SUFFIXES: .cpp
CFLAGS = -O0 -ggdb -c -D_POSIX_PTHREAD_SEMANTICS -IInclude -DPOSIX -D_DEBUG -D_CONSOLE
"""

foot_lib = """
all:\t$(OBJS)
clean:
\t@rm -f $(OBJS)
depend:
\tmakedepend --$(CFLAGS) --$(SRCS)
.cpp.o:
\tg++ $(CFLAGS) -c -o $@ $<
"""

head_bin = """
LIBS = -lm -lpthread -lsocket -lnsl
CLDFLAGS = $(LIBS) -o $(CLIENT)
SLDFLAGS = $(LIBS) -o $(SERVER)
CLIENT= coma3d
SERVER= comaserver
"""

foot_bin = """Dunno!"""

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

def generate_makefile(vcfile, makename, header, footer, type):
    cpps = os.popen("./vcprojfiles.py -i "+vcfile+" -f Win32").read()
    cpps = cpps.split()
    cpps = filter(lambda x: isSrc(x), cpps)
    objs = map(lambda x: os.path.splitext(x)[0]+".o", cpps)
    f = open(makename, "wt")
    f.write("# 'Twas generated from %s, %s, type='%s'.\n" % (vcfile, datetime.now().isoformat()[:10], type))
    f.write("# Don't edit manually. See 'generate_makefile.py' for info.\n")
    f.write(header)
    f.write("\nSRCS=\t\\\n")
    f.write("\t\\\n".join(cpps))
    f.write("\n\nOBJS=\t\\\n")
    f.write("\t\\\n".join(objs))
    f.write("\n")
    f.write(footer)
    f.close()

def generate_makefiles(vcfileinfolist):
    global maketext
    for type, vcfile in vcfileinfolist:
        makename = os.path.join(os.path.dirname(vcfile), "makefile")
        sys.stdout.write(os.path.abspath(makename)+":\t")
        generate_makefile(vcfile, makename, maketext["head_"+type], maketext["foot_"+type], type)
        sys.stdout.write("done (type="+type+").\n")

def main():
    projects = [["lib_nowarn", "../ThirdParty/ThirdPartyLib_800.vcproj"],
                ["lib", "../Lepra/Lepra.vcproj"],
                ["lib", "../TBC/TBC.vcproj"],
                ["lib", "../Cure/Cure.vcproj"],
                ["lib", "../Life/Life.vcproj"],
                ["bin", "../Life/LifeServer/LifeServer.vcproj"]]
    generate_makefiles(projects)

if __name__ == '__main__':
    main()
