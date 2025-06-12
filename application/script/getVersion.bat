@echo off
set WORKPATH=%~dp0
set DISK=%~d0
pushd %WORKPATH%
%DISK%
cd %WORKPATH%
cd ..


cd .\script
bash version_maker.sh
