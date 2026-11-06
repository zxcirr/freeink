@echo off
rem Send the built-in test card.   usage: test.bat 21|42
setlocal
if "%~1"=="" (echo usage: test.bat 21^|42 & exit /b 1)
pushd %~dp0..\..
py -3 host\zkc_epd.py --name ZKC%~1V --test
if errorlevel 1 python host\zkc_epd.py --name ZKC%~1V --test
popd
