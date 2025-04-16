@echo off
setlocal

:: ==== 架构切换，支持 x64 / x86 ====
set ARCHITECTURE=x64
:: set ARCHITECTURE=x86

:: ==== 加载 VS 环境变量 ====
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=%ARCHITECTURE%

:: ==== 编译参数 ====
set SRC=%1
set OUT=%~dpn1.exe

echo Compiling %SRC% ...
cl /nologo /EHsc /Zi /Fe:%OUT% %SRC%
