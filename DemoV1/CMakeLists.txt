cmake_minimum_required(VERSION 3.20)
project(RavenloftDemoV1 LANGUAGES CXX)

# Keep the Windows release self-contained: do not require a separately
# installed Microsoft Visual C++ runtime on the user's machine.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

enable_testing()

add_library(rloft_boot_flow
    src/anm_cinematic.cpp
    src/boot_flow.cpp
    src/character_creation_flow.cpp
    src/character_creation_presenter.cpp
    src/character_palette_remap.cpp
    src/character_stats.cpp
    src/character_transfer.cpp
    src/exploration_state.cpp
    src/exploration_session.cpp
    src/exploration_frame.cpp
    src/level_display.cpp
    src/party_presentation.cpp
    src/top_edge_menu.cpp
    src/exploration_ui.cpp
    src/exploration_world.cpp
    src/exploration_collision.cpp
    src/environment_view.cpp
    src/environment_projection.cpp
    src/environment_presenter.cpp
    src/automap_presentation.cpp
    src/automap_annotations.cpp
    src/surface_texture.cpp
    src/game_source.cpp
    src/resource_assets.cpp
    src/startup_presenter.cpp
    src/sound_config.cpp
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

add_executable(exploration_state_tests tests/exploration_state_tests.cpp)
target_link_libraries(exploration_state_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_state_tests COMMAND exploration_state_tests)

add_executable(exploration_session_tests tests/exploration_session_tests.cpp)
target_link_libraries(exploration_session_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_session_tests COMMAND exploration_session_tests)

add_executable(exploration_frame_tests tests/exploration_frame_tests.cpp)
target_link_libraries(exploration_frame_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_frame_tests COMMAND exploration_frame_tests)

add_executable(level_display_tests tests/level_display_tests.cpp)
target_link_libraries(level_display_tests PRIVATE rloft_boot_flow)
add_test(NAME level_display_tests COMMAND level_display_tests)

add_executable(party_presentation_tests tests/party_presentation_tests.cpp)
target_link_libraries(party_presentation_tests PRIVATE rloft_boot_flow)
add_test(NAME party_presentation_tests COMMAND party_presentation_tests)

add_executable(top_edge_menu_tests tests/top_edge_menu_tests.cpp)
target_link_libraries(top_edge_menu_tests PRIVATE rloft_boot_flow)
add_test(NAME top_edge_menu_tests COMMAND top_edge_menu_tests)

add_executable(exploration_ui_tests tests/exploration_ui_tests.cpp)
target_link_libraries(exploration_ui_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_ui_tests COMMAND exploration_ui_tests)

add_executable(exploration_world_tests tests/exploration_world_tests.cpp)
target_link_libraries(exploration_world_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_world_tests COMMAND exploration_world_tests)

add_executable(exploration_collision_tests tests/exploration_collision_tests.cpp)
target_link_libraries(exploration_collision_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_collision_tests COMMAND exploration_collision_tests)

add_executable(environment_view_tests tests/environment_view_tests.cpp)
target_link_libraries(environment_view_tests PRIVATE rloft_boot_flow)
add_test(NAME environment_view_tests COMMAND environment_view_tests)

add_executable(environment_projection_tests tests/environment_projection_tests.cpp)
target_link_libraries(environment_projection_tests PRIVATE rloft_boot_flow)
add_test(NAME environment_projection_tests COMMAND environment_projection_tests)

add_executable(environment_presenter_tests tests/environment_presenter_tests.cpp)
target_link_libraries(environment_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME environment_presenter_tests COMMAND environment_presenter_tests)

add_executable(automap_presentation_tests tests/automap_presentation_tests.cpp)
target_link_libraries(automap_presentation_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_presentation_tests COMMAND automap_presentation_tests)

add_executable(automap_annotations_tests tests/automap_annotations_tests.cpp)
target_link_libraries(automap_annotations_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_annotations_tests COMMAND automap_annotations_tests)

add_executable(surface_texture_tests tests/surface_texture_tests.cpp)
target_link_libraries(surface_texture_tests PRIVATE rloft_boot_flow)
add_test(NAME surface_texture_tests COMMAND surface_texture_tests)

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

add_executable(character_stats_tests tests/character_stats_tests.cpp)
target_link_libraries(character_stats_tests PRIVATE rloft_boot_flow)
add_test(NAME character_stats_tests COMMAND character_stats_tests)

add_executable(character_transfer_tests tests/character_transfer_tests.cpp)
target_link_libraries(character_transfer_tests PRIVATE rloft_boot_flow)
add_test(NAME character_transfer_tests COMMAND character_transfer_tests)

add_executable(character_palette_remap_tests tests/character_palette_remap_tests.cpp)
target_link_libraries(character_palette_remap_tests PRIVATE rloft_boot_flow)
add_test(NAME character_palette_remap_tests COMMAND character_palette_remap_tests)

add_executable(sound_config_tests tests/sound_config_tests.cpp)
target_link_libraries(sound_config_tests PRIVATE rloft_boot_flow)
add_test(NAME sound_config_tests COMMAND sound_config_tests)

set(RLOFT_TEST_TARGETS
    boot_flow_tests
    exploration_state_tests
    exploration_session_tests
    exploration_frame_tests
    level_display_tests
    party_presentation_tests
    top_edge_menu_tests
    exploration_ui_tests
    exploration_world_tests
    exploration_collision_tests
    environment_view_tests
    environment_projection_tests
    environment_presenter_tests
    automap_presentation_tests
    automap_annotations_tests
    surface_texture_tests
    game_source_tests
    presenter_tests
    voc_decoder_tests
    xmidi_tests
    character_flow_tests
    character_stats_tests
    character_transfer_tests
    character_palette_remap_tests
    sound_config_tests
)
if(MSVC)
    foreach(test_target IN LISTS RLOFT_TEST_TARGETS)
        target_compile_options(${test_target} PRIVATE /UNDEBUG)
    endforeach()
else()
    foreach(test_target IN LISTS RLOFT_TEST_TARGETS)
        target_compile_options(${test_target} PRIVATE -UNDEBUG)
    endforeach()
endif()

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
