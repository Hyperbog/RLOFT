cmake_minimum_required(VERSION 3.20)
project(RavenloftNativeEngine LANGUAGES CXX)
# This standalone test tree deliberately does not build or rename DemoV1.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()
# Ship independent native Windows executables without a separate VC runtime.
if(WIN32 AND MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
add_library(rloft_native_contracts INTERFACE)
target_include_directories(rloft_native_contracts INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
add_executable(native_framework_contract_tests tests/framework_contract_tests.cpp)
target_link_libraries(native_framework_contract_tests PRIVATE rloft_native_contracts)
if(MSVC)
  target_compile_options(native_framework_contract_tests PRIVATE /W4 /UNDEBUG)
else()
  target_compile_options(native_framework_contract_tests PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
endif()
add_test(NAME native_framework_contract_tests COMMAND native_framework_contract_tests)

# Full Git checkout only: compile the real inherited media reader when available.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../DemoV1/src/game_source.cpp" AND
   EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../DemoV1/include/rloft/game_source.hpp")
# Integrates the adapter with the actual inherited reader, not a mock loader.
# This is still a read-only synthetic media fixture, not a GUI or game build.
add_executable(native_game_source_adapter_tests
  tests/game_source_adapter_tests.cpp
  ../DemoV1/src/game_source.cpp)
target_include_directories(native_game_source_adapter_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../DemoV1/include")
target_link_libraries(native_game_source_adapter_tests PRIVATE rloft_native_contracts)
# The inherited Windows GameSource probes the GOG install registry (Advapi32).
if(WIN32)
  target_link_libraries(native_game_source_adapter_tests PRIVATE Advapi32)
endif()
if(MSVC)
  target_compile_options(native_game_source_adapter_tests PRIVATE /W4 /UNDEBUG)
else()
  target_compile_options(native_game_source_adapter_tests PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
endif()
add_test(NAME native_game_source_adapter_tests COMMAND native_game_source_adapter_tests)

# Exercise original-source RES0 EH type-1 decoding and COLORS palette
# as a diagnostic cursor preview. Uses only synthetic test media.
add_executable(native_original_resource_preview_tests
  tests/original_resource_preview_tests.cpp
  ../DemoV1/src/game_source.cpp
  ../DemoV1/src/resource_assets.cpp)
target_include_directories(native_original_resource_preview_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../DemoV1/include")
target_link_libraries(native_original_resource_preview_tests PRIVATE
  rloft_native_contracts)
if(WIN32)
  target_link_libraries(native_original_resource_preview_tests PRIVATE Advapi32)
endif()
if(MSVC)
  target_compile_options(native_original_resource_preview_tests PRIVATE /W4 /utf-8 /UNDEBUG)
else()
  target_compile_options(native_original_resource_preview_tests PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
endif()
add_test(NAME native_original_resource_preview_tests COMMAND native_original_resource_preview_tests)


# Standalone preflight executable, distinct from the future playable engine.
add_executable(rloft_install_probe
  src/install_probe_main.cpp
  ../DemoV1/src/game_source.cpp)
target_include_directories(rloft_install_probe PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../DemoV1/include")
target_link_libraries(rloft_install_probe PRIVATE rloft_native_contracts)
if(WIN32)
  target_link_libraries(rloft_install_probe PRIVATE Advapi32)
endif()
set(RLOFT_NATIVE_SOURCE_SHA "local-unversioned" CACHE STRING
  "Exact Git checkout revision reported by standalone install probe")
target_compile_definitions(rloft_install_probe PRIVATE
  "RLOFT_NATIVE_SOURCE_SHA=\"${RLOFT_NATIVE_SOURCE_SHA}\"")
if(MSVC)
  target_compile_options(rloft_install_probe PRIVATE /W4 /UNDEBUG)
else()
  target_compile_options(rloft_install_probe PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
endif()
add_test(NAME native_install_probe_help COMMAND rloft_install_probe --help)

else()
  message(STATUS "NativeEngine: full GameSource source absent; adapter CTest NOT registered")
endif()

# CPU golden output for future GPU backend readback equality, not a DOS CRT.
add_executable(native_cpu_reference_presenter_tests tests/cpu_reference_presenter_tests.cpp)
target_link_libraries(native_cpu_reference_presenter_tests PRIVATE rloft_native_contracts)
if(MSVC)
  target_compile_options(native_cpu_reference_presenter_tests PRIVATE /W4 /UNDEBUG)
else()
  target_compile_options(native_cpu_reference_presenter_tests PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
endif()
add_test(NAME native_cpu_reference_presenter_tests COMMAND native_cpu_reference_presenter_tests)

# Real folder mount proof; no proprietary assets or external engine dependencies.
add_executable(native_disk_mod_mount_tests tests/disk_mod_mount_tests.cpp)
target_link_libraries(native_disk_mod_mount_tests PRIVATE rloft_native_contracts)
if(MSVC)
  target_compile_options(native_disk_mod_mount_tests PRIVATE /W4 /UNDEBUG)
else()
  target_compile_options(native_disk_mod_mount_tests PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
endif()
add_test(NAME native_disk_mod_mount_tests COMMAND native_disk_mod_mount_tests)

# Windows release QA requires Python for original-media and ZIP evidence
# fixture tests. Never silently ship an artifact with those checks skipped.
if(WIN32)
  find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
else()
  find_package(Python3 COMPONENTS Interpreter QUIET)
endif()
# Synthetic metadata only. This Python test never benchmarks a real GPU.
if(Python3_Interpreter_FOUND)
  add_test(NAME native_renderer_evidence_contract_tests
    COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_compare_renderer_candidates.py")
endif()

# Algorithm-only contracts; never enables modded original gameplay.
add_executable(native_mod_load_plan_tests tests/mod_load_plan_tests.cpp)
target_link_libraries(native_mod_load_plan_tests PRIVATE rloft_native_contracts)
add_executable(native_install_probe_contract_tests tests/install_probe_tests.cpp)
target_link_libraries(native_install_probe_contract_tests PRIVATE rloft_native_contracts)
foreach(t IN ITEMS native_mod_load_plan_tests native_install_probe_contract_tests)
  if(MSVC)
    target_compile_options(${t} PRIVATE /W4 /UNDEBUG)
  else()
    target_compile_options(${t} PRIVATE -Wall -Wextra -Wpedantic -UNDEBUG)
  endif()
  add_test(NAME ${t} COMMAND ${t})
endforeach()

# Synthetic loose/ISO9660 integration against actual linked GameSource.
if(TARGET rloft_install_probe AND Python3_Interpreter_FOUND)
  add_test(NAME native_install_probe_cli_fixture_tests
    COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_install_probe_cli.py"
      --executable "$<TARGET_FILE:rloft_install_probe>")
endif()

# Dependency-ordered transactional mod directory application: diagnostic only.
add_executable(native_mod_mount_plan_tests tests/mod_mount_plan_tests.cpp)
target_link_libraries(native_mod_mount_plan_tests PRIVATE rloft_native_contracts)
if(MSVC)
  target_compile_options(native_mod_mount_plan_tests PRIVATE /W4 /WX /UNDEBUG)
else()
  target_compile_options(native_mod_mount_plan_tests PRIVATE -Wall -Wextra -Wpedantic -Werror -UNDEBUG)
endif()
add_test(NAME native_mod_mount_plan_tests COMMAND native_mod_mount_plan_tests)

# Windows-first user-visible app. This is a NON-PLAYABLE install/media
# bootstrap and synthetic indexed-renderer diagnostic, not DemoV1 or the
# accepted full rloft_native.exe game. No SDL3/bgfx required for first MSVC.
if(WIN32)
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../DemoV1/src/game_source.cpp")
    message(FATAL_ERROR "Windows bootstrap requires exact inherited GameSource")
  endif()
  add_executable(rloft_native_bootstrap WIN32
    src/win32_bootstrap.cpp
    ../DemoV1/src/game_source.cpp
    ../DemoV1/src/resource_assets.cpp
    ../DemoV1/src/startup_presenter.cpp)
  target_include_directories(rloft_native_bootstrap PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../DemoV1/include")
  target_link_libraries(rloft_native_bootstrap PRIVATE
    rloft_native_contracts User32 Gdi32 Shell32 Ole32 Advapi32)
  target_compile_definitions(rloft_native_bootstrap PRIVATE
    "RLOFT_NATIVE_SOURCE_SHA=\"${RLOFT_NATIVE_SOURCE_SHA}\""
    UNICODE _UNICODE)
  if(MSVC)
    target_compile_options(rloft_native_bootstrap PRIVATE /W4 /permissive- /utf-8)
  endif()
  add_test(NAME native_win32_bootstrap_headless_contract
    COMMAND rloft_native_bootstrap --self-test)
  if(Python3_Interpreter_FOUND)
    # Reuse the original-media synthetic corpus against the actual Windows
    # GUI inventory code, without opening a desktop window in CI.
    add_test(NAME native_win32_bootstrap_original_media_contract
      COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_install_probe_cli.py"
        --executable "$<TARGET_FILE:rloft_install_probe>"
        --bootstrap "$<TARGET_FILE:rloft_native_bootstrap>")
  endif()
  # Package QA runs only under real Windows PowerShell; no Linux analogue.
  find_program(RLOFT_NATIVE_WINDOWS_POWERSHELL NAMES powershell.exe REQUIRED)
  add_test(NAME native_windows_package_integrity_contract
    COMMAND "${RLOFT_NATIVE_WINDOWS_POWERSHELL}"
      -NoProfile -ExecutionPolicy Bypass
      -File "${CMAKE_CURRENT_SOURCE_DIR}/Windows/Test-NativeEngine-WindowsPackage.ps1")
endif()
