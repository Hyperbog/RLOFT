cmake_minimum_required(VERSION 3.20)
project(RavenloftNativeFrameworkContracts LANGUAGES CXX)
# This standalone test tree deliberately does not build or rename DemoV1.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()
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

# Synthetic metadata only. This Python test never benchmarks a real GPU.
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
  add_test(NAME native_renderer_evidence_contract_tests
    COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_compare_renderer_candidates.py")
endif()
