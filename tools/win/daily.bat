@echo off
rem Generate today's card and send it.   usage: daily.bat 21|42
setlocal
if "%~1"=="" (echo usage: daily.bat 21^|42 & exit /b 1)
set "TAG=ZKC%~1V"
if "%~1"=="21" set "PANEL=213"
if "%~1"=="42" set "PANEL=420"
pushd %~dp0..\..
set "PYEXE=py -3"
%PYEXE% --version >nul 2>nul || set "PYEXE=python"
echo [daily] generating card for %TAG% ...
for /f "delims=" %%F in ('%PYEXE% host\calendar_card.py --panel %PANEL%') do set "CARD=%%F"
if not defined CARD (echo card generation failed & popd & exit /b 1)
echo [daily] sending %CARD% ...
%PYEXE% host\zkc_epd.py --name %TAG% --image "%CARD%"
popd
