@echo off
rem Backup stock flash of ZKC42V to backup\ (DO THIS FIRST!)
setlocal
pushd %~dp0..\..
set "JL=JLink.exe"
where "%JL%" >nul 2>nul
if errorlevel 1 (
  if exist "C:\Program Files\SEGGER\JLink\JLink.exe" set "JL=C:\Program Files\SEGGER\JLink\JLink.exe"
)
"%JL%" -Device OM662XX -If SWD -Speed 4000 -AutoConnect 1 -CommanderScript firmware\jlink\backup_42v.jlink
popd
