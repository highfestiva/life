#!/bin/bash


cd ../../TrabantSim/prototypes
python -m pydoc -w trabant
cat trabant.html | perl -pe 's/<a href="(builtins.html.*?|random.html.*?|[^#]*?)">(.*?)<\/a>/\2/g' | perl -pe 's/c:\\.*?__init__.py/trabant\/__init__.py/g' > ../../TrabantIDE/build/trabant_py_api.html
rm trabant.html
sed '/__api__/{
	s/__api__//g
	r ../../TrabantIDE/build/trabant_py_api.html
	}' /c/Users/Jonte/Documents/Pixel\ Doctrine\ AB/WebDev/html/trabant_help.template > \
		/c/Users/Jonte/Documents/Pixel\ Doctrine\ AB/WebDev/html/trabant_help.html
