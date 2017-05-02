@echo off
setLocal EnableDelayedExpansion

set "src_dir=%~dp0"
rem Remove trailing backslash. Possibly not safe if we are in a strange directory
if "!src_dir:~-1!"=="\" (
	set src_dir=!src_dir:~,-1!
)

set cur_dir=%cd%

set build32=%src_dir%-build\VisualStudio14\win32
set build64=%src_dir%-build\VisualStudio14\win64

if "%1"=="cmake" GOTO cmake
if "%1"=="build" GOTO build

:cmake

echo %src_dir%-build\VisualStudio14

rd /S /Q %src_dir%-build\VisualStudio14	
mkdir %src_dir%-build
mkdir %src_dir%-build\VisualStudio14
mkdir %build32%
mkdir %build64%

cd %build32%
cmake -G "Visual Studio 14" %options% %src_dir%
cd %build64%
cmake -G "Visual Studio 14 Win64" %options% %src_dir%

:build

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vsvars32.bat"

cd %build32%
msbuild ZERO_CHECK.vcxproj /p:Configuration=Debug
msbuild INSTALL.vcxproj /p:Configuration=Debug
msbuild INSTALL.vcxproj /p:Configuration=RelWithDebInfo

:build64

cd %build64%
msbuild ZERO_CHECK.vcxproj /p:Configuration=Debug
msbuild INSTALL.vcxproj /p:Configuration=Debug
msbuild INSTALL.vcxproj /p:Configuration=RelWithDebInfo

:cleanup
cd %cur_dir%