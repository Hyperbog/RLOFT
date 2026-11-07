# Independently compiled host-owned game systems.
# This module is intentionally loaded AFTER the existing game/test targets:
# CODE.1 and CODE.2 retain their single reconstructed source ownership.

add_library(rloft_game_audio STATIC src/game/audio/Audio.cpp)
add_library(rloft_game_resources STATIC src/game/resources/Resources.cpp)
foreach(system_target IN ITEMS rloft_game_audio rloft_game_resources)
    target_compile_features(${system_target} PUBLIC cxx_std_17)
    if(MSVC)
        target_compile_options(${system_target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${system_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
target_include_directories(rloft_game_audio PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/audio")
target_include_directories(rloft_game_resources PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/resources")

# Only owner-facing decoders depend on the host-only VOC codec.
foreach(consumer IN ITEMS
        rloft_host_original_voc_waveout
        rloft_original_voc_pcm8_tests
        rloft_original_source_winmm_boundary_test
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing audio consumer: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE rloft_game_audio)
endforeach()

# Palette resolver is independent of CODE.2. Link it into all guest-host
# owners and its direct regression, never compile CODE.2 a second time.
foreach(consumer IN ITEMS
        rloft_original_scene_palette_selection_test
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot
        rloft_original_code2_world_frame)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing resource consumer: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE rloft_game_resources)
endforeach()
if(WIN32)
    if(NOT TARGET rloft_code2_level22_window)
        message(FATAL_ERROR "Missing Windows retained CODE.2 player")
    endif()
    target_link_libraries(rloft_code2_level22_window PRIVATE rloft_game_resources)
endif()

# The retained Windows player instantiates WinMMSourceSfx::preflight,
# which invokes source_audio::decode_original_length_prefixed_voc.
# Its Audio.cpp definition must be linked into this executable.
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE rloft_game_audio)
endif()

# Original source record-index/metadata ABI, not a second decoded-image cache.
add_library(rloft_game_resource_cache STATIC src/game/resources/ResourceCache.cpp)
target_compile_features(rloft_game_resource_cache PUBLIC cxx_std_17)
add_executable(rloft_resource_cache_test src/test_original_res_host_cache.cpp)
target_compile_features(rloft_resource_cache_test PRIVATE cxx_std_17)
target_link_libraries(rloft_resource_cache_test PRIVATE rloft_game_resource_cache)
if(BUILD_TESTING)
    add_test(NAME original_code2_31334_resource_record_host_abi
        COMMAND rloft_resource_cache_test)
endif()
foreach(consumer IN ITEMS rloft_original_code2_init_resources
                          rloft_code1_code2_staged_boot)
    target_link_libraries(${consumer} PRIVATE rloft_game_resource_cache)
endforeach()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE rloft_game_resource_cache)
endif()

# Source-authenticated WORK/LEV and world-raster services; one original guest.
add_library(rloft_game_save STATIC src/game/save/Save.cpp)
add_library(rloft_game_world STATIC src/game/world/World.cpp)
foreach(system_target IN ITEMS rloft_game_save rloft_game_world)
    target_compile_features(${system_target} PUBLIC cxx_std_17)
    if(MSVC)
        target_compile_options(${system_target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${system_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
foreach(consumer IN ITEMS
        rloft_original_code2_live_level_files_test
        rloft_original_code2_saved_level_policy_test
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot
        rloft_original_code2_world_frame
        rloft_original_code2_save_game)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing Save ABI consumer: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE rloft_game_save)
endforeach()
target_link_libraries(rloft_original_code2_world_frame PRIVATE rloft_game_world)
# CODE.2 native entry and staged owners include the original 36CEC
# compositor; its non-inline World.cpp must be present at final link.
foreach(consumer IN ITEMS
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot
        rloft_original_code2_369d8_raster_test)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing World ABI consumer: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE rloft_game_world)
endforeach()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window
        PRIVATE rloft_game_save rloft_game_world)
endif()
