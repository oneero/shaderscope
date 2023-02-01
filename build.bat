@echo off

call .\clean.bat

call .\configure-ninja.bat

cmake --build .\build\debug-ninja\
