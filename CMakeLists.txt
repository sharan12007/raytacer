cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE
    "C:/Program Files/Microsoft Visual Studio/18/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING ""
)

project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

# Your ray tracer executable
add_executable(renderer
    main.cpp
    window.cpp
 "camera.h")

target_link_libraries(renderer PRIVATE
    glm::glm
    glfw
    glad::glad
)