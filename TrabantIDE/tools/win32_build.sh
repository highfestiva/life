#!/bin/bash

rgo='python -B Tools/build/rgo.py'

cd ../..
rm -Rf bin
mkdir bin
$rgo -m final copycode cleandata builddata zipdata
mkdir bin/sim/
mv bin/* bin/sim/

cp ../cpp/scite/bin/TrabantIDE.exe TrabantIDE/
cp -r TrabantIDE/* bin/
rm -Rf bin/tools
rm bin/todo.txt

cp -r trabantsim/prototypes bin/
rm -Rf bin/prototypes/_*.py bin/*/__pycache__ bin/*/*/__pycache__

mkdir bin/sync
cp ../py/sync/* bin/sync/
rm bin/sync/test.py

mkdir bin/sync/originals
cp bin/prototypes/* bin/sync/originals

mkdir bin/Trabant
mv bin/* bin/Trabant/
cd bin
/c/Program\ Files/7-Zip/7z.exe a -tzip Trabant_Win32
cd ..
mv bin/Trabant_Win32.zip .
echo "Done! .zip in PD root folder."
