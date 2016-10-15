#!/bin/bash

cd ..

rm -Rf python3?/
mkdir python35
cd python35

pydir=$(dirname $(which python3))
echo Copying from $pydir...

cp $pydir/python.exe $pydir/python35.dll .

mkdir Scripts
cp $pydir/Scripts/pyinstaller.exe $pydir/Scripts/pyinstaller-script.py Scripts/

cp -r $pydir/DLLs .

mkdir Lib
cp $pydir/Lib/* Lib/ 2> /dev/null
cp -r $pydir/Lib/collections Lib/
cp -r $pydir/Lib/ctypes Lib/
cp -r $pydir/Lib/encodings Lib/
cp -r $pydir/Lib/importlib Lib/
cp -r $pydir/Lib/json Lib/

cd Lib
rm turtle.py doctest.py mailbox.py smtp*.py ftp*.py imap*.py poplib.py
echo "Done with ../python35/"
