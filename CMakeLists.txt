cmake_minimum_required(VERSION 3.20)
project(RLOFT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(GLOB_RECURSE RLOFT_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)

add_library(rloft_port STATIC ${RLOFT_SOURCES})
target_include_directories(rloft_port PUBLIC src)
