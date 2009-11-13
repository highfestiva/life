#!/bin/bash

/c/Program/Python31/python.exe -B ../Tools/Maya/import_chunky.py fjask
if [[ "$?" == "0" ]]; then
/c/Program/Python31/python.exe -B ../Tools/Maya/import_chunky.py tractor_01
#/c/Program/Python31/python.exe -B ../Tools/Maya/import_chunky.py simple_scale_shear
#python3.1 -B ../Tools/Maya/import_chunky.py simple_scale_shear
#python3.1 -B ../Tools/Maya/import_chunky.py simple_scale
fi

#if [[ "$?" == "0" ]]; then
#./r.sh
#fi
