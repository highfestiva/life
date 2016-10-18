#!/bin/bash

rgo='python3 -B tools/build/rgo.py'

cd ~/pd/
$rgo set_target trabantsim TrabantSim
rm -Rf bin
#rm makefile
$rgo -m final buildcode builddata zipdata macappify
mkdir bin/Trabant/
mv bin/TrabantSim.app bin/Trabant/
cp -r trabantsim/prototypes/* bin/Trabant/
cd bin/Trabant/
rm -Rf `find | grep '__pycache__$'`
rm `find . | grep '\./_'`
rm `find . | grep '\.pyc$'`
cd ..
zip -r -9 Trabant_Mac_CmdLine.zip Trabant/
mv Trabant_Mac_CmdLine.zip ..
