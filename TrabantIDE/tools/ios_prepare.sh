#!/bin/bash

./builddoc.sh

rm ~/RnD/python-embedded/pylib/prototypes/*
rm ~/RnD/python-embedded/pylib/sitepkgs/trabant/*
cp ../../TrabantSim/prototypes/* ~/RnD/python-embedded/pylib/prototypes/
cp ../../TrabantSim/prototypes/trabant/* ~/RnD/python-embedded/pylib/sitepkgs/trabant/
rm ~/RnD/python-embedded/pylib/prototypes/_*.py
echo "Fixing XCode signing by dropping shebang..."
./xcode_py_fixup.py ~/RnD/python-embedded/pylib

cd ../..
python -B tools/build/rgo.py cleandata zipdata
