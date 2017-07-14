#!/bin/bash

cd ../../trabantsim/prototypes
python -m pydoc -w trabant
cat trabant.html | perl -pe 's/<a href="(builtins.html.*?|random.html.*?|[^#]*?)">(.*?)<\/a>/\2/g' | perl -pe 's/c:\\.*?__init__.py/trabant\/__init__.py/g' | perl -pe 's/\/Users\/.*?__init__.py/trabant\/__init__.py/g' > ../../TrabantIDE/tools/trabant_py_api.html
rm trabant.html
if [ -d /c/Users ]; then
sed '/__api__/{
	s/__api__//g
	r ../../TrabantIDE/tools/trabant_py_api.html
	}' /c/data/pd-company/WebDev/site-trabant/trabant_help.template > \
		/c/data/pd-company/WebDev/site-trabant/trabant_help.html
echo "Updated html documentation."
fi
