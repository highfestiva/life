FOR /F "tokens=*" %%G IN ('DIR /B /A-D /S *.ncb') DO del /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /A-D /S *.suo') DO del /S /Q "%%G"

move ThirdParty ..

FOR /F "tokens=*" %%G IN ('DIR /B /AD /S Debug') DO RMDIR /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S "Unicode Debug"') DO RMDIR /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S lib') DO RMDIR /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *Final*') DO RMDIR /S /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *Release*') DO RMDIR /S /Q "%%G"

move ..\ThirdParty .

rmdir /s /q ThirdParty\Debug
rmdir /s /q ThirdParty\Release
del ThirdParty\*.lib

pause
