@echo off
rem Restore ZKC42V from backup\zkc42v-app-0x404000.bin
setlocal
pushd %~dp0..\..
set "JL=JLink.exe"
where "%JL%" >nul 2>nul
if errorlevel 1 (
  if exist "C:\Program Files\SEGGER\JLink\JLink.exe" set "JL=C:\Program Files\SEGGER\JLink\JLink.exe"
)
"%JL%" -Device OM662XX -If SWD -Speed 4000 -AutoConnect 1 -CommanderScript firmware\jlink\restore_42v.jlink
popd
