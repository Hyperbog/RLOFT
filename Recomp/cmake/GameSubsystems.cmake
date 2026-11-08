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

# Original guest callbacks borrow the retained IVT, guest byte vector,
# and private WORK/LEV overlay. No duplicate CODE.2 symbol or guest owner.
add_library(rloft_game_guest_callbacks STATIC src/game/core/GuestCallbacks.cpp)
target_compile_features(rloft_game_guest_callbacks PUBLIC cxx_std_17)
target_link_libraries(rloft_game_guest_callbacks PUBLIC rloft_game_save)
if(MSVC)
    target_compile_options(rloft_game_guest_callbacks PRIVATE /W4 /permissive-)
else()
    target_compile_options(rloft_game_guest_callbacks PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(rloft_original_code2_guest_callbacks_test
    src/test_original_code2_guest_callbacks.cpp)
target_compile_features(rloft_original_code2_guest_callbacks_test PRIVATE cxx_std_17)
target_link_libraries(rloft_original_code2_guest_callbacks_test
    PRIVATE rloft_game_guest_callbacks)
if(BUILD_TESTING)
    add_test(NAME original_code2_borrowed_guest_vector_and_level_io_callbacks
        COMMAND rloft_original_code2_guest_callbacks_test)
endif()
foreach(consumer IN ITEMS
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing CODE2 guest callback owner: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE rloft_game_guest_callbacks)
endforeach()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE rloft_game_guest_callbacks)
endif()

# Original 139AC DOS filename and original 30BB4 source vector contracts.
# Compiled outside the canonical CODE.2 owner; never replace DOS semantics.
add_library(rloft_game_host_dos_services STATIC src/game/filesystem/DosServices.cpp)
add_library(rloft_game_interrupt_lifecycle STATIC src/game/timing/InterruptLifecycle.cpp)
target_link_libraries(rloft_game_interrupt_lifecycle PUBLIC rloft_game_guest_callbacks)
foreach(system_target IN ITEMS
        rloft_game_host_dos_services
        rloft_game_interrupt_lifecycle)
    target_compile_features(${system_target} PUBLIC cxx_std_17)
    if(MSVC)
        target_compile_options(${system_target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${system_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
add_executable(rloft_original_code2_dos_interrupt_host_test
    src/test_original_code2_dos_interrupt_host.cpp)
target_compile_features(rloft_original_code2_dos_interrupt_host_test PRIVATE cxx_std_17)
target_link_libraries(rloft_original_code2_dos_interrupt_host_test PRIVATE
    rloft_game_host_dos_services rloft_game_interrupt_lifecycle)
if(BUILD_TESTING)
    add_test(NAME original_code2_139ac_dos_path_30bb4_vector_host_contract
        COMMAND rloft_original_code2_dos_interrupt_host_test)
endif()
foreach(consumer IN ITEMS
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing original DOS/IRQ host consumer: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE
        rloft_game_host_dos_services rloft_game_interrupt_lifecycle)
endforeach()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE
        rloft_game_host_dos_services rloft_game_interrupt_lifecycle)
endif()

# Independent host DOS file I/O and VGA device; guest/source handlers remain
# exclusively compiled inside the retained CODE.2 owner.
add_library(rloft_game_dos_filesystem STATIC src/game/filesystem/Filesystem.cpp)
add_library(rloft_game_vga STATIC src/game/graphics/Graphics.cpp)
foreach(system_target IN ITEMS rloft_game_dos_filesystem rloft_game_vga)
    target_compile_features(${system_target} PUBLIC cxx_std_17)
    if(MSVC)
        target_compile_options(${system_target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${system_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
target_link_libraries(rloft_game_world PUBLIC rloft_game_vga)
foreach(consumer IN ITEMS
        rloft_dos_readonly_files_test
        rloft_original_code2_chars_access_test
        rloft_original_code2_level_loader
        rloft_original_code2_save_game
        rloft_original_code2_world_frame
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing original DOS file consumer: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE rloft_game_dos_filesystem)
endforeach()
foreach(consumer IN ITEMS
        rloft_original_vga_host_tests
        rloft_original_code2_vga_boot_test
        rloft_original_code2_graphics_frame
        rloft_original_code2_cine00_player
        rloft_original_code2_scene_actor_source_test
        rloft_original_code2_game_over_vga_strip_test
        rloft_original_code2_world_frame
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing original VGA host consumer: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE rloft_game_vga)
endforeach()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE
        rloft_game_dos_filesystem rloft_game_vga)
endif()

# Independent CODE.2 host presentation and interactive input adapters.
# They borrow the same source guest; CODE.2 is never compiled into libraries.
add_library(rloft_game_presentation STATIC src/game/ui/Presentation.cpp)
add_library(rloft_game_input STATIC src/game/input/Input.cpp)
foreach(system_target IN ITEMS rloft_game_presentation rloft_game_input)
    target_compile_features(${system_target} PUBLIC cxx_std_17)
    if(MSVC)
        target_compile_options(${system_target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${system_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
add_executable(rloft_original_code2_presentation_input_test
    src/test_original_code2_presentation_input_adapters.cpp)
target_compile_features(rloft_original_code2_presentation_input_test PRIVATE cxx_std_17)
target_link_libraries(rloft_original_code2_presentation_input_test PRIVATE
    rloft_game_presentation rloft_game_input)
if(BUILD_TESTING)
    add_test(NAME original_code2_presentation_input_host_abi
        COMMAND rloft_original_code2_presentation_input_test)
endif()
foreach(consumer IN ITEMS
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing CODE2 presentation/input owner: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE
        rloft_game_presentation rloft_game_input)
endforeach()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE
        rloft_game_presentation rloft_game_input)
endif()

# Phase 9: independently compiled borrowed title-event and mouse ABI.
add_library(rloft_game_title_interaction STATIC src/game/ui/TitleInteraction.cpp)
add_library(rloft_game_mouse_lifecycle STATIC src/game/input/MouseLifecycle.cpp)
foreach(system_target IN ITEMS rloft_game_title_interaction rloft_game_mouse_lifecycle)
    target_compile_features(${system_target} PUBLIC cxx_std_17)
    if(MSVC)
        target_compile_options(${system_target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${system_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
add_executable(rloft_original_code2_title_mouse_host_test
    src/test_original_code2_title_mouse_host.cpp)
target_compile_features(rloft_original_code2_title_mouse_host_test PRIVATE cxx_std_17)
target_link_libraries(rloft_original_code2_title_mouse_host_test PRIVATE
    rloft_game_title_interaction rloft_game_mouse_lifecycle)
if(BUILD_TESTING)
    add_test(NAME original_code2_19464_title_mouse_1a064_callback_host_abi
        COMMAND rloft_original_code2_title_mouse_host_test)
endif()
foreach(consumer IN ITEMS
        rloft_code2_startup_gate
        rloft_original_code2_stackful_mouse_gate
        rloft_original_code2_init_resources
        rloft_code1_code2_staged_boot)
    if(NOT TARGET ${consumer})
        message(FATAL_ERROR "Missing CODE2 original title/mouse guest owner: ${consumer}")
    endif()
    target_link_libraries(${consumer} PRIVATE
        rloft_game_title_interaction rloft_game_mouse_lifecycle)
endforeach()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE
        rloft_game_title_interaction rloft_game_mouse_lifecycle)
endif()

# Phase 10: physical title mouse edge retention, independent from CODE.2.
add_library(rloft_game_mouse_dispatch STATIC src/game/input/MouseDispatch.cpp)
target_compile_features(rloft_game_mouse_dispatch PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(rloft_game_mouse_dispatch PRIVATE /W4 /permissive-)
else()
    target_compile_options(rloft_game_mouse_dispatch PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(rloft_original_code2_mouse_dispatch_test
    src/test_original_code2_mouse_dispatch.cpp)
target_compile_features(rloft_original_code2_mouse_dispatch_test PRIVATE cxx_std_17)
target_link_libraries(rloft_original_code2_mouse_dispatch_test PRIVATE
    rloft_game_mouse_dispatch)
if(BUILD_TESTING)
    add_test(NAME original_code2_19464_physical_mouse_edges
        COMMAND rloft_original_code2_mouse_dispatch_test)
endif()
if(WIN32)
    target_link_libraries(rloft_code2_level22_window PRIVATE
        rloft_game_mouse_dispatch)
endif()

# Phase 10 source regression: execute real reconstructed 27894->19D18.
# This test compiles the original CODE.2 only inside its own fixture.
add_executable(rloft_original_code2_27894_mouse_isr_test
    src/test_original_code2_27894_mouse_isr.cpp)
set_target_properties(rloft_original_code2_27894_mouse_isr_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_27894_mouse_isr_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_27894_mouse_isr_test
        PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
target_link_libraries(rloft_original_code2_27894_mouse_isr_test PRIVATE
    rloft_game_mouse_dispatch rloft_game_title_interaction)
if(BUILD_TESTING)
    add_test(NAME original_code2_27894_19D18_mouse_isr_execution
        COMMAND rloft_original_code2_27894_mouse_isr_test)
endif()
