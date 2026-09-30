cmake_minimum_required(VERSION 3.20)
project(RLOFT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(GLOB CODE1_SOURCES CONFIGURE_DEPENDS
    "CODE1/*.cpp"
)

file(GLOB CODE2_SOURCES CONFIGURE_DEPENDS
    "CODE2/*.cpp"
)

add_library(rloft_code1 STATIC ${CODE1_SOURCES})
target_include_directories(rloft_code1 PUBLIC CODE1)

add_library(rloft_code2 STATIC ${CODE2_SOURCES})
target_include_directories(rloft_code2 PUBLIC CODE2)
