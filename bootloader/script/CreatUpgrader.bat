@echo off
set WORKPATH=%~dp0
set DISK=%~d0
pushd %WORKPATH%
%DISK%
cd %WORKPATH%
cd ..

set TOOL=".\script\s3GenDataArray.exe"
set targetFolder=..\application\user\upgrader

copy .\project\Objects\Ccu.axf .\bootloader.tmp >NUL
fromelf --m32combined -o .\bootloader.mot .\bootloader.tmp

if not exist "%targetFolder%\" (
    :: 创建目标文件夹
    md "%targetFolder%" > NUL
    echo Created folder: %targetFolder%
) else (
    echo Folder already exists: %targetFolder%
)

type .\script\head.txt > "%targetFolder%\upgrader.c"
echo. >> "%targetFolder%\upgrader.c"

%TOOL% a "..\bootloader.mot" >> "%targetFolder%\upgrader.c"
%TOOL% f "..\bootloader.mot" > ..\application\script\bootloader.sct

type ..\application\script\Ccu.sct > ..\application\script\project.sct
type ..\application\script\bootloader.sct >> ..\application\script\project.sct

del ..\application\script\bootloader.sct
del *.mot
del *.tmp
