
# Py code

import os
import fnmatch

class GlobDirectoryWalker:
    # a forward iterator that traverses a directory tree

    def __init__(self, directory, pattern="*", mustmatch_files=[], nomatch_files=[], nomatch_dirs=[]):
        self.stack = [directory]
        self.pattern = pattern
        self.mustmatch_files = mustmatch_files
        self.nomatch_files = nomatch_files
        self.nomatch_dirs = nomatch_dirs
        self.files = []
        self.index = 0

    def __getitem__(self, index):
        while 1:
            try:
                file = self.files[self.index]
                self.index = self.index + 1
            except IndexError:
                # pop next directory from stack
                self.directory = self.stack.pop()
                self.files = os.listdir(self.directory)
                self.index = 0
            else:
                # got a filename
                fullname = os.path.join(self.directory, file)
                skip_match = False
                if os.path.isdir(fullname):
                    for nomatch in self.nomatch_dirs:
                        if fnmatch.fnmatchcase(file, nomatch):  # TRICKY: directories are case sensitive!
                            skip_match = True
                            break
                else:
                    is_must = False
                    for mustmatch in self.mustmatch_files:
                        if fnmatch.fnmatch(file, mustmatch):
                            is_must = True
                            break
                    if not is_must:
                        for nomatch in self.nomatch_files:
                            if fnmatch.fnmatch(file, nomatch):
                                skip_match = True
                                break
                if skip_match:
                    continue
                if os.path.isdir(fullname) and not os.path.islink(fullname):
                    self.stack.append(fullname)
                if fnmatch.fnmatch(file, self.pattern):
                    return fullname

def working_makedirs(newdir):
    """works the way a good mkdir should :)
        - already exists, silently complete
        - regular file in the way, raise an exception
        - parent directory(ies) does not exist, make them as well
    """
    if os.path.isdir(newdir):
        pass
    elif os.path.isfile(newdir):
        raise OSError("a file with the same name as the desired " \
                      "dir, '%s', already exists." % newdir)
    else:
        head, tail = os.path.split(newdir)
        if head and not os.path.isdir(head):
            working_makedirs(head)
        #print "working_makedirs %s" % repr(newdir)
        if tail:
            os.mkdir(newdir)


def main():
    fl = GlobDirectoryWalker(".", "*",
        ("fmod*.lib", "fmod*.dll", "DspFx.dll"),
        ("*.o", "*.obj", "*.log", ".cvsignore", "*.vcproj.*.user", "*.suo", "*.lib", "*.exp", "*.dll", "*.pdb", "*.ncb"),
        ("*.svn", "*.git", "Unicode Release*", "Unicode Final", "Unicode Debug*", "Release Candidate", "Debug", "Release", "CVS"))
    print "Copying..."
    target_dir = "..\\bak"
    cnt = 0
    import shutil
    import sys
    for f in fl:
        if os.path.isfile(f):
            sys.stdout.write(".")
            target_file = os.path.join(target_dir, f.lstrip(".").lstrip("/").lstrip("\\"))
            target_subdir = os.path.split(target_file)[0]
            working_makedirs(target_subdir)
            shutil.copyfile(f, target_file)
            cnt += 1
    print
    print cnt, "files backed-up to "+target_dir+"."

if __name__ == "__main__":
    main()
