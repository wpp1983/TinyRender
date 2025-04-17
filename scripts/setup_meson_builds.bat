@echo off
setlocal



REM 设置项目目录
set PROJECT_DIR=%~dp0..

REM 构建目录名称
set NINJA_DIR=%PROJECT_DIR%\build-ninja
set VS2022_DIR=%PROJECT_DIR%\build-vs2022

REM === 是否清除旧目录 ===
set DELETE_EXISTING=1



echo [Meson] Creating Ninja build directory...
meson setup %NINJA_DIR% %PROJECT_DIR% --backend ninja  --vsenv --reconfigure  || goto :error



echo [Meson] Creating VS2022 build directory...
meson setup %VS2022_DIR% %PROJECT_DIR% --backend vs2022  --vsenv --reconfigure  || goto :error

echo.
echo [Done] Build directories created successfully.
goto :eof

:error
echo [Error] Meson setup failed.
exit /b 1
