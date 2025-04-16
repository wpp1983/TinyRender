@echo off
setlocal

REM 设置项目目录
set PROJECT_DIR=%~dp0

REM 构建目录名称
set NINJA_DIR=build-ninja
set VS2022_DIR=build-vs2022

REM === 是否清除旧目录 ===
set DELETE_EXISTING=1

REM 清理并创建 Ninja 构建目录
if exist %NINJA_DIR% (
    if %DELETE_EXISTING%==1 (
        echo [Meson] Deleting existing Ninja build directory...
        rmdir /S /Q %NINJA_DIR%
    )
)

echo [Meson] Creating Ninja build directory...
meson setup %NINJA_DIR% %PROJECT_DIR% --backend ninja || goto :error

REM 清理并创建 VS2022 构建目录
if exist %VS2022_DIR% (
    if %DELETE_EXISTING%==1 (
        echo [Meson] Deleting existing VS2022 build directory...
        rmdir /S /Q %VS2022_DIR%
    )
)

echo [Meson] Creating VS2022 build directory...
meson setup %VS2022_DIR% %PROJECT_DIR% --backend vs2022 || goto :error

echo.
echo [Done] Build directories created successfully.
goto :eof

:error
echo [Error] Meson setup failed.
exit /b 1
