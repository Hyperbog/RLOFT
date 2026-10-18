cmake_minimum_required(VERSION 3.22)
project(RavenloftReconstructionLinkGate LANGUAGES CXX)

# Full canonical CODE.1/CODE.2 compile/link/leaf-execution witness; not yet DOS boot.
add_executable(rloft_recomp_link_gate
    src/code1_compilation_unit.cpp
    src/code2_compilation_unit.cpp
    src/original_le_image.cpp
    src/link_gate_main.cpp)
add_executable(rloft_code1_startup_gate
    src/original_le_image.cpp
    src/code1_startup_gate.cpp)
# Source-backed CODE.1 original 10AA9 heap growth from an empty host DPMI
# arena, separate from the CODE.2 fixture-seeded startup tests.
add_executable(rloft_code1_original_heap_growth_gate
    src/code1_original_heap_growth_gate.cpp)
# Real source CODE.1 mid-runtime DPMI 0100/0006/0101 and
# 0501/0502: source-written DOS/linear ownership headers.
add_executable(rloft_code1_original_dpmi_memory_gate
    src/code1_original_dpmi_memory_gate.cpp)
set_target_properties(rloft_code1_original_dpmi_memory_gate PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_code1_original_dpmi_memory_gate PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_code1_original_dpmi_memory_gate PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
# Original CODE.1 mode-1 fallback, DOS 48h/49h host paragraph lifecycle.
add_executable(rloft_code1_original_dos_fallback_gate
    src/code1_original_dos_fallback_gate.cpp)
set_target_properties(rloft_code1_original_dos_fallback_gate PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_code1_original_dos_fallback_gate PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_code1_original_dos_fallback_gate PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
set_target_properties(rloft_code1_original_heap_growth_gate PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_code1_original_heap_growth_gate PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_code1_original_heap_growth_gate PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
add_executable(rloft_code2_startup_gate
    src/original_le_image.cpp
    src/code2_startup_gate.cpp)
add_executable(rloft_original_image_gate
    src/original_le_image.cpp
    src/original_image_gate.cpp)
foreach(target IN ITEMS rloft_recomp_link_gate rloft_original_image_gate rloft_code1_startup_gate rloft_code2_startup_gate)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    if(MSVC)
        target_compile_options(${target} PRIVATE /Od /bigobj /W3)
    else()
        target_compile_options(${target} PRIVATE -O0 -Wall -Wextra -Wpedantic)
    endif()
endforeach()
# Constrained host-backed DOS read-only file transport, tested independently
# before binding it to canonical reconstructed game callbacks.
add_executable(rloft_dos_readonly_files_test src/test_dos_readonly_files.cpp)
set_target_properties(rloft_dos_readonly_files_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
# Stateful DOS conventional memory / selector transport, independently tested.
add_executable(rloft_extender_data_break_test src/test_extender_data_break.cpp)
set_target_properties(rloft_extender_data_break_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
add_executable(rloft_dos_paragraph_memory_test src/test_dos_paragraph_memory.cpp)
set_target_properties(rloft_dos_paragraph_memory_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
add_executable(rloft_dpmi_dos_memory_test src/test_dpmi_dos_memory.cpp)
set_target_properties(rloft_dpmi_dos_memory_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
add_executable(rloft_dpmi_selector_descriptors_test src/test_dpmi_selector_descriptors.cpp)
set_target_properties(rloft_dpmi_selector_descriptors_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
# Real host-backed block lifetime for DPMI protected-mode linear memory.
add_executable(rloft_dpmi_linear_memory_test src/test_dpmi_linear_memory.cpp)
set_target_properties(rloft_dpmi_linear_memory_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME canonical_host_readonly_dos_file_service_contract
        COMMAND rloft_dos_readonly_files_test)
    add_test(NAME canonical_dpmi_dos_memory_selector_lifecycle
        COMMAND rloft_dpmi_dos_memory_test)
    add_test(NAME canonical_dpmi_0006_selector_base_and_descriptor_lifecycle
        COMMAND rloft_dpmi_selector_descriptors_test)
    add_test(NAME canonical_dpmi_0501_0502_0503_linear_memory_lifecycle
        COMMAND rloft_dpmi_linear_memory_test)
    # Execute actual compiled CODE.2 30D80->30D52->33B44 DPMI 0100
    # with host-owned allocation state, stopping at external INT31 0300.
    add_test(NAME canonical_code2_original_dpmi_dos_allocation_via_host
        COMMAND rloft_code2_startup_gate --mouse-dpmi-to-real-mode)
    # The source 33B44->30DE8->30D52 frees the tracked live selector
    # after a clearly synthetic INT31 0300 no-driver response.
    add_test(NAME canonical_code2_original_dpmi_selector_release_via_host
        COMMAND rloft_code2_startup_gate --mouse-dpmi-to-disk-query)
    # Asset-free integration: a generated 59-byte noncommercial SOUND.CFG.
    # Actual compiled CODE.2 37E77/37E99/38106/380F8 invokes the
    # read-only host DOS service. DOS memory, vectors and mouse are fixtures.
    set(_dos_host_fixture "${CMAKE_CURRENT_BINARY_DIR}/dos_host_fixture")
    file(MAKE_DIRECTORY "${_dos_host_fixture}")
    string(REPEAT "A" 59 _dos_host_sound_bytes)
    file(WRITE "${_dos_host_fixture}/SOUND.CFG" "${_dos_host_sound_bytes}")
    add_test(NAME canonical_code2_original_sound_file_via_host_dos_provider
        COMMAND rloft_code2_startup_gate --to-mouse-reset-host-file
                "${_dos_host_fixture}")
    # A SECOND independent synthetic DOS 8.3 resource tests compiled
    # original 37E77, 38106, 380F8 after the bounded mouse-stop witness.
    string(REPEAT "Z" 127 _dos_host_objects_bytes)
    file(WRITE "${_dos_host_fixture}/OBJECTS.DAT" "${_dos_host_objects_bytes}")
    add_test(NAME canonical_code2_original_host_multifile_descriptor_and_seek
        COMMAND rloft_code2_startup_gate --to-mouse-reset-host-multifile
                "${_dos_host_fixture}")
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        # Pure standard-library tests. Validate authenticated-media
        # execution matrix contracts without bundling commercial files.
        add_test(NAME canonical_original_media_matrix_contract
            COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_original_media_matrix.py")
        add_test(NAME canonical_recomp_progress_major_gate_score
            COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_recomp_progress.py")
    endif()
    add_test(NAME canonical_reconstruction_compiles_links_and_runs COMMAND rloft_recomp_link_gate)
    add_test(NAME original_le_image_and_fixup_integrity COMMAND rloft_original_image_gate)
    add_test(NAME canonical_code1_original_10aa9_host_dpmi_heap_growth
        COMMAND rloft_code1_original_heap_growth_gate)
    add_test(NAME canonical_code1_original_6f10_6fc0_6ff4_709c_host_dpmi_memory
        COMMAND rloft_code1_original_dpmi_memory_gate)
    add_test(NAME canonical_code1_original_10aa9_dos48_fallback_and_dos49_reclaim
        COMMAND rloft_code1_original_dos_fallback_gate)
    add_test(NAME canonical_host_dos_ah48_49_4a_paragraph_memory
        COMMAND rloft_dos_paragraph_memory_test)
    add_test(NAME canonical_pharlap_ah4a_4kib_guest_dgroup_resize
        COMMAND rloft_extender_data_break_test)
    add_test(NAME canonical_code1_original_ccde_ah4a_host_dgroup_resize
        COMMAND rloft_code1_startup_gate --extender-host-resize)
    add_test(NAME canonical_code1_startup_ccde_to_original_source_dpmi_heap_lifetime
        COMMAND rloft_code1_startup_gate --extender-host-resize-to-source-heap)
    # From the original CODE.1 CC1C/CCDE extender handshake through CD97:
    # FCE8 original constructor table, FC98 original heap default setter.
    add_test(NAME canonical_code1_startup_original_cd97_pharlap_mode2
        COMMAND rloft_code1_startup_gate --extender-host-resize-to-cd97)
    add_test(NAME canonical_code1_startup_cd97_mode2_source_10dd7_break_heap
        COMMAND rloft_code1_startup_gate --extender-host-resize-to-cd97-break-heap)
    add_test(NAME canonical_code1_startup_cd97_original_source_stdio_argv_constructors
        COMMAND rloft_code1_startup_gate --extender-host-resize-to-cd97-constructors)
    # Prevent an authentic-source circular-list regression from stalling
    # the entire canonical Windows workflow indefinitely.
    set_tests_properties(
        canonical_code1_startup_cd97_original_source_stdio_argv_constructors
        PROPERTIES TIMEOUT 12)
    add_test(NAME canonical_code1_startup_to_first_original_dos_version
        COMMAND rloft_code1_startup_gate --stop-before-version)
    add_test(NAME canonical_code1_original_dos6_startup_via_cd17_cd50
        COMMAND rloft_code1_startup_gate --dos6-to-ff00)
    add_test(NAME canonical_code1_original_extender_marker_via_ccde_resize
        COMMAND rloft_code1_startup_gate --extender-to-resize)
    add_test(NAME canonical_code2_startup_to_first_original_initializer COMMAND rloft_code2_startup_gate)
    add_test(NAME canonical_code2_all_four_initializers_to_game_entry_boundary COMMAND rloft_code2_startup_gate --through-initializers)
    # Executes the complete original canonical 353A0 startup and all four
    # initializers, then compiled CODE.2 20608 -> 20440 -> 2F2EC.
    # Stops at the first original 375FE far/ISR installation, not a fake
    # substitute. Three independent DOS environment/atoi configurations.
    add_test(NAME canonical_code2_actual_game_entry_to_original_isr_boundary
        COMMAND rloft_code2_startup_gate --enter-game)
    add_test(NAME canonical_code2_original_game_environment_overrides
        COMMAND rloft_code2_startup_gate --enter-game-config)
    add_test(NAME canonical_code2_original_game_signed_wrapping_args
        COMMAND rloft_code2_startup_gate --enter-game-signed-wrap)
    # Exhaustive original source CODE.2 375FE 8-bit extender-mode branch.
    # Executes real canonical function_375fe 256 times, *observes* but never
    # installs a DOS interrupt vector (separate acceptance gate).
    add_test(NAME canonical_code2_original_far_callback_vector_mode_matrix
        COMMAND rloft_code2_startup_gate --vector-mode-matrix)
    # Executes original 375FE + 30BB4 vector lifecycle using a bounded
    # TEST-ONLY DOS IVT, then real CODE.2 290D0/30CC4 RNG seed,
    # 3104C/37B34 checked arena allocation, 30D30/379D0 memset and
    # 312A0 to the fail-closed original sound.cfg open (37E77).
    add_test(NAME canonical_code2_source_runtime_to_sound_cfg_open
        COMMAND rloft_code2_startup_gate --to-sound-config)
    add_test(NAME canonical_code2_configured_runtime_to_sound_cfg_open
        COMMAND rloft_code2_startup_gate --to-sound-config-configured)
    # Original 3968E parsed argc==1 causes real CODE.2 2F2EC to execute
    # 3104C / 37B34 checked allocator and 30D30/379D0 zero fill.
    # Original argc==2 goes through 35349 parsed DOS argv instead.
    add_test(NAME canonical_code2_no_argv_original_audio_checked_allocator
        COMMAND rloft_code2_startup_gate --to-sound-config-no-args)
    add_test(NAME canonical_code2_configured_no_argv_audio_checked_allocator
        COMMAND rloft_code2_startup_gate --to-sound-config-no-args-configured)
    # TEST-ONLY read-only 59-byte DOS fixture, then full original compiled
    # 37E99 descriptor publication, 312EC->38106 binary file read,
    # 312DC->380F8 close, 2F2EC flags and source 33B44. Halt on actual
    # external INT 33h mouse reset; no synthetic "mouse installed" outcome.
    add_test(NAME canonical_code2_original_sound_source_to_mouse_reset
        COMMAND rloft_code2_startup_gate --to-mouse-reset)
    add_test(NAME canonical_code2_configured_sound_source_to_mouse_reset
        COMMAND rloft_code2_startup_gate --to-mouse-reset-configured)
    add_test(NAME canonical_code2_no_argv_sound_source_to_mouse_reset
        COMMAND rloft_code2_startup_gate --to-mouse-reset-no-args)
    add_test(NAME canonical_code2_configured_no_argv_sound_source_to_mouse_reset
        COMMAND rloft_code2_startup_gate --to-mouse-reset-no-args-configured)
    # Run real CODE.2 33B44 with explicitly simulated INT33 results:
    # present => source 30D80 DOS-memory boundary, absent => source
    # 311E8 warning boundary. Neither branch invents DOS runtime success.
    add_test(NAME canonical_code2_mouse_present_source_to_real_dos_memory_boundary
        COMMAND rloft_code2_startup_gate --mouse-present-to-memory)
    # Source-backed original DPMI success receipt (explicit TEST-ONLY result)
    # then original 33AA0 real-mode INT33 simulation wrapper to DPMI 0300.
    add_test(NAME canonical_code2_source_dpmi_0100_to_actual_33aa0_bridge
        COMMAND rloft_code2_startup_gate --mouse-present-to-real-mode)
    # Three deeper original-source calls executed under explicitly synthetic
    # DPMI service outcomes. Halts before external hardware effects.
    add_test(NAME canonical_code2_source_dpmi_0101_original_mouse_release
        COMMAND rloft_code2_startup_gate --mouse-no-driver-to-dpmi-free)
    add_test(NAME canonical_code2_original_33d18_mouse_x_range_entry
        COMMAND rloft_code2_startup_gate --mouse-no-driver-to-x-range)
    add_test(NAME canonical_code2_original_33d38_mouse_y_range_entry
        COMMAND rloft_code2_startup_gate --mouse-no-driver-to-y-range)
    # Source CODE.2 20440 -> 202F0 -> 35286 original DOS disk-free
    # call, reached only after the full original 2F2EC audio prelude returns.
    add_test(NAME canonical_code2_original_20440_to_35286_disk_free_dos_boundary
        COMMAND rloft_code2_startup_gate --mouse-no-driver-to-disk-query)
    add_test(NAME canonical_code2_mouse_absent_source_to_real_dos_warning_boundary
        COMMAND rloft_code2_startup_gate --mouse-absent-to-warning)
    add_test(NAME canonical_code2_original_termination_table_to_exit_boundary COMMAND rloft_code2_startup_gate --termination-probe)
    add_test(NAME canonical_code2_original_arena_split_coalesce_reuse COMMAND rloft_code2_startup_gate --arena-roundtrip-probe)
    add_test(NAME canonical_code2_env_allocation_failure_frees_original_arena_block COMMAND rloft_code2_startup_gate --env-rollback-probe)
endif()

# R04: original CODE.2 37B42 -> 39D4D -> 39C7B source allocation,
# DPMI 0501 host page ownership; arena chain begins at zero (unseeded).
add_executable(rloft_code2_original_dos_fallback_gate
    src/code2_original_dos_fallback_gate.cpp)
set_target_properties(rloft_code2_original_dos_fallback_gate PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_code2_original_dos_fallback_gate PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_code2_original_dos_fallback_gate PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
add_executable(rloft_code2_original_dpmi_heap_gate
    src/code2_original_dpmi_heap_gate.cpp)
set_target_properties(rloft_code2_original_dpmi_heap_gate PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_code2_original_dpmi_heap_gate PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_code2_original_dpmi_heap_gate PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_unseeded_host_dpmi_arena_growth
        COMMAND rloft_code2_original_dpmi_heap_gate)
    add_test(NAME canonical_code2_original_39c7b_dos_fallback_39e1c_break_growth
        COMMAND rloft_code2_original_dos_fallback_gate)
    # Continuous original entry -> four source initializers and game entry,
    # separate from the historical source-startup preseeded allocator mode.
    add_test(NAME canonical_code2_startup_initializers_unseeded_host_dpmi_heap
        COMMAND rloft_code2_startup_gate --through-initializers-host-dpmi)
    add_test(NAME canonical_code2_startup_game_entry_unseeded_host_dpmi_heap
        COMMAND rloft_code2_startup_gate --enter-game-host-dpmi)
    # Same continuous compiled original startup and 39D4D -> 39C7B path,
    # but a denied DPMI 0501 must produce authentic source-selected DOS 48
    # probe/request, 49 probe release and owned conventional memory pages.
    add_test(NAME canonical_code2_startup_initializers_unseeded_dos48_fallback
        COMMAND rloft_code2_startup_gate --through-initializers-host-dos-fallback)
    add_test(NAME canonical_code2_startup_game_entry_unseeded_dos48_fallback
        COMMAND rloft_code2_startup_gate --enter-game-host-dos-fallback)
    add_test(NAME canonical_code2_startup_no_args_sound_unseeded_dos48_fallback
        COMMAND rloft_code2_startup_gate --to-sound-config-no-args-host-dos-fallback)
    add_test(NAME canonical_code2_startup_mouse_reset_unseeded_dos48_fallback
        COMMAND rloft_code2_startup_gate --to-mouse-reset-host-dos-fallback)
    # Source-selected Phar Lap mode 2 through original CODE.2 35418,
    # 39E1C DS-break growth, live 39838/398E0 and actual application.
    add_test(NAME canonical_code2_startup_initializers_39e1c_pharlap_ds_break
        COMMAND rloft_code2_startup_gate --through-initializers-host-break)
    add_test(NAME canonical_code2_startup_game_entry_39e1c_pharlap_ds_break
        COMMAND rloft_code2_startup_gate --enter-game-host-break)
    add_test(NAME canonical_code2_startup_sound_no_args_39e1c_pharlap_ds_break
        COMMAND rloft_code2_startup_gate --to-sound-config-no-args-host-break)
    add_test(NAME canonical_code2_startup_mouse_reset_39e1c_pharlap_ds_break
        COMMAND rloft_code2_startup_gate --to-mouse-reset-host-break)
    add_test(NAME canonical_code2_startup_sound_no_args_unseeded_host_dpmi_heap
        COMMAND rloft_code2_startup_gate --to-sound-config-no-args-host-dpmi-heap)
    add_test(NAME canonical_code2_startup_mouse_reset_unseeded_host_dpmi_heap
        COMMAND rloft_code2_startup_gate --to-mouse-reset-host-dpmi-heap)
endif()

# Source-derived, asset-free VGA planar/DAC6 host boundary. This is NOT
# original CODE.2 graphics execution or a gameplay visual acceptance gate.
add_executable(rloft_original_vga_host_tests src/test_original_vga_host.cpp)
set_target_properties(rloft_original_vga_host_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_vga_host_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(rloft_original_vga_host_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME original_code2_planar_vga_dac6_crtc_host_boundary
        COMMAND rloft_original_vga_host_tests)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME original_code2_vga_source_sha_manifest_self_test
            COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/../Code2/graphics/verify_original_vga.py" --self-test)
    endif()
endif()

# Actual compiled reconstructed CODE.2 graphics routines executing through a
# host VGA port/aperture adapter. Input image/palette are synthetic only.
add_executable(rloft_original_code2_graphics_frame
    src/code2_compiled_graphics_frame.cpp)
set_target_properties(rloft_original_code2_graphics_frame PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_graphics_frame PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_graphics_frame PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME compiled_code2_original_graphics_indexed_frame
        COMMAND rloft_original_code2_graphics_frame)
    set_tests_properties(compiled_code2_original_graphics_indexed_frame
        PROPERTIES TIMEOUT 25)
    if(Python3_Interpreter_FOUND)
        add_test(NAME compiled_code2_original_colors_bank_indexed_bmp
            COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_compiled_graphics_artifact.py"
                "$<TARGET_FILE:rloft_original_code2_graphics_frame>")
        set_tests_properties(compiled_code2_original_colors_bank_indexed_bmp
            PROPERTIES TIMEOUT 35)
        add_test(NAME compiled_code2_real_res_loader_cached_image_render
            COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_compiled_original_res.py"
                "$<TARGET_FILE:rloft_original_code2_graphics_frame>")
        set_tests_properties(compiled_code2_real_res_loader_cached_image_render
            PROPERTIES TIMEOUT 40)
        add_test(NAME compiled_original_code2_res4_first_person_columns
            COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_compiled_res4.py"
                "$<TARGET_FILE:rloft_original_code2_graphics_frame>")
        set_tests_properties(compiled_original_code2_res4_first_person_columns
            PROPERTIES TIMEOUT 35)
    endif()
endif()

# Verified original 37700 loop control: guest 2B6C2 stop checks and ring dictionary.
add_executable(rloft_original_37700_control_tests test_original_37700_control.cpp)
set_target_properties(rloft_original_37700_control_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME original_code2_37700_guest_flag_source_decompression
        COMMAND rloft_original_37700_control_tests)
endif()
