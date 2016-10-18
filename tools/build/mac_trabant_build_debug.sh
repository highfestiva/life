#!/bin/bash

rgo='python3 -B Tools/build/rgo.py'

$rgo copycode
rm bin/*
rm uilepra/libuilepra.so
rm uilepra/src/mac/uimacinput.o
cd uilepra
make
cd ..
rm trabantsim/trabantsim
make
cd trabantsim
make
cd ..
cp trabantsim/trabantsim bin/
$rgo macappify
cd bin/TrabantSim.app/Contents/MacOS
lldb trabantsim

