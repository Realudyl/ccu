@echo off
set WORKPATH=%~dp0
set DISK=%~d0
pushd %WORKPATH%
%DISK%
cd %WORKPATH%
cd ..

:: 设置编译生成路径
set "OUTPUTNAME=Objects"
set "UPDATENAME=update"

set TOOL=.\tools\update

:: 设置加密生成路径
set SECRETPATH=..\project\secret

:: 提取上一级目录的工程名
cd ..
for /f "delims=" %%a in ("%cd%") do (
    set "ProjectName=%%~nxa"
)
echo ProjectName: %ProjectName%

:: 返回到脚本目录
cd %WORKPATH%
cd ..

setlocal enabledelayedexpansion

set "source_folder=.\project\%OUTPUTNAME%"
set "target_folder=.\%UPDATENAME%"

type .\script\version.txt > .\script\update.hex 

%TOOL%\configParse.exe .\config.xml %1 >> .\script\update.hex
type %source_folder%\*.hex >> .\script\update.hex

:: 将输出文件命名为工程名字+日期+版本号
for /f %%i in (.\script\rename.txt) do set ver=%%i

for %%f in ("%source_folder%\*.hex") do (
    set "filename=%%~nf"
)

:: 创建目标文件夹
if not exist %target_folder% (
    md %target_folder% > NUL
    echo Created folder: %target_folder%
) else (
    echo Folder already exists: %target_folder%
)

ren %source_folder%\*.hex %filename%_%date:~2,2%%date:~5,2%%date:~8,2%_V%ver%.hex
move %source_folder%\*.hex %target_folder% > NUL

:: 返回到脚本目录
cd %WORKPATH%
cd ..

cd .\script

md5sum.exe update.hex > md5.txt
type .\md5.txt >> update.hex

for /f %%i in (rename.txt) do set ver=%%i

if not exist ..\%UPDATENAME%\%1_V%ver% (
    :: 创建目标文件夹
    md ..\%UPDATENAME%\%1_V%ver% > NUL
    echo Created folder: ..\%UPDATENAME%\%1_V%ver%
) else (
    echo Folder already exists: ..\%UPDATENAME%\%1_V%ver%
)

:: 输出升级文件
ren .\update.hex %1_V%ver%.hex
move %1_V%ver%.hex ..\%UPDATENAME%\%1_V%ver% > NUL

del md5.txt
del rename.txt
del version.txt
