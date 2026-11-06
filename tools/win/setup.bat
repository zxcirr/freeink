@echo off
rem Install python dependencies for the FreeInk host tool.
setlocal
pushd %~dp0..\..
echo [FreeInk] installing bleak + Pillow ...
py -3 -m pip install -r host\requirements.txt || python -m pip install -r host\requirements.txt
py -3 -c "import bleak, PIL; print('OK: deps installed')" || python -c "import bleak, PIL; print('OK: deps installed')"
popd
pause
