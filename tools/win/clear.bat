@echo off
rem Clear the panel.   usage: clear.bat 21|42
setlocal
if "%~1"=="" (echo usage: clear.bat 21^|42 & exit /b 1)
pushd %~dp0..\..
py -3 host\zkc_epd.py --name ZKC%~1V --clear
if errorlevel 1 python host\zkc_epd.py --name ZKC%~1V --clear
popd
