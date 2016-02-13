
@echo off
if not "%DevEnvDir%" == "" goto start
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"

:start
devenv gbe.sln /build

if %errorlevel% GEQ 1 goto end
debug\gbe.exe ..\roms\DMG_ROM.bin ..\roms\tetris.gb

:end