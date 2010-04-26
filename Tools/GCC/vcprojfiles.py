#!/usr/bin/env python


from optparse import OptionParser
import sys, xml.dom.minidom, os

def relpath(target, base=os.curdir):
    if not os.path.exists(target):
        raise OSError('Target does not exist: '+target)

    if not os.path.isdir(base):
        raise OSError('Base is not a directory or does not exist: '+base)

    base_list = (os.path.abspath(base)).split(os.sep)
    target_list = (os.path.abspath(target)).split(os.sep)

    if os.name in ['nt','dos','os2'] and base_list[0] != target_list[0]:
        raise OSError('Target is on a different drive to base. Target: '+target_list[0].upper()+', base: '+base_list[0].upper())

    for i in range(min(len(base_list), len(target_list))):
        if base_list[i] != target_list[i]: break
    else:
        i+=1

    rel_list = [os.pardir] * (len(base_list)-i) + target_list[i:]
    return os.path.join(*rel_list)


def main():
    parser = OptionParser("usage: %prog -i <input file> [options]")
     
    parser.description = "%prog is used to strip out docstrings from .py-files"
    parser.epiolog = "enjoy."
    parser.add_option("-i", "--input-file", dest="input", help="Reads input from FILE", metavar="FILE")
    parser.add_option("-s",  "--suffix", dest="suffix", help="List only file that has the suffix SUFF", metavar="SUFF")
    parser.add_option("-r", "--rel-path", dest="absPath", action="store_false", help="Lists the files with a relative path", default=False)
    parser.add_option("-a", "--abs-path", dest="absPath", action="store_true", help="Lists the files with an absolute path", default=False)
    parser.add_option("-q", "--qouted", dest="qouted", action="store_true", help="Qoutes the output with double qoutes (i.e. \"<file>\")", default=False)
    parser.add_option("-b", "--base-path", dest="base", help="If used with --rel-path this will set the base directory from where the path should be relative from.", default=False)
    parser.add_option("-f", "--filter-out", dest="filter", help="Remove any entries containing SUBSTR, to use several filter separate each with a ';'.", metavar="SUBSTR")
    parser.add_option("-n", "--basename", dest="basename", action="store_true", help="List only the basename of each file. That is; ../example_dir/example_file.cpp will only be shown as example_file.cpp.", default=False)
        
    (options, args) = parser.parse_args()

    if not options.input:
        parser.print_help()
        sys.exit(1)

    projDir = os.path.dirname(options.input)
    try:
        mdp = xml.dom.minidom.parse(options.input)
    except IOError as e:
        print(e.args[1] + ":", e.filename)
        sys.exit(2)
    files = mdp.getElementsByTagName("File")
    for f in files:
        relp = f.getAttribute("RelativePath").replace("\\", os.path.sep)
        doPrint = True
        if options.suffix:
            doPrint = False
            if options.suffix == relp.split(".")[-1]:
                doPrint = True               
        if options.filter:
            for fe in options.filter.split(';'):
                if fe in relp:
                    doPrint = False
        if doPrint:
            if options.qouted: q = "\""
            else: q = ""
            filepath = os.path.abspath(os.path.join(projDir, relp))
            if options.basename:
                print(q + os.path.basename(filepath)  + q)
            elif options.absPath:
                print(q + filepath + q)
            else:
                if options.base: base = options.base
                else: base = os.curdir
                try:
                    print(q + relpath(filepath, base) + q)
                except OSError as e:
                    print('OSError: %s' % e.message, file=sys.stderr)

if __name__ == '__main__':
    main()
