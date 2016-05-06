#!/bin/bash

rgo='python3 -B Tools/build/rgo.py'

$rgo copycode
rm bin/*
rm UiLepra/libUiLepra.so
rm UiLepra/Source/Mac/UiMacInput.o
cd UiLepra
make
cd ..
rm TrabantSim/TrabantSim
make
cd TrabantSim
make
cd ..
cp TrabantSim/TrabantSim bin/
$rgo macappify
cd bin/TrabantSim.app/Contents/MacOS
lldb TrabantSim

