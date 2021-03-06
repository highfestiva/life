#!/usr/bin/env python


from datetime import datetime
import os
import sys


is_debug = True
if len(sys.argv) > 1 and sys.argv[1] == '--release':
    is_debug = False


is_ios = os.environ.get('PD_BUILD_IOS')
if is_ios: is_ios = int(is_ios)
is_ios_sim = os.environ.get('PD_BUILD_IOS_SIM')
if is_ios_sim: is_ios_sim = int(is_ios_sim)
is_mac = (sys.platform == 'darwin')

#print("iOS: %s, iOS sim: %s, mac: %s" % (is_ios, is_ios_sim, is_mac))

cextraflags = ''
cppflags = ''
glframework = 'OpenGL'
gcc = 'gcc'
gpp = 'g++'
if is_mac:
    cextraflags = ' -D_DARWIN_C_SOURCE -DUSE_FILE32API'
    if is_ios:
        darwin_kit = '-framework UIKit -framework Foundation -framework QuartzCore -framework CoreGraphics'
        glframework = 'OpenGLES'
        gcc = 'i686-apple-darwin10-gcc' if is_ios_sim else 'armv6-apple-darwin10-gcc'
    else:
        darwin_kit = '-framework AppKit -framework Cocoa -framework CoreServices -framework IOKit'
        gcc = 'clang'
        gpp = 'clang++'
        cppflags = '-std=c++11 -stdlib=libc++'

platform_extraflags = ''
if is_ios:
    target = 'iPhoneOS' if not is_ios_sim else 'iPhoneSimulator'
    arch = 'armv6' if not is_ios_sim else 'i386'
    compiler_path = '/Developer/Platforms/'+target+'.platform/Developer/usr/bin/'
    platform_extraflags = ' -arch '+arch+' -isysroot /Developer/Platforms/'+target+'.platform/Developer/SDKs/'+target+'4.2.sdk -gdwarf-2 -mthumb -miphoneos-version-min=3.0'
    if is_ios_sim:
        platform_extraflags += ' -DLEPRA_IPHONE_SIMULATOR'
else:
    compiler_path = ''
    if is_mac:
        platform_extraflags = ' -mmacosx-version-min=10.7'
cextraflags += platform_extraflags

c_current_flags = '-O0 -ggdb -D_DEBUG' if is_debug else '-O3'
link_type_flag = '-shared '+cppflags #if is_debug else '-static'
link_bin_type_flag = ''+cppflags #if is_debug else '-static'
link_output_ext = '.so' #if is_debug else '.a'

cflags_1 = "C_COMPILER = "+compiler_path+gcc+"""
CPP_COMPILER = """+compiler_path+gpp+"""

CFLAGS = """+c_current_flags+" -fPIC"+cextraflags+""" -D_POSIX_PTHREAD_SEMANTICS %(includes)s -DPOSIX -D_XOPEN_SOURCE=600 -D_CONSOLE -DPNG_NO_ASSEMBLER_CODE -DdSingle -DdTLS_ENABLED=1 -DHAVE_CONFIG_H=1 -DLEPRA_WITHOUT_FMOD
CPPFLAGS = $(CFLAGS) """+cppflags
cflags_2 = "-Wno-unknown-pragmas"
ldflags = platform_extraflags

librt = '-lrt'
libgl = '-lGL'
openal_noui = ''
openal_ui = '-lOpenAL'
mac_hid = ''
space_mac_hid = ''

if is_mac:
    librt = ''
    openal_noui = '-framework OpenAL'
    openal_ui = '-framework OpenAL'
    libgl = '-framework '+glframework
    mac_hid = 'HID'
    space_mac_hid = ' '+mac_hid
    cflags_1 += ' -framework '+glframework+' -framework CoreServices -framework OpenAL -DMAC_OS_X_VERSION=1050'
    ldflags += ' '+darwin_kit+' -lobjc -headerpad_max_install_names '

link_line = "$(CPP_COMPILER) "+link_type_flag+" $(LIBS) " + ldflags + ' ' + openal_noui + " -o $@ $(OBJS)"
#if not is_debug:
#    link_line = 'ar -r -s $@ $(OBJS)'

cflags_head = cflags_1+" -Wall "+cflags_2+"\n"
cflags_nowarn_head = cflags_1+" "+cflags_2+"\n"
libs_head = "LIBS = %(deplib_switches)s -lpthread -ldl " + librt + " %(libs)s\n"
gfx_libs_head = "LIBS = %(deplib_switches)s " + openal_ui + " -lpthread -ldl " + librt + " " + libgl + " %(libs)s\n"

head_lib = cflags_head+libs_head
head_lib_nowarn = cflags_nowarn_head+libs_head
head_gfx_lib = cflags_head+gfx_libs_head

extra_foot_rule_head = "\n.SUFFIXES: .o .mm" if is_mac else "\n.SUFFIXES: .o .cxx"
extra_foot_rule_tail = """.m.o:
\t$(C_COMPILER) $(CFLAGS) -o $@ -c $<
.mm.o:
\t$(CPP_COMPILER) $(CPPFLAGS) -o $@ -c $<""" if is_mac else ""
foot_rules = extra_foot_rule_head + """
depend:
\tmakedepend -- $(CPPFLAGS) -- $(SRCS)
\t#@grep -Ev "\\.o: .*([T]hirdParty|/[u]sr)/" makefile >.tmpmake
\t#@mv .tmpmake makefile
.c.o:
\t$(C_COMPILER) $(CFLAGS) -o $@ -c $<
.cpp.o:
\t$(CPP_COMPILER) $(CPPFLAGS) -o $@ -c $<
.cxx.o:
\t$(CPP_COMPILER) $(CPPFLAGS) -o $@ -c $<
""" + extra_foot_rule_tail + "\n\n"

foot_lib = """
all:\tlib%(lib)s"""+link_output_ext+""" $(OBJS)
clean:
\t@rm -f lib%(lib)s"""+link_output_ext+""" $(OBJS)
lib%(lib)s"""+link_output_ext+""":\t$(OBJS)
\t"""+link_line+"""
"""+foot_rules

foot_lib_nowarn = foot_lib
foot_gfx_lib = foot_lib

head_bin = head_lib

head_gfx_bin = head_gfx_lib

foot_bin = """
all:\t%(lib)s $(OBJS)
clean:
\t@rm -f %(lib)s $(OBJS)
%(lib)s:\t$(OBJS)
\t$(CPP_COMPILER) """+link_bin_type_flag+""" $(LIBS) """ + ldflags + """ -o $@ $(OBJS)
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
\t@cp $@ bin/

$(OBJS):\t$(SRCS)
\t$(MAKE) -C $@

$(SRCS):
\t$(MAKE) -C $@
\t@rm -f $(BINS)
\t@mkdir -p bin
\t@cp $@/*"""+link_output_ext+""" bin/
"""


def printstart(makename):
    sys.stdout.write(os.path.abspath(makename)+":\t")

def printend(type):
    sys.stdout.write("done (type="+type+").\n")

def issrc(fname):
    ext = os.path.splitext(fname)[1]
    return (ext == ".cpp" or ext == ".c" \
		or ((ext == '.m' or ext == ".mm") and is_mac) \
                or (ext == '.cxx' and not is_mac))

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
        rawname = "lib"+rawname+link_output_ext
    pathname = os.path.join(os.path.dirname(vcfile), rawname)
    #print pathname
    return pathname

def generate_makefile(vcfile, makename, includedirs, libdirs, deplibs, header, footer, type):
    if type.endswith('nobuild'):
            return
    libname = convert_out_name(vcfile)
    projbasedir = os.path.dirname(vcfile)
    extrafilter = ""
    if is_mac:
        extrafilter = "\\;X11\\;x11"
    else:
        extrafilter = "\\;Mac\\;mac"
    cpps = os.popen(sys.executable+" vcxprojfiles.py -i "+vcfile+" -f Win32\\;win32"+extrafilter+" --rel-path --base-path="+projbasedir).read()
    #print("CPPs:\n"+cpps)
    cpps = cpps.split()
    cpps = [x for x in cpps if issrc(x)]
    objs = [os.path.splitext(x)[0]+".o" for x in cpps]
    f = create_makefile(makename, vcfile, type)
    deplib_switches = " ".join(deplibs)
    includes = " ".join(["-I%s" % i for i in includedirs])
    libs = " ".join(["-L%s" % i for i in libdirs])
    f.write(header % {"deplib_switches":deplib_switches, "includes":includes, "libs":libs})
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
    files = {"bin":[], "gfx_bin":[], "lib":[], "gfx_lib":[]}

    for name, type, vcfile, deps in vcfileinfolist:
        basetype = "lib" if type.startswith("lib") else type
        files[basetype] += [linux_bin_name(basetype, vcfile)]

        vcfile = os.path.join(basedir, vcfile)
        projdir = os.path.dirname(vcfile)
        includedirs = [os.path.relpath(basedir+"thirdparty/utf8cpp", projdir),
                       os.path.relpath(basedir+"thirdparty/ChibiXM", projdir),
                       os.path.relpath(basedir+"thirdparty/freealut-1.1.0/include/", projdir),
                       os.path.relpath(basedir+"thirdparty/happyhttp-0.1/", projdir),
                       os.path.relpath(basedir+"thirdparty/libogg-1.2.2/include/", projdir),
                       os.path.relpath(basedir+"thirdparty/libvorbis-1.3.2/include/", projdir),
                       os.path.relpath(basedir+"thirdparty/libvorbis-1.3.2/lib/", projdir),
                       os.path.relpath(basedir+"thirdparty/ode-0.11.1/include", projdir),
                       os.path.relpath(basedir+"thirdparty/ode-0.11.1/ode/src", projdir),
                       os.path.relpath(basedir+"thirdparty/ode-0.11.1/ode/src/joints", projdir),
                       os.path.relpath(basedir+"thirdparty/ode-0.11.1/OPCODE", projdir),
                       os.path.relpath(basedir+"thirdparty/ode-0.11.1/GIMPACT/include", projdir),
                       os.path.relpath(basedir+"thirdparty/ode-0.11.1/ou/include", projdir)]
        libdirs = [os.path.relpath(basedir+"thirdparty", projdir),
                   os.path.relpath(basedir+"thirdparty/openal-soft-1.10.622", projdir),
                   os.path.relpath(basedir+"thirdparty/freealut-1.1.0/admin/VisualStudioDotNET/alut", projdir),
                   os.path.relpath(basedir+"lepra", projdir),
                   os.path.relpath(basedir+"tbc", projdir),
                   os.path.relpath(basedir+"cure", projdir),
                   os.path.relpath(basedir+"uilepra", projdir),
                   os.path.relpath(basedir+"uitbc", projdir),
                   os.path.relpath(basedir+"uicure", projdir),
                   os.path.relpath(basedir+"life", projdir),
                   os.path.relpath(basedir+"life/lifeclient", projdir),
                   os.path.relpath(basedir+"life/lifeserver", projdir),
                   os.path.relpath(basedir+"life/lifemaster", projdir)]

        if not is_mac:
            includedirs = [os.path.relpath(basedir+"thirdparty/openal-soft-1.10.622/OpenAL32/Include/", projdir),
                os.path.relpath(basedir+"thirdparty/openal-soft-1.10.622/include/", projdir)]+includedirs
        else:
            includedirs = [os.path.relpath(basedir+"thirdparty/HID_Utilities/", projdir)] + includedirs
            libdirs = [os.path.relpath(basedir+"thirdparty/HID_Utilities/", projdir)] + libdirs

        makename = os.path.join(os.path.dirname(vcfile), "makefile")
        printstart(makename)
        deplibs = get_dep_libs(vcfileinfolist, deps)
        generate_makefile(vcfile, makename, includedirs, libdirs, deplibs, eval("head_"+type), eval("foot_"+type), type)
        printend(type)

    makename = os.path.join(basedir, "makefile")
    printstart(makename)
    generate_base_make(makename, files["bin"]+files["gfx_bin"], files["lib"]+files["gfx_lib"])
    printend("base")


def main():
    basedir = "../../"
    projects = [
        ["thirdparty",      "lib_nowarn", "thirdparty/thirdpartyLib_14.vcxproj", ""],
        ["alut",            "lib_nowarn", "thirdparty/freealut-1.1.0/admin/VisualStudioDotNET/alut/alut14.vcxproj", ""],
        ["lepra",           "lib",        "lepra/lepra14.vcxproj", "thirdparty"],
        ["tbc",             "lib",        "tbc/tbc14.vcxproj", "lepra"],
        ["cure",            "lib",        "cure/cure14.vcxproj", "tbc"],
        ["uilepra",         "gfx_lib",    "uilepra/uilepra14.vcxproj", "lepra alut"+space_mac_hid],
        ["uitbc",           "gfx_lib",    "uitbc/uitbc14.vcxproj", "uilepra tbc"],
        ["uicure",          "gfx_lib",    "uicure/uicure14.vcxproj", "uitbc cure"],
        ["life",            "lib",        "life/life14.vcxproj", "cure"],
        ["lifeserver",      "lib",        "life/lifeserver/lifeserver14.vcxproj", "life"],
        ["lifemaster",      "lib",        "life/lifemaster/lifemaster14.vcxproj", "life"],
        ["lifeclient",      "gfx_lib",    "life/lifeclient/lifeclient14.vcxproj", "life uicure"],
        ["slimevolleyball", "gfx_bin",    "slimevolleyball/slimevolleyball14.vcxproj", "uitbc"],
        ["killcutie",       "gfx_bin",    "killcutie/killcutie14.vcxproj", "uicure"],
        ["push",            "gfx_bin",    "push/push14.vcxproj", "lifeclient lifeserver"],
        ["hovertank",       "gfx_bin",    "hovertank/hovertank14.vcxproj", "lifeclient lifeserver"],
        ["downwash",        "gfx_bin",    "downwash/downwash14.vcxproj", "lifeclient"],
        ["fire",            "gfx_bin",    "fire/fire14.vcxproj", "lifeclient"],
        ["bound",           "gfx_bin",    "bound/bound14.vcxproj", "lifeclient"],
        ["trabantsim",      "gfx_bin",    "trabantsim/trabantsim14.vcxproj", "lifeclient"],
        ["impuzzable",      "gfx_bin",    "impuzzable/impuzzable14.vcxproj", "lifeclient"],
        ["pushserver",      "bin",        "push/pushserver/pushserver14.vcxproj", "lifeserver"],
        ["hovertankserver", "bin",        "hovertank/hovertankserver/hovertankserver14.vcxproj", "lifeserver"],
        ["pushmaster",      "bin",        "push/pushmaster/pushmaster14.vcxproj", "lifemaster"],
        ["hovertankmaster", "bin",        "hovertank/hovertankmaster/hovertankmaster14.vcxproj", "lifemaster"],
        ["tirefire",        "gfx_bin",    "tirefire/tirefire14.vcxproj", "uicure"],
        ["bounce",          "gfx_bin",    "bounce/bounce14.vcxproj", "uicure"],
        ["cureTest",        "gfx_bin",    "uicure/curetestapp/curetestapp14.vcxproj", "uicure"]]

    if not is_mac:
        projects = [["OpenAL", "lib_nowarn", "thirdparty/openal-soft-1.10.622/OpenAL_14.vcxproj", ""]] + projects
    else:
        projects = [[mac_hid, "lib_nowarn", "thirdparty/HID_Utilities/HID_Utilities.vcxproj", ""]] + projects

    skipnames = os.environ.get("PD_SKIP_PROJECTS")
    if skipnames:
        for skip in skipnames.split(":"):
            drop_projects = []
            for project in projects:
                if project[0].find(skip) >= 0:
                    print("Dropping project %s." % str(project))
                    drop_projects += [project]
            [projects.remove(dp) for dp in drop_projects]
    #print(projects)

    generate_makefiles(basedir, projects)

if __name__ == '__main__':
    main()
