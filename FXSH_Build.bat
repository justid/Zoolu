@echo off
rem Do not edit! This batch file is created by CASIO fx-9860G SDK.


if exist ZOOLU.G1A  del ZOOLU.G1A

cd debug
if exist FXADDINror.bin  del FXADDINror.bin
"D:\fx-9860SDK\OS\SH\Bin\Hmake.exe" Addin.mak
cd ..
if not exist debug\FXADDINror.bin  goto error

"D:\fx-9860SDK\Tools\MakeAddinHeader363.exe" "F:\ªÊ–«"
if not exist ZOOLU.G1A  goto error
echo Build has completed.
goto end

:error
echo Build was not successful.

:end

