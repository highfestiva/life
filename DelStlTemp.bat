cd ThirdParty\stlport


REM - Delete files:

FOR /F "tokens=*" %%G IN ('DIR /B /A-D /S *.o') DO del /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /A-D /S *.pdb') DO del /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /A-D /S *.dll') DO del /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /A-D /S *.exp') DO del /S /Q "%%G"


REM - Delete directories:

FOR /F "tokens=*" %%G IN ('DIR /B /AD /S obj') DO RMDIR /S /Q "%%G"


cd ..\..

pause
