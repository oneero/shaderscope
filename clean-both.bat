@echo off

REM Remove build folder

@RD /S /Q "%~dp0\third-party\build"

@RD /S /Q "%~dp0\build"