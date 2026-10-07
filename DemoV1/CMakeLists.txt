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
    src/character_sheet.cpp
    src/character_transfer.cpp
    src/exploration_state.cpp
    src/exploration_session.cpp
    src/exploration_frame.cpp
    src/exploration_scene.cpp
    src/level_display.cpp
    src/party_presentation.cpp
    src/object_catalog.cpp
    src/inventory_screen.cpp
    src/party_hud.cpp
    src/top_edge_menu.cpp
    src/top_edge_menu_presenter.cpp
    src/exploration_ui.cpp
    src/exploration_hud_presenter.cpp
    src/exploration_presenter.cpp
    src/exploration_world.cpp
    src/exploration_collision.cpp
    src/exploration_motion.cpp
    src/exploration_legacy_input.cpp
    src/exploration_locomotion.cpp
    src/exploration_runtime_motion.cpp
    src/exploration_runtime.cpp
    src/environment_view.cpp
    src/view_sweep_record.cpp
    src/view_sweep_setup.cpp
    src/view_sweep_traversal.cpp
    src/view_sweep_runs.cpp
    src/fixed_projection.cpp
    src/renderer_tables.cpp
    src/micro_rasterizer_layout.cpp
    src/micro_rasterizer_plan.cpp
    src/environment_projection.cpp
    src/environment_presenter.cpp
    src/automap_presentation.cpp
    src/automap_annotations.cpp
    src/automap_viewport_presenter.cpp
    src/automap_screen_state.cpp
    src/automap_controls.cpp
    src/surface_texture.cpp
    src/surface_column_scaler.cpp
    src/surface_shading.cpp
    src/surface_fill.cpp
    src/surface_wall_presenter.cpp
    src/surface_prepass.cpp
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

add_executable(exploration_scene_tests tests/exploration_scene_tests.cpp)
target_link_libraries(exploration_scene_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_scene_tests COMMAND exploration_scene_tests)

add_executable(level_display_tests tests/level_display_tests.cpp)
target_link_libraries(level_display_tests PRIVATE rloft_boot_flow)
add_test(NAME level_display_tests COMMAND level_display_tests)

add_executable(party_presentation_tests tests/party_presentation_tests.cpp)
target_link_libraries(party_presentation_tests PRIVATE rloft_boot_flow)
add_test(NAME party_presentation_tests COMMAND party_presentation_tests)

add_executable(object_catalog_tests tests/object_catalog_tests.cpp)
target_link_libraries(object_catalog_tests PRIVATE rloft_boot_flow)
add_test(NAME object_catalog_tests COMMAND object_catalog_tests)

add_executable(inventory_screen_tests tests/inventory_screen_tests.cpp)
target_link_libraries(inventory_screen_tests PRIVATE rloft_boot_flow)
add_test(NAME inventory_screen_tests COMMAND inventory_screen_tests)

add_executable(party_hud_tests tests/party_hud_tests.cpp)
target_link_libraries(party_hud_tests PRIVATE rloft_boot_flow)
add_test(NAME party_hud_tests COMMAND party_hud_tests)

add_executable(top_edge_menu_tests tests/top_edge_menu_tests.cpp)
target_link_libraries(top_edge_menu_tests PRIVATE rloft_boot_flow)
add_test(NAME top_edge_menu_tests COMMAND top_edge_menu_tests)

add_executable(top_edge_menu_presenter_tests tests/top_edge_menu_presenter_tests.cpp)
target_link_libraries(top_edge_menu_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME top_edge_menu_presenter_tests COMMAND top_edge_menu_presenter_tests)

add_executable(exploration_ui_tests tests/exploration_ui_tests.cpp)
target_link_libraries(exploration_ui_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_ui_tests COMMAND exploration_ui_tests)

add_executable(exploration_hud_presenter_tests tests/exploration_hud_presenter_tests.cpp)
target_link_libraries(exploration_hud_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_hud_presenter_tests COMMAND exploration_hud_presenter_tests)

add_executable(exploration_presenter_tests tests/exploration_presenter_tests.cpp)
target_link_libraries(exploration_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_presenter_tests COMMAND exploration_presenter_tests)

add_executable(exploration_world_tests tests/exploration_world_tests.cpp)
target_link_libraries(exploration_world_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_world_tests COMMAND exploration_world_tests)

add_executable(exploration_collision_tests tests/exploration_collision_tests.cpp)
target_link_libraries(exploration_collision_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_collision_tests COMMAND exploration_collision_tests)

add_executable(exploration_motion_tests tests/exploration_motion_tests.cpp)
target_link_libraries(exploration_motion_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_motion_tests COMMAND exploration_motion_tests)

add_executable(exploration_legacy_input_tests tests/exploration_legacy_input_tests.cpp)
target_link_libraries(exploration_legacy_input_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_legacy_input_tests COMMAND exploration_legacy_input_tests)

add_executable(exploration_locomotion_tests tests/exploration_locomotion_tests.cpp)
target_link_libraries(exploration_locomotion_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_locomotion_tests COMMAND exploration_locomotion_tests)

add_executable(exploration_runtime_motion_tests tests/exploration_runtime_motion_tests.cpp)
target_link_libraries(exploration_runtime_motion_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_runtime_motion_tests COMMAND exploration_runtime_motion_tests)

add_executable(exploration_runtime_tests tests/exploration_runtime_tests.cpp)
target_link_libraries(exploration_runtime_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_runtime_tests COMMAND exploration_runtime_tests)

add_executable(environment_view_tests tests/environment_view_tests.cpp)
target_link_libraries(environment_view_tests PRIVATE rloft_boot_flow)
add_test(NAME environment_view_tests COMMAND environment_view_tests)

add_executable(view_sweep_record_tests tests/view_sweep_record_tests.cpp)
target_link_libraries(view_sweep_record_tests PRIVATE rloft_boot_flow)
add_test(NAME view_sweep_record_tests COMMAND view_sweep_record_tests)

add_executable(view_sweep_setup_tests tests/view_sweep_setup_tests.cpp)
target_link_libraries(view_sweep_setup_tests PRIVATE rloft_boot_flow)
add_test(NAME view_sweep_setup_tests COMMAND view_sweep_setup_tests)

add_executable(view_sweep_traversal_tests tests/view_sweep_traversal_tests.cpp)
target_link_libraries(view_sweep_traversal_tests PRIVATE rloft_boot_flow)
add_test(NAME view_sweep_traversal_tests COMMAND view_sweep_traversal_tests)

add_executable(view_sweep_runs_tests tests/view_sweep_runs_tests.cpp)
target_link_libraries(view_sweep_runs_tests PRIVATE rloft_boot_flow)
add_test(NAME view_sweep_runs_tests COMMAND view_sweep_runs_tests)

add_executable(fixed_projection_tests tests/fixed_projection_tests.cpp)
target_link_libraries(fixed_projection_tests PRIVATE rloft_boot_flow)
add_test(NAME fixed_projection_tests COMMAND fixed_projection_tests)

add_executable(renderer_tables_tests tests/renderer_tables_tests.cpp)
target_link_libraries(renderer_tables_tests PRIVATE rloft_boot_flow)
add_test(NAME renderer_tables_tests COMMAND renderer_tables_tests)

add_executable(micro_rasterizer_layout_tests tests/micro_rasterizer_layout_tests.cpp)
target_link_libraries(micro_rasterizer_layout_tests PRIVATE rloft_boot_flow)
add_test(NAME micro_rasterizer_layout_tests COMMAND micro_rasterizer_layout_tests)

add_executable(micro_rasterizer_plan_tests tests/micro_rasterizer_plan_tests.cpp)
target_link_libraries(micro_rasterizer_plan_tests PRIVATE rloft_boot_flow)
add_test(NAME micro_rasterizer_plan_tests COMMAND micro_rasterizer_plan_tests)

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

add_executable(automap_viewport_presenter_tests tests/automap_viewport_presenter_tests.cpp)
target_link_libraries(automap_viewport_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_viewport_presenter_tests COMMAND automap_viewport_presenter_tests)

add_executable(automap_screen_state_tests tests/automap_screen_state_tests.cpp)
target_link_libraries(automap_screen_state_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_screen_state_tests COMMAND automap_screen_state_tests)

add_executable(automap_controls_tests tests/automap_controls_tests.cpp)
target_link_libraries(automap_controls_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_controls_tests COMMAND automap_controls_tests)

add_executable(surface_texture_tests tests/surface_texture_tests.cpp)
target_link_libraries(surface_texture_tests PRIVATE rloft_boot_flow)
add_test(NAME surface_texture_tests COMMAND surface_texture_tests)

add_executable(surface_column_scaler_tests tests/surface_column_scaler_tests.cpp)
target_link_libraries(surface_column_scaler_tests PRIVATE rloft_boot_flow)
add_test(NAME surface_column_scaler_tests COMMAND surface_column_scaler_tests)

add_executable(surface_shading_tests tests/surface_shading_tests.cpp)
target_link_libraries(surface_shading_tests PRIVATE rloft_boot_flow)
add_test(NAME surface_shading_tests COMMAND surface_shading_tests)

add_executable(surface_fill_tests tests/surface_fill_tests.cpp)
target_link_libraries(surface_fill_tests PRIVATE rloft_boot_flow)
add_test(NAME surface_fill_tests COMMAND surface_fill_tests)

add_executable(surface_wall_presenter_tests tests/surface_wall_presenter_tests.cpp)
target_link_libraries(surface_wall_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME surface_wall_presenter_tests COMMAND surface_wall_presenter_tests)

add_executable(surface_prepass_tests tests/surface_prepass_tests.cpp)
target_link_libraries(surface_prepass_tests PRIVATE rloft_boot_flow)
add_test(NAME surface_prepass_tests COMMAND surface_prepass_tests)

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

add_executable(character_sheet_tests tests/character_sheet_tests.cpp)
target_link_libraries(character_sheet_tests PRIVATE rloft_boot_flow)
add_test(NAME character_sheet_tests COMMAND character_sheet_tests)

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
    exploration_scene_tests
    level_display_tests
    party_presentation_tests
    object_catalog_tests
    inventory_screen_tests
    party_hud_tests
    top_edge_menu_tests
    top_edge_menu_presenter_tests
    exploration_ui_tests
    exploration_hud_presenter_tests
    exploration_presenter_tests
    exploration_world_tests
    exploration_collision_tests
    exploration_motion_tests
    exploration_legacy_input_tests
    exploration_locomotion_tests
    exploration_runtime_motion_tests
    exploration_runtime_tests
    environment_view_tests
    view_sweep_record_tests
    view_sweep_setup_tests
    view_sweep_traversal_tests
    view_sweep_runs_tests
    fixed_projection_tests
    renderer_tables_tests
    micro_rasterizer_layout_tests
    micro_rasterizer_plan_tests
    environment_projection_tests
    environment_presenter_tests
    automap_presentation_tests
    automap_annotations_tests
    automap_viewport_presenter_tests
    automap_screen_state_tests
    automap_controls_tests
    surface_texture_tests
    surface_column_scaler_tests
    surface_shading_tests
    surface_fill_tests
    surface_wall_presenter_tests
    surface_prepass_tests
    game_source_tests
    presenter_tests
    voc_decoder_tests
    xmidi_tests
    character_flow_tests
    character_stats_tests
    character_sheet_tests
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
