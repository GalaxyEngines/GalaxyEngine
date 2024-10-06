@echo off
setlocal

rem 进入 vcpkg 目录
cd "%~dp0vcpkg" || exit /b

rem 检查 vcpkg 是否已安装
if not exist "installed" (
    echo 安装 vcpkg...
    call bootstrap-vcpkg.bat
) else (
    echo vcpkg 已安装，跳过安装。
)

rem 安装依赖项
call vcpkg install glfw3 vulkan glm

endlocal