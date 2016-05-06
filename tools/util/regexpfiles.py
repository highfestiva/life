#!/bin/env python

def printUsage():
	import sys
	print("%s <regexp-search> <regexp-replace> <wildcard>" % sys.argv[0])
	print("Example: %s \"<td>[0-9]*</td>\\r\\n\" 0 *.html" % sys.argv[0])

def regexpreplace(r, w, search, replace):
        import re
        data = r.read();
        data = re.sub(search, replace, data);
        w.write(data);

def main():
	import sys
	args = sys.argv;
	if len(args) != 4:
		printUsage();
		sys.exit(1)
	import glob
	import os
	search, replace, wildcard = args[1:4]
	fls = glob.glob(wildcard)
	for fn in fls:
		outfn = fn+".tmp"
		try:
			with open(fn, "rt") as r:
				with open(outfn, "wt") as w:
					regexpreplace(r, w, search, replace)
			os.remove(fn)
			os.rename(outfn, fn)
			print("Replaced in " + fn)
		except:
			try:
				os.remove(outfn)
			except:
				pass
			#print("No replace in " + fn)

if __name__ == "__main__":
	main()
