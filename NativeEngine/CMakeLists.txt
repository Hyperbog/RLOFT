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
  src/native_lzss.cpp
  src/native_type6_texture.cpp
  src/native_raster_browser.cpp
  src/native_raster_colors.cpp
  src/native_palette_context.cpp
  src/native_bank_system.cpp
  src/native_world_system.cpp
  src/native_world_payload.cpp
  src/native_world_fingerprints.cpp
  src/native_world_runtime.cpp
  src/native_scene_view.cpp
  src/native_type7_scene_shape.cpp
  src/native_scene_instances.cpp
  src/native_sintab_raster.cpp
  src/native_type7_vga_dispatch.cpp
  src/native_scene_frame_ranges.cpp
  src/native_type6_column_sampler.cpp)
target_link_libraries(rloft_native_assets PUBLIC rloft_native_contracts)

function(native_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8 /UNDEBUG)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
  endif()
endfunction()
native_warnings(rloft_native_assets)

# Single executable for all 11 active and optional 2 legacy original RES
# archives. Source stays read-only; reports metadata JSON to stdout.
add_executable(rloft_native_bank_audit src/native_bank_audit_main.cpp)
target_link_libraries(rloft_native_bank_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_bank_audit)

add_executable(rloft_native_world_audit src/native_world_audit_main.cpp)
target_link_libraries(rloft_native_world_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_world_audit)

# Metadata-only verification: reconstruct every typed original world section,
# verify round-trip equality and emit independent-comparable SHA-256 digests.
add_executable(rloft_native_payload_audit src/native_world_payload_audit_main.cpp)
target_link_libraries(rloft_native_payload_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_payload_audit)

add_executable(rloft_native_world_runtime_audit src/native_world_runtime_audit_main.cpp)
target_link_libraries(rloft_native_world_runtime_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_world_runtime_audit)
# Original-topology-backed host first-person view PROBE (not DOS renderer).
add_executable(rloft_native_scene_probe src/native_scene_probe_main.cpp)
target_link_libraries(rloft_native_scene_probe PRIVATE rloft_native_assets)
native_warnings(rloft_native_scene_probe)

# Original RES3 EH7 geometries and INIT source-class source banks, no x86 execution.
add_executable(rloft_native_res3_scene_audit src/native_res3_scene_audit_main.cpp)
target_link_libraries(rloft_native_res3_scene_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_res3_scene_audit)

add_executable(rloft_native_sintab_raster_audit src/native_sintab_raster_audit_main.cpp)
target_link_libraries(rloft_native_sintab_raster_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_sintab_raster_audit)

# Original CODE.2 36B14/369D8 ordered VGA mask and EH7 descriptor tracing.
add_executable(rloft_native_type7_vga_audit src/native_type7_vga_audit_main.cpp)
target_link_libraries(rloft_native_type7_vga_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_type7_vga_audit)
# Source-only original authored family A/B/C scene-instance resolver.
# May run with the known 4234-count RES3 bank shape when original RES3 bytes
# are not locally available, and reports that reduced evidence explicitly.
add_executable(rloft_native_scene_instances_audit src/native_scene_instances_audit_main.cpp)
target_link_libraries(rloft_native_scene_instances_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_scene_instances_audit)

# Original-authored scene base, variant-stride, and EH7 body bounds.
add_executable(rloft_native_scene_frame_ranges_audit src/native_scene_frame_ranges_audit_main.cpp)
target_link_libraries(rloft_native_scene_frame_ranges_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_scene_frame_ranges_audit)

# CODE.2 338C8 portable fixed-point EH6 column rendering (not camera parity).
add_executable(rloft_native_type6_strip_audit src/native_type6_strip_audit_main.cpp)
target_link_libraries(rloft_native_type6_strip_audit PRIVATE rloft_native_assets)
native_warnings(rloft_native_type6_strip_audit)

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

add_executable(native_raster_colors_tests tests/native_raster_colors_tests.cpp)
target_link_libraries(native_raster_colors_tests PRIVATE rloft_native_assets)
native_warnings(native_raster_colors_tests)
add_test(NAME native_raster_colors_tests COMMAND native_raster_colors_tests)

add_executable(native_palette_context_tests tests/native_palette_context_tests.cpp)
target_link_libraries(native_palette_context_tests PRIVATE rloft_native_assets)
native_warnings(native_palette_context_tests)
add_test(NAME native_palette_context_tests COMMAND native_palette_context_tests)

add_executable(native_type6_texture_tests tests/native_type6_texture_tests.cpp)
target_link_libraries(native_type6_texture_tests PRIVATE rloft_native_assets)
native_warnings(native_type6_texture_tests)
add_test(NAME native_type6_texture_tests COMMAND native_type6_texture_tests)

add_executable(native_bank_system_tests tests/native_bank_system_tests.cpp)
target_link_libraries(native_bank_system_tests PRIVATE rloft_native_assets)
native_warnings(native_bank_system_tests)
add_test(NAME native_bank_system_tests COMMAND native_bank_system_tests)

add_executable(native_world_system_tests tests/native_world_system_tests.cpp)
target_link_libraries(native_world_system_tests PRIVATE rloft_native_assets)
native_warnings(native_world_system_tests)
add_test(NAME native_world_system_tests COMMAND native_world_system_tests)

add_executable(native_world_payload_tests tests/native_world_payload_tests.cpp)
target_link_libraries(native_world_payload_tests PRIVATE rloft_native_assets)
native_warnings(native_world_payload_tests)
add_test(NAME native_world_payload_tests COMMAND native_world_payload_tests)

add_executable(native_world_runtime_tests tests/native_world_runtime_tests.cpp)
target_link_libraries(native_world_runtime_tests PRIVATE rloft_native_assets)
native_warnings(native_world_runtime_tests)
add_test(NAME native_world_runtime_tests COMMAND native_world_runtime_tests)

add_executable(native_scene_view_tests tests/native_scene_view_tests.cpp)
target_link_libraries(native_scene_view_tests PRIVATE rloft_native_assets)
native_warnings(native_scene_view_tests)
add_test(NAME native_scene_view_tests COMMAND native_scene_view_tests)

add_executable(native_type7_scene_shape_tests tests/native_type7_scene_shape_tests.cpp)
target_link_libraries(native_type7_scene_shape_tests PRIVATE rloft_native_assets)
native_warnings(native_type7_scene_shape_tests)
add_test(NAME native_type7_scene_shape_tests COMMAND native_type7_scene_shape_tests)

add_executable(native_sintab_raster_tests tests/native_sintab_raster_tests.cpp)
target_link_libraries(native_sintab_raster_tests PRIVATE rloft_native_assets)
native_warnings(native_sintab_raster_tests)
add_test(NAME native_sintab_raster_tests COMMAND native_sintab_raster_tests)

add_executable(native_type7_vga_dispatch_tests tests/native_type7_vga_dispatch_tests.cpp)
target_link_libraries(native_type7_vga_dispatch_tests PRIVATE rloft_native_assets)
native_warnings(native_type7_vga_dispatch_tests)
add_test(NAME native_type7_vga_dispatch_tests COMMAND native_type7_vga_dispatch_tests)

add_executable(native_scene_instances_tests tests/native_scene_instances_tests.cpp)
target_link_libraries(native_scene_instances_tests PRIVATE rloft_native_assets)
native_warnings(native_scene_instances_tests)
add_test(NAME native_scene_instances_tests COMMAND native_scene_instances_tests)

add_executable(native_scene_frame_ranges_tests tests/native_scene_frame_ranges_tests.cpp)
target_link_libraries(native_scene_frame_ranges_tests PRIVATE rloft_native_assets)
native_warnings(native_scene_frame_ranges_tests)
add_test(NAME native_scene_frame_ranges_tests COMMAND native_scene_frame_ranges_tests)

add_executable(native_type6_column_sampler_tests tests/native_type6_column_sampler_tests.cpp)
target_link_libraries(native_type6_column_sampler_tests PRIVATE rloft_native_assets)
native_warnings(native_type6_column_sampler_tests)
add_test(NAME native_type6_column_sampler_tests COMMAND native_type6_column_sampler_tests)

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
