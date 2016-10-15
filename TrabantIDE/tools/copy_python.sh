#!/bin/bash

cd ..

rm -Rf python3?/
mkdir python35
cd python35

pydir=$(dirname $(which python3))
echo Copying from $pydir...

cp $pydir/python.exe $pydir/python35.dll .

mkdir Scripts
cp $pydir/Scripts/pyinstaller-script.py Scripts/

cp -r $pydir/DLLs .

mkdir Lib
cp $pydir/Lib/* Lib/ 2> /dev/null
cp -r $pydir/Lib/collections Lib/
cp -r $pydir/Lib/ctypes Lib/
cp -r $pydir/Lib/distutils Lib/
cp -r $pydir/Lib/email Lib/
cp -r $pydir/Lib/encodings Lib/
cp -r $pydir/Lib/http Lib/
cp -r $pydir/Lib/importlib Lib/
cp -r $pydir/Lib/json Lib/
cp -r $pydir/Lib/lib2to3 Lib/
cp -r $pydir/Lib/logging Lib/
cp -r $pydir/Lib/urllib Lib/
cp -r $pydir/Lib/xml Lib/
mkdir Lib/site-packages
cp $pydir/Lib/site-packages/*.py Lib/site-packages
cp $pydir/Lib/site-packages/pypiwin32.pth Lib/site-packages
cp -r $pydir/Lib/site-packages/pkg_resources Lib/site-packages
cp -r $pydir/Lib/site-packages/PyInstaller* Lib/site-packages
cp -r $pydir/Lib/site-packages/pefile* Lib/site-packages
cp -r $pydir/Lib/site-packages/setuptools* Lib/site-packages
cp -r $pydir/Lib/site-packages/_markerlib Lib/site-packages
cp -r $pydir/Lib/site-packages/future* Lib/site-packages
cp -r $pydir/Lib/site-packages/pypiwin32_* Lib/site-packages
cp -r $pydir/Lib/site-packages/win32 Lib/site-packages
cp -r $pydir/Lib/site-packages/past Lib/site-packages
cp -r $pydir/Lib/site-packages/libfuturize Lib/site-packages
cp -r $pydir/Lib/site-packages/ordlookup Lib/site-packages

cd Lib
rm turtle.py doctest.py mailbox.py smtp*.py ftp*.py imap*.py poplib.py
rm -Rf `find | grep __pycache__`
echo "Done with ../python35/"
