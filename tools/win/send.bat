@echo off
rem Send an image to a tag.   usage: send.bat 21|42 image.png [extra zkc_epd args]
rem example: send.bat 21 examples\mycard.png --preview out.png
setlocal
if "%~1"=="" (echo usage: send.bat 21^|42 image.png [args] & exit /b 1)
if "%~2"=="" (echo usage: send.bat 21^|42 image.png [args] & exit /b 1)
set "TAG=ZKC%~1V"
pushd %~dp0..\..
py -3 host\zkc_epd.py --name %TAG% --image "%~2" %3 %4 %5 %6
if errorlevel 1 python host\zkc_epd.py --name %TAG% --image "%~2" %3 %4 %5 %6
popd
