# Neutral original CODE.1 character choice/race/class flow.
# No Win32/UI renderer, native engine, or DemoV1 dependency.
add_library(rloft_shared_character_creation STATIC
  src/character_creation_flow.cpp)
target_compile_features(rloft_shared_character_creation PUBLIC cxx_std_17)
target_include_directories(rloft_shared_character_creation PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
if(MSVC)
  target_compile_options(rloft_shared_character_creation PRIVATE /W4 /permissive- /utf-8)
else()
  target_compile_options(rloft_shared_character_creation PRIVATE -Wall -Wextra -Wpedantic)
endif()
