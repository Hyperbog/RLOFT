cmake_minimum_required(VERSION 3.20)
project(RavenloftNativeEngine LANGUAGES CXX)

# Independent replacement game engine. Original files are DATA, not code.
# Production build MUST NOT compile or link DemoV1 gameplay, presenters or
# original DOS executables. File-format findings remain reference material.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32 AND MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
enable_testing()

add_library(rloft_native_contracts INTERFACE)
target_include_directories(rloft_native_contracts INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(rloft_native_assets STATIC
  src/native_media.cpp
  src/native_assets.cpp
  src/native_raster.cpp
  src/native_raster_browser.cpp)
target_link_libraries(rloft_native_assets PUBLIC rloft_native_contracts)

function(native_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8 /UNDEBUG)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
  endif()
endfunction()
native_warnings(rloft_native_assets)

set(RLOFT_NATIVE_SOURCE_SHA "local-unversioned" CACHE STRING
  "Exact source revision of independent asset engine")
if(WIN32)
  add_executable(rloft_native WIN32 src/win32_native_assets.cpp)
  target_link_libraries(rloft_native PRIVATE
    rloft_native_assets User32 Gdi32 Shell32 Ole32)
  target_compile_definitions(rloft_native PRIVATE UNICODE _UNICODE
    "RLOFT_NATIVE_SOURCE_SHA=\"${RLOFT_NATIVE_SOURCE_SHA}\"")
  native_warnings(rloft_native)
endif()

function(native_test_target name file)
  add_executable(${name} "tests/${file}")
  target_link_libraries(${name} PRIVATE rloft_native_contracts)
  native_warnings(${name})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

native_test_target(native_framework_contract_tests framework_contract_tests.cpp)
native_test_target(native_cpu_reference_presenter_tests cpu_reference_presenter_tests.cpp)
native_test_target(native_disk_mod_mount_tests disk_mod_mount_tests.cpp)
native_test_target(native_mod_load_plan_tests mod_load_plan_tests.cpp)
native_test_target(native_install_probe_contract_tests install_probe_tests.cpp)
native_test_target(native_mod_mount_plan_tests mod_mount_plan_tests.cpp)

add_executable(native_asset_media_tests tests/native_asset_media_tests.cpp)
target_link_libraries(native_asset_media_tests PRIVATE rloft_native_assets)
native_warnings(native_asset_media_tests)
add_test(NAME native_asset_media_tests COMMAND native_asset_media_tests)

add_executable(native_raster_tests tests/native_raster_tests.cpp)
target_link_libraries(native_raster_tests PRIVATE rloft_native_assets)
native_warnings(native_raster_tests)
add_test(NAME native_raster_tests COMMAND native_raster_tests)

add_executable(native_raster_browser_tests tests/native_raster_browser_tests.cpp)
target_link_libraries(native_raster_browser_tests PRIVATE rloft_native_assets)
native_warnings(native_raster_browser_tests)
add_test(NAME native_raster_browser_tests COMMAND native_raster_browser_tests)

# Native Windows asset UI built as its own EXE, never a renamed demo.
if(WIN32)
  add_test(NAME native_asset_engine_window_target
    COMMAND "${CMAKE_COMMAND}" -E echo "Independent Windows native asset engine compiled")
  set_tests_properties(native_asset_engine_window_target PROPERTIES
    REQUIRED_FILES "$<TARGET_FILE:rloft_native>")
endif()

if(WIN32)
  find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
else()
  find_package(Python3 COMPONENTS Interpreter QUIET)
endif()
if(Python3_Interpreter_FOUND)
  add_test(NAME native_renderer_evidence_contract_tests
    COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_compare_renderer_candidates.py")
endif()
