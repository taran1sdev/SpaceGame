cmake_minimum_required(VERSION 3.10)
project(BlackHole)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

file(GLOB SRC_FILES "src/*.cpp")

add_executable(blackhole ${SRC_FILES})

target_include_directories(blackhole PUBLIC external)

target_link_libraries(blackhole
    glad
    glfw
    OpenGL::GL
)
