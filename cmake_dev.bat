@echo off

mkdir %~dp0\build_jobs_vs2019
cd %~dp0\build_jobs_vs2019

cmake .. -G "Visual Studio 16 2019" -A x64 -DLIBCOPP_FCONTEXT_USE_TSX=ON -DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON

if %ERRORLEVEL% == 0 GOTO no_error

pause

:no_error
