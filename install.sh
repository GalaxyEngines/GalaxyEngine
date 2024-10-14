#!/bin/bash
cd "$(pwd)/vcpkg/vcpkg" || exit

if [ ! -d "installed" ]; then
    echo "安装 vcpkg..."
    ./bootstrap-vcpkg.sh
else
    echo "vcpkg已安装，跳过安装。"
fi

vcpkg install glfw3 vulkan glm imgui nlohmann-json yaml-cpp
