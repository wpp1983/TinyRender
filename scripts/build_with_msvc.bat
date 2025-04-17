@echo off
rem 切到 UTF-8 控制台，若你脚本已保存为 ANSI 可删掉这行
chcp 65001>nul


set "PROJECT_DIR=%~dp0.."
set BUILD_DIR=%PROJECT_DIR%\build-ninja

echo [INFO] Running ninja compile...
meson compile -C "%BUILD_DIR%" || (
    echo [ERROR] build failed.
    exit /b 1
)

echo [SUCCESS] Build completed!
pause
