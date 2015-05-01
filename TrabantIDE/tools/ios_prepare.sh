#!/bin/bash

. builddoc.sh

rm ~/RnD/python-embedded/pylib/prototypes/*
rm ~/RnD/python-embedded/pylib/sitepkgs/trabant/*
cp ../../TrabantSim/prototypes/* ~/RnD/python-embedded/pylib/prototypes/
cp ../../TrabantSim/prototypes/trabant/* ~/RnD/python-embedded/pylib/sitepkgs/trabant/
rm ~/RnD/python-embedded/pylib/prototypes/_*.py

cd ../..
python -B Tools/build/rgo.py cleandata zipdata

