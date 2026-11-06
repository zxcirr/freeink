@echo off
rem Restore ZKC21V first slot to stock InkTag recovery
@rem no setlocal here: keep the JLink path found by find_jlink.bat
pushd %~dp0..\..
call "%~dp0find_jlink.bat" || (popd & exit /b 1)
"%JL%" -Device OM6626 -If SWD -Speed 4000 -AutoConnect 1 -CommanderScript firmware\jlink\restore_21v.jlink
popd
