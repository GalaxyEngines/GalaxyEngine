project(GERender)

file(GLOB_RECURSE CORE_SOURCE src/*.cpp src/*.h)

add_library(GERender SHARED ${CORE_SOURCE}
        src/GraphicsBase.cpp
        src/PhysicalDeivce.cpp
        src/PhysicalDeivce.h
        src/SwapChain.cpp
        src/SwapChain.h
        src/LogicalDevice.cpp
        src/LogicalDevice.h
        src/WindowSurface.cpp
        src/WindowSurface.h
        include/b.cpp)

find_package(Vulkan REQUIRED)

target_link_libraries(GERender PRIVATE Vulkan::Vulkan)