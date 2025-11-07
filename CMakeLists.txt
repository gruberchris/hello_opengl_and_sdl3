cmake_minimum_required(VERSION 3.10)
project(hello_opengl_and_sdl3)

set(CMAKE_CXX_STANDARD 17)

# Platform-specific OpenGL version
if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
    set(OPENGL_VERSION "4.1")
elseif(UNIX)
    set(OPENGL_VERSION "4.6")
elseif(WIN32)
    set(OPENGL_VERSION "4.6")
endif()

# Find SDL3
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

# Find OpenGL
find_package(OpenGL REQUIRED)

add_executable(hello_opengl_and_sdl3 main.cpp)

# Link libraries
target_link_libraries(hello_opengl_and_sdl3 PRIVATE SDL3::SDL3 OpenGL::GL)

if(APPLE)
    target_compile_definitions(hello_opengl_and_sdl3 PRIVATE USE_OPENGL_41)
endif()
