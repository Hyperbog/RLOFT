cmake_minimum_required(VERSION 3.20)
project(RLOFT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(GLOB_RECURSE RLOFT_SOURCES CONFIGURE_DEPENDS
    "CODE1/*.cpp"
    "CODE2/*.cpp"
    "DATA/*.cpp"
    "ENGINE/*.cpp"
    "PORT/*.cpp"
)

# The source tree is intentionally flat. Older reconstructed files still use
# their historical include spellings, so generate forwarding headers only in
# the build directory. These compatibility folders are never committed.
set(RLOFT_COMPAT_INCLUDE "${CMAKE_BINARY_DIR}/compat_include")
file(MAKE_DIRECTORY
    "${RLOFT_COMPAT_INCLUDE}/games/ravenloft/game"
    "${RLOFT_COMPAT_INCLUDE}/games/ravenloft/data"
    "${RLOFT_COMPAT_INCLUDE}/engine/core"
    "${RLOFT_COMPAT_INCLUDE}/engine/render"
)

file(GLOB CODE1_HEADERS CONFIGURE_DEPENDS "CODE1/*.h")
foreach(header IN LISTS CODE1_HEADERS)
    get_filename_component(name "${header}" NAME)
    file(WRITE "${RLOFT_COMPAT_INCLUDE}/games/ravenloft/game/${name}"
         "#include \"${name}\"\n")
endforeach()

file(GLOB DATA_HEADERS CONFIGURE_DEPENDS "DATA/*.h")
foreach(header IN LISTS DATA_HEADERS)
    get_filename_component(name "${header}" NAME)
    file(WRITE "${RLOFT_COMPAT_INCLUDE}/games/ravenloft/data/${name}"
         "#include \"${name}\"\n")
endforeach()

file(GLOB ENGINE_HEADERS CONFIGURE_DEPENDS "ENGINE/*.h")
foreach(header IN LISTS ENGINE_HEADERS)
    get_filename_component(name "${header}" NAME)
    if(name STREQUAL "FileSystem.h")
        set(subdir "engine/core")
    else()
        set(subdir "engine/render")
    endif()
    file(WRITE "${RLOFT_COMPAT_INCLUDE}/${subdir}/${name}"
         "#include \"${name}\"\n")
endforeach()

file(GLOB PORT_HEADERS CONFIGURE_DEPENDS "PORT/*.h")
foreach(header IN LISTS PORT_HEADERS)
    get_filename_component(name "${header}" NAME)
    file(WRITE "${RLOFT_COMPAT_INCLUDE}/games/ravenloft/game/${name}"
         "#include \"${name}\"\n")
endforeach()

add_library(rloft_port STATIC ${RLOFT_SOURCES})
target_include_directories(rloft_port PUBLIC
    CODE1
    CODE2
    DATA
    ENGINE
    PORT
    "${RLOFT_COMPAT_INCLUDE}"
)
