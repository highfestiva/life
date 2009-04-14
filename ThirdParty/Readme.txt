The ThirdPartyLib project concatenates all third party libraries 
into one. In case you want to create an equivalent project for 
another platform/compiler, please read the following notes.

The MTwist library contain c-files which must be treated as cpp-
files. Thus, make sure to configure your project to compile them 
as C++ code.

NOTE: This applies to all .c files in all projects. Do not rename
the files to .cpp since the code in these files is still C-code 
and not C++.
