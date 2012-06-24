#!/bin/env python3

import glob
import os
import re
import sys

def printUsage():
	print("%s <wildcard> <replacement wildcard>" % sys.argv[0])
	print("Example: %s * Ui*" % sys.argv[0])
	print("Only one asterisk may be used in the wildcards.")

def renameFiles(wildcard, replacement):
	wildcardRegex = wildcard.replace('.', '\\.').replace('*', '(.*)')
	replacementRegex = replacement.replace('*', '\\1')
	infiles = glob.glob(wildcard)
	for infile in infiles:
		outfile = re.sub(wildcardRegex, replacementRegex, infile);
		print(infile, "->", outfile)
		os.rename(infile, outfile)

def main():
	if len(sys.argv) == 3:
		wildcardList = sys.argv[1].split("*")
		replacementList = sys.argv[2].split("*")
		if len(wildcardList) == 2 and len(replacementList) == 2:
			renameFiles("*".join(wildcardList), "*".join(replacementList))
		else:
			printUsage()
	else:
		printUsage();

if __name__ == "__main__":
	main()
