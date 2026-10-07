cmake_minimum_required(VERSION 3.20)
project(RavenloftDemoV1 LANGUAGES CXX)

enable_testing()

add_library(rloft_boot_flow
    src/anm_cinematic.cpp
    src/boot_flow.cpp
    src/character_creation_flow.cpp
    src/character_creation_presenter.cpp
    src/game_source.cpp
    src/resource_assets.cpp
    src/startup_presenter.cpp
    src/voc_decoder.cpp
    src/xmidi.cpp
)
target_include_directories(rloft_boot_flow PUBLIC include)
target_compile_features(rloft_boot_flow PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(rloft_boot_flow PRIVATE Advapi32)
endif()

if(MSVC)
    target_compile_options(rloft_boot_flow PRIVATE /W4 /permissive-)
else()
    target_compile_options(rloft_boot_flow PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(boot_flow_tests tests/boot_flow_tests.cpp)
target_link_libraries(boot_flow_tests PRIVATE rloft_boot_flow)
add_test(NAME boot_flow_tests COMMAND boot_flow_tests)

add_executable(game_source_tests tests/game_source_tests.cpp)
target_link_libraries(game_source_tests PRIVATE rloft_boot_flow)
add_test(NAME game_source_tests COMMAND game_source_tests)

add_executable(presenter_tests tests/presenter_tests.cpp)
target_link_libraries(presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME presenter_tests COMMAND presenter_tests)

add_executable(voc_decoder_tests tests/voc_decoder_tests.cpp)
target_link_libraries(voc_decoder_tests PRIVATE rloft_boot_flow)
add_test(NAME voc_decoder_tests COMMAND voc_decoder_tests)

add_executable(xmidi_tests tests/xmidi_tests.cpp)
target_link_libraries(xmidi_tests PRIVATE rloft_boot_flow)
add_test(NAME xmidi_tests COMMAND xmidi_tests)

add_executable(character_flow_tests tests/character_flow_tests.cpp)
target_link_libraries(character_flow_tests PRIVATE rloft_boot_flow)
add_test(NAME character_flow_tests COMMAND character_flow_tests)

if(WIN32)
    add_executable(
        ravenloft_demo_v1 WIN32
        src/win32_audio.cpp
        src/win32_main.cpp
        src/win32_midi.cpp
    )
    target_link_libraries(
        ravenloft_demo_v1
        PRIVATE rloft_boot_flow User32 Gdi32 Shell32 Ole32 Advapi32 Winmm
    )
    target_compile_features(ravenloft_demo_v1 PRIVATE cxx_std_17)
    if(MSVC)
        target_compile_options(ravenloft_demo_v1 PRIVATE /W4 /permissive-)
    endif()
endif()
