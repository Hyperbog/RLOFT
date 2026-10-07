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
    src/assassin_relic_transition.cpp
    src/boot_flow.cpp
    src/character_creation_flow.cpp
    src/character_creation_presenter.cpp
    src/character_palette_remap.cpp
    src/character_stats.cpp
    src/character_sheet.cpp
    src/character_modal_stats.cpp
    src/character_transfer.cpp
    src/exploration_state.cpp
    src/exploration_session.cpp
    src/exploration_frame.cpp
    src/exploration_scene.cpp
    src/level_display.cpp
    src/party_presentation.cpp
    src/object_catalog.cpp
    src/npc_catalog.cpp
    src/level22_story.cpp
    src/level22_encounter.cpp
    src/level_world_state.cpp
    src/inventory_screen.cpp
    src/inventory_runtime.cpp
    src/party_hud.cpp
    src/party_panel_controls.cpp
    src/party_modal_presenter.cpp
    src/top_edge_menu.cpp
    src/top_edge_menu_presenter.cpp
    src/quit_confirm.cpp
    src/options_menu.cpp
    src/exploration_ui.cpp
    src/exploration_hud_presenter.cpp
    src/exploration_presenter.cpp
    src/exploration_world.cpp
    src/exploration_collision.cpp
    src/exploration_motion.cpp
    src/exploration_legacy_input.cpp
    src/exploration_hit_regions.cpp
    src/exploration_locomotion.cpp
    src/exploration_runtime_motion.cpp
    src/exploration_runtime.cpp
    src/environment_view.cpp
    src/view_sweep_record.cpp
    src/view_sweep_setup.cpp
    src/view_sweep_traversal.cpp
    src/view_sweep_runs.cpp
    src/fixed_projection.cpp
    src/first_person_entity_projection.cpp
    src/npc_scene.cpp
    src/loose_item_scene.cpp
    src/scene_shape_bounds.cpp
    src/scene_shape_selector.cpp
    src/scene_source_banks.cpp
    src/world_scene_record.cpp
    src/world_scene_selection.cpp
    src/renderer_tables.cpp
    src/micro_rasterizer_layout.cpp
    src/micro_rasterizer_plan.cpp
    src/environment_projection.cpp
    src/environment_presenter.cpp
    src/automap_presentation.cpp
    src/automap_annotations.cpp
    src/automap_annotation_presenter.cpp
    src/automap_viewport_presenter.cpp
    src/automap_screen_state.cpp
    src/automap_controls.cpp
    src/automap_modal.cpp
    src/automap_screen_presenter.cpp
    src/surface_texture.cpp
    src/surface_column_scaler.cpp
    src/surface_shading.cpp
    src/surface_fill.cpp
    src/surface_wall_presenter.cpp
    src/surface_prepass.cpp
    src/game_source.cpp
    src/resource_assets.cpp
    src/startup_presenter.cpp
    src/indexed_font_presenter.cpp
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

add_executable(npc_catalog_tests tests/npc_catalog_tests.cpp)
target_link_libraries(npc_catalog_tests PRIVATE rloft_boot_flow)
add_test(NAME npc_catalog_tests COMMAND npc_catalog_tests)

add_executable(level22_story_tests tests/level22_story_tests.cpp)
target_link_libraries(level22_story_tests PRIVATE rloft_boot_flow)
add_test(NAME level22_story_tests COMMAND level22_story_tests)

add_executable(level22_encounter_tests tests/level22_encounter_tests.cpp)
target_link_libraries(level22_encounter_tests PRIVATE rloft_boot_flow)
add_test(NAME level22_encounter_tests COMMAND level22_encounter_tests)

add_executable(level_world_state_tests tests/level_world_state_tests.cpp)
target_link_libraries(level_world_state_tests PRIVATE rloft_boot_flow)
add_test(NAME level_world_state_tests COMMAND level_world_state_tests)

add_executable(inventory_screen_tests tests/inventory_screen_tests.cpp)
target_link_libraries(inventory_screen_tests PRIVATE rloft_boot_flow)
add_test(NAME inventory_screen_tests COMMAND inventory_screen_tests)

add_executable(inventory_runtime_tests tests/inventory_runtime_tests.cpp)
target_link_libraries(inventory_runtime_tests PRIVATE rloft_boot_flow)
add_test(NAME inventory_runtime_tests COMMAND inventory_runtime_tests)

add_executable(party_hud_tests tests/party_hud_tests.cpp)
target_link_libraries(party_hud_tests PRIVATE rloft_boot_flow)
add_test(NAME party_hud_tests COMMAND party_hud_tests)

add_executable(party_panel_controls_tests tests/party_panel_controls_tests.cpp)
target_link_libraries(party_panel_controls_tests PRIVATE rloft_boot_flow)
add_test(NAME party_panel_controls_tests COMMAND party_panel_controls_tests)

add_executable(top_edge_menu_tests tests/top_edge_menu_tests.cpp)
target_link_libraries(top_edge_menu_tests PRIVATE rloft_boot_flow)
add_test(NAME top_edge_menu_tests COMMAND top_edge_menu_tests)

add_executable(top_edge_menu_presenter_tests tests/top_edge_menu_presenter_tests.cpp)
target_link_libraries(top_edge_menu_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME top_edge_menu_presenter_tests COMMAND top_edge_menu_presenter_tests)

add_executable(quit_confirm_tests tests/quit_confirm_tests.cpp)
target_link_libraries(quit_confirm_tests PRIVATE rloft_boot_flow)
add_test(NAME quit_confirm_tests COMMAND quit_confirm_tests)

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

add_executable(exploration_hit_regions_tests tests/exploration_hit_regions_tests.cpp)
target_link_libraries(exploration_hit_regions_tests PRIVATE rloft_boot_flow)
add_test(NAME exploration_hit_regions_tests COMMAND exploration_hit_regions_tests)

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

add_executable(first_person_entity_projection_tests tests/first_person_entity_projection_tests.cpp)
target_link_libraries(first_person_entity_projection_tests PRIVATE rloft_boot_flow)
add_test(NAME first_person_entity_projection_tests COMMAND first_person_entity_projection_tests)

add_executable(npc_scene_tests tests/npc_scene_tests.cpp)
target_link_libraries(npc_scene_tests PRIVATE rloft_boot_flow)
add_test(NAME npc_scene_tests COMMAND npc_scene_tests)

add_executable(scene_shape_selector_tests tests/scene_shape_selector_tests.cpp)
target_link_libraries(scene_shape_selector_tests PRIVATE rloft_boot_flow)
add_test(NAME scene_shape_selector_tests COMMAND scene_shape_selector_tests)

add_executable(loose_item_scene_tests tests/loose_item_scene_tests.cpp)
target_link_libraries(loose_item_scene_tests PRIVATE rloft_boot_flow)
add_test(NAME loose_item_scene_tests COMMAND loose_item_scene_tests)

add_executable(scene_source_banks_tests tests/scene_source_banks_tests.cpp)
target_link_libraries(scene_source_banks_tests PRIVATE rloft_boot_flow)
add_test(NAME scene_source_banks_tests COMMAND scene_source_banks_tests)

add_executable(world_scene_record_tests tests/world_scene_record_tests.cpp)
target_link_libraries(world_scene_record_tests PRIVATE rloft_boot_flow)
add_test(NAME world_scene_record_tests COMMAND world_scene_record_tests)

add_executable(world_scene_selection_tests tests/world_scene_selection_tests.cpp)
target_link_libraries(world_scene_selection_tests PRIVATE rloft_boot_flow)
add_test(NAME world_scene_selection_tests COMMAND world_scene_selection_tests)

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

add_executable(automap_annotation_presenter_tests tests/automap_annotation_presenter_tests.cpp)
target_link_libraries(automap_annotation_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_annotation_presenter_tests COMMAND automap_annotation_presenter_tests)

add_executable(automap_viewport_presenter_tests tests/automap_viewport_presenter_tests.cpp)
target_link_libraries(automap_viewport_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_viewport_presenter_tests COMMAND automap_viewport_presenter_tests)

add_executable(automap_screen_state_tests tests/automap_screen_state_tests.cpp)
target_link_libraries(automap_screen_state_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_screen_state_tests COMMAND automap_screen_state_tests)

add_executable(automap_controls_tests tests/automap_controls_tests.cpp)
target_link_libraries(automap_controls_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_controls_tests COMMAND automap_controls_tests)

add_executable(automap_modal_tests tests/automap_modal_tests.cpp)
target_link_libraries(automap_modal_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_modal_tests COMMAND automap_modal_tests)

add_executable(automap_screen_presenter_tests tests/automap_screen_presenter_tests.cpp)
target_link_libraries(automap_screen_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME automap_screen_presenter_tests COMMAND automap_screen_presenter_tests)

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

add_executable(indexed_font_presenter_tests tests/indexed_font_presenter_tests.cpp)
target_link_libraries(indexed_font_presenter_tests PRIVATE rloft_boot_flow)
add_test(NAME indexed_font_presenter_tests COMMAND indexed_font_presenter_tests)

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

add_executable(character_modal_stats_tests tests/character_modal_stats_tests.cpp)
target_link_libraries(character_modal_stats_tests PRIVATE rloft_boot_flow)
add_test(NAME character_modal_stats_tests COMMAND character_modal_stats_tests)

add_executable(character_transfer_tests tests/character_transfer_tests.cpp)
target_link_libraries(character_transfer_tests PRIVATE rloft_boot_flow)
add_test(NAME character_transfer_tests COMMAND character_transfer_tests)

add_executable(character_palette_remap_tests tests/character_palette_remap_tests.cpp)
target_link_libraries(character_palette_remap_tests PRIVATE rloft_boot_flow)
add_test(NAME character_palette_remap_tests COMMAND character_palette_remap_tests)

add_executable(sound_config_tests tests/sound_config_tests.cpp)
target_link_libraries(sound_config_tests PRIVATE rloft_boot_flow)
add_test(NAME sound_config_tests COMMAND sound_config_tests)

add_executable(options_menu_tests tests/options_menu_tests.cpp)
target_link_libraries(options_menu_tests PRIVATE rloft_boot_flow)
add_test(NAME options_menu_tests COMMAND options_menu_tests)

set(RLOFT_TEST_TARGETS
    boot_flow_tests
    exploration_state_tests
    exploration_session_tests
    exploration_frame_tests
    exploration_scene_tests
    level_display_tests
    party_presentation_tests
    object_catalog_tests
    npc_catalog_tests
    level22_story_tests
    level22_encounter_tests
    level_world_state_tests
    inventory_screen_tests
    inventory_runtime_tests
    party_hud_tests
    party_panel_controls_tests
    top_edge_menu_tests
    top_edge_menu_presenter_tests
    quit_confirm_tests
    exploration_ui_tests
    exploration_hud_presenter_tests
    exploration_presenter_tests
    exploration_world_tests
    exploration_collision_tests
    exploration_motion_tests
    exploration_legacy_input_tests
    exploration_hit_regions_tests
    exploration_locomotion_tests
    exploration_runtime_motion_tests
    exploration_runtime_tests
    environment_view_tests
    view_sweep_record_tests
    view_sweep_setup_tests
    view_sweep_traversal_tests
    view_sweep_runs_tests
    fixed_projection_tests
    first_person_entity_projection_tests
    npc_scene_tests
    scene_shape_selector_tests
    loose_item_scene_tests
    scene_source_banks_tests
    world_scene_record_tests
    world_scene_selection_tests
    renderer_tables_tests
    micro_rasterizer_layout_tests
    micro_rasterizer_plan_tests
    environment_projection_tests
    environment_presenter_tests
    automap_presentation_tests
    automap_annotations_tests
    automap_annotation_presenter_tests
    automap_viewport_presenter_tests
    automap_screen_state_tests
    automap_controls_tests
    automap_modal_tests
    automap_screen_presenter_tests
    surface_texture_tests
    surface_column_scaler_tests
    surface_shading_tests
    surface_fill_tests
    surface_wall_presenter_tests
    surface_prepass_tests
    game_source_tests
    presenter_tests
    indexed_font_presenter_tests
    voc_decoder_tests
    xmidi_tests
    character_flow_tests
    character_stats_tests
    character_sheet_tests
    character_modal_stats_tests
    character_transfer_tests
    character_palette_remap_tests
    sound_config_tests
    options_menu_tests
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

add_executable(assassin_relic_transition_tests tests/assassin_relic_transition_tests.cpp)
target_link_libraries(assassin_relic_transition_tests PRIVATE rloft_boot_flow)
add_test(NAME assassin_relic_transition_tests COMMAND assassin_relic_transition_tests)
