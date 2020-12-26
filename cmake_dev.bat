@echo off

mkdir %~dp0\build_jobs_vs2019
cd %~dp0\build_jobs_vs2019

cmake .. -G "Visual Studio 16 2019" -A x64 -DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_SHARED_LIBS=YES -DCMAKE_INSTALL_PREFIX=%cd%/install-prefix

if %ERRORLEVEL% == 0 GOTO no_error

pause

:no_error
