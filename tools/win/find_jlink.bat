@echo off
rem Locate JLink.exe. On success sets JL for the caller (no setlocal here!).
rem Search order: cached jlink.cfg -> PATH -> common install dirs -> ask user.
rem The resolved path is cached in jlink.cfg next to this file.
set "CFG=%~dp0jlink.cfg"
set "JL="

if exist "%CFG%" (
  for /f "usebackq delims=" %%L in ("%CFG%") do set "JL=%%L"
)
if defined JL if not exist "%JL%" (echo [JLink] stale jlink.cfg, re-detecting... & del "%CFG%" & set "JL=")
if defined JL if exist "%JL%" goto have

for %%F in (JLink.exe) do if not "%%~$PATH:F"=="" (set "JL=%%~$PATH:F" & goto have)

if defined ProgramFiles if exist "%ProgramFiles%\SEGGER\JLink\JLink.exe" (set "JL=%ProgramFiles%\SEGGER\JLink\JLink.exe" & goto have)
if defined ProgramFiles(x86) if exist "%ProgramFiles(x86)%\SEGGER\JLink\JLink.exe" (set "JL=%ProgramFiles(x86)%\SEGGER\JLink\JLink.exe" & goto have)
if exist "C:\Program Files\SEGGER\JLink\JLink.exe" (set "JL=C:\Program Files\SEGGER\JLink\JLink.exe" & goto have)

echo JLink.exe was not found automatically.
echo You can paste the full path, e.g.
echo   D:\Program Files\SEGGER\JLink\JLink.exe
set /p "JL=Path to JLink.exe (empty to retry): "
if exist "%JL%" goto have
set /p "JL=Path to JLink.exe (empty to abort): "
if exist "%JL%" goto have
echo ERROR: JLink.exe not found. Install SEGGER JLink and retry.
exit /b 1

:have
if /i not "%JL%"=="JLink.exe" (>"%CFG%" echo %JL%)
echo [JLink] %JL%
exit /b 0
