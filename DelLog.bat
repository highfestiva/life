move ThirdParty ..

FOR /F "tokens=*" %%G IN ('DIR /B /A-D /S *.log') DO del /S /Q "%%G"

move ..\ThirdParty .

pause
