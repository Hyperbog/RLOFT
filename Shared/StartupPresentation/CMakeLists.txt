# Exact original CODE.2 startup logos/menu presentation, independent
# of DemoV1's installation loader and NativeEngine's original-media API.
add_library(rloft_shared_startup_presentation STATIC
  src/startup_presenter.cpp
  src/anm_cinematic.cpp)
target_compile_features(rloft_shared_startup_presentation PUBLIC cxx_std_17)
target_include_directories(rloft_shared_startup_presentation PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
if(MSVC)
  target_compile_options(rloft_shared_startup_presentation PRIVATE /W4 /permissive- /utf-8)
else()
  target_compile_options(rloft_shared_startup_presentation PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(shared_original_startup_presenter_tests
  tests/startup_presenter_tests.cpp)
target_link_libraries(shared_original_startup_presenter_tests PRIVATE
  rloft_shared_startup_presentation)
add_test(NAME shared_original_startup_presenter_tests
  COMMAND shared_original_startup_presenter_tests)
