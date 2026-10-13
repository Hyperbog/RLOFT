# Shared source-traced original R.EXE/CODE.2/CODE.1 boot state machine.
# This module is neutral: it depends on neither DemoV1 nor NativeEngine.
# The executable front-end (cinematics, user input, CHARS) remains
# separately owned and must not be misrepresented as integrated here.
add_library(rloft_shared_boot STATIC src/boot_flow.cpp)
target_compile_features(rloft_shared_boot PUBLIC cxx_std_17)
target_include_directories(rloft_shared_boot PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
if(MSVC)
  target_compile_options(rloft_shared_boot PRIVATE /W4 /permissive- /utf-8)
else()
  target_compile_options(rloft_shared_boot PRIVATE -Wall -Wextra -Wpedantic)
endif()
