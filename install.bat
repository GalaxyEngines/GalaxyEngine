@echo off
setlocal

cd "%~dp0vcpkg\vcpkg" || exit /b

if not exist "installed" (
    echo 安装 vcpkg...
    call bootstrap-vcpkg.bat
) else (
    echo vcpkg 已安装，跳过安装。
)

call vcpkg install glfw3 vulkan glm imgui yaml-cpp openal-soft bullet3

endlocal