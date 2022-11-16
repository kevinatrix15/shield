@echo off
@set CTEST_OUTPUT_ON_FAILURE=1
:: Build script for windows version of SaveBB8
:: Run from project root folder

:: Set defaults
set DEF_GENERATOR_NAME="Visual Studio 16 2019"
set DEF_DEVENV="c:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/Common7/IDE/devenv.com"
set BUILD_DIR=win_build
set COMPILE_CODE=1
set UNITTESTING=1

:: Parse arguments
:ParseArgs
if /i "%1"=="" (
   goto Build
)
if /i "%1"=="-help" (
   goto Syntax
)
if /i "%1" == "/?" (
   goto Syntax
)
if /i "%1"=="/D" (
   if exist "%~2" (
     set DEVENV=%2
     shift & shift & goto ParseArgs
   ) else (
     echo /D: Does not exist: %2
     goto Syntax
   )
)
if /i "%1"=="-devenv" (
   if exist "%~2" (
     set DEVENV=%2
     shift & shift & goto ParseArgs
   ) else (
     echo -devenv: Does not exist: %2
     goto Syntax
   )
)
if /i "%1"=="/G" (
   if /i "%~2"=="" (
     goto Syntax
   ) else (
     set GENERATOR_NAME=%2
     shift & shift & goto ParseArgs
   )
)
if /i "%1"=="-generator" (
   if /i "%~2"=="" (
     goto Syntax
   ) else (
     set GENERATOR_NAME=%2
     shift & shift & goto ParseArgs
   )
)
echo Unknown argument: %1
goto Syntax

:Build
if not defined GENERATOR_NAME (
  set GENERATOR_NAME=%DEF_GENERATOR_NAME%
)
if not defined DEVENV (
  set DEVENV=%DEF_DEVENV%
)

:: Setup up build directory
rmdir /s /q %BUILD_DIR%
mkdir %BUILD_DIR%
cd %BUILD_DIR%
:: Run cmake
cmake -G %GENERATOR_NAME%^
 .. || exit /b !ERRORLEVEL!

:: Compile

if "%COMPILE_CODE%"=="1" (
  %DEVENV% SaveBB8.sln /Build "Release|x64"^
   /out release_log.txt || exit /b !ERRORLEVEL!
  if "%UNITTESTING%"=="1" (
    %DEVENV% SaveBB8.sln /Build "Release|x64"^
     /Project "RUN_TESTS" || exit /b !ERRORLEVEL!
  )
)
cd ..

exit /B 0

:: Display help
:Syntax
echo Syntax:
echo  %SCRIPT_NAME% [/D ^<path^>] [/G ^<Name^>]
echo.
echo        /D,-devenv           path to MSVC devenv
echo                             full path recommended
echo                             default: %DEF_DEVENV%
echo        /G,-generator        CMake Generator name
echo                             default: %DEF_GENERATOR_NAME%
echo.
exit /B 1
