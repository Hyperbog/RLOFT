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

# Real CODE.2 INIT/INFO.DAT/RES archive 0..10 source path:
# separate, bounded subsystem startup (not a game executable).
# The resource source includes the canonical startup implementation in
# a single source translation unit. CODE2.cpp is non-inline; compiling
# both independently produces duplicate original game-code symbols.
add_executable(rloft_original_code2_init_resources
    src/code2_original_res_startup.cpp
    src/original_le_image.cpp)
set_target_properties(rloft_original_code2_init_resources PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_init_resources PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_init_resources PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME original_code2_16d04_init_2e768_info_all_11_res_startup
        COMMAND rloft_original_code2_init_resources)
    set_tests_properties(original_code2_16d04_init_2e768_info_all_11_res_startup
        PROPERTIES TIMEOUT 25)
    add_test(NAME original_code2_2eb60_31334_indexed_archive_record_seek
        COMMAND rloft_original_code2_init_resources --test-indexed-31334)
    set_tests_properties(original_code2_2eb60_31334_indexed_archive_record_seek
        PROPERTIES TIMEOUT 25)
endif()

# Original SOURCE-STAGED CODE.1 constructors -> CODE.2 INIT/INFO/RES
# -> scene-selected full COLORS 325B0/32150/29CA0 in one host process.
# This is NOT the unimplemented DOS guest CODE.1->CODE.2 branch transfer.
add_executable(rloft_code1_code2_staged_boot
    src/code1_code2_staged_boot.cpp
    src/code1_startup_gate.cpp
    src/code2_original_res_startup.cpp
    src/original_le_image.cpp)
target_compile_definitions(rloft_code1_code2_staged_boot PRIVATE RLOFT_BUNDLE_NO_MAIN)
set_target_properties(rloft_code1_code2_staged_boot PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_code1_code2_staged_boot PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_code1_code2_staged_boot PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
# Original-media acceptance is run by recomp-milestone-windows.yml with
# read-only sparse-checkout files, not a self-contained synthetic CTest.

# Portably encode observed host volume stats into bounded DOS AH36
# AX/BX/CX/DX registers. This never depends on original game media.
add_executable(rloft_host_dos_disk_space_test
    tests/host_dos_disk_space_test.cpp)
target_compile_features(rloft_host_dos_disk_space_test PRIVATE cxx_std_17)
if(BUILD_TESTING)
    add_test(NAME host_dos_int21_ah36_cluster_geometry
        COMMAND rloft_host_dos_disk_space_test)
endif()

# DPMI 0300 0x32-byte real-mode register frame is validated without
# commercial media, including EAX/EBX/ECX/EDX writeback and ES/flags.
add_executable(rloft_original_dpmi_real_mode_mouse_test
    tests/original_dpmi_real_mode_mouse_test.cpp)
target_compile_features(rloft_original_dpmi_real_mode_mouse_test PRIVATE cxx_std_17)
if(BUILD_TESTING)
    add_test(NAME original_dpmi_0300_real_mode_mouse_frame_semantics
        COMMAND rloft_original_dpmi_real_mode_mouse_test)
endif()

# Retail CODE.2 source object1 5E38 missing/present GAME.OPT branches:
# read exactly 0Eh bytes, apply original actor flag, palette and sound calls.
add_executable(rloft_original_code2_game_options_test
    tests/original_code2_game_options_test.cpp)
target_compile_features(rloft_original_code2_game_options_test PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(rloft_original_code2_game_options_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_game_options_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME original_code2_5e38_game_opt_exact_optional_file_branches
        COMMAND rloft_original_code2_game_options_test)
endif()

# Execute the original 290F4 -> 29930 VGA mode implementation on the
# existing planar VGA host; source CRTC, page and DAC callbacks are real.
add_executable(rloft_original_code2_vga_boot_test
    tests/original_code2_vga_boot_test.cpp)
target_compile_features(rloft_original_code2_vga_boot_test PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(rloft_original_code2_vga_boot_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_vga_boot_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME original_code2_290f4_vga_full_mode_and_palette
        COMMAND rloft_original_code2_vga_boot_test)
endif()

# Original CODE.2 source entry + true stackful DOS mouse host boundary,
# executed against owner-original CODE.2 and SOUND.CFG in Windows CI.
add_executable(rloft_original_code2_stackful_mouse_gate
    tests/original_code2_stackful_mouse_gate.cpp
    src/code2_startup_gate.cpp
    src/original_le_image.cpp)
target_compile_definitions(rloft_original_code2_stackful_mouse_gate PRIVATE
    RLOFT_BUNDLE_NO_MAIN)
target_compile_features(rloft_original_code2_stackful_mouse_gate PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(rloft_original_code2_stackful_mouse_gate PRIVATE
        /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_stackful_mouse_gate PRIVATE
        -O0 -Wall -Wextra -Wpedantic)
endif()

# Keep the ACTUAL native C++ source guest call stack suspended while
# a host DOS/4GW dependency is handled. This avoids replaying 353A0
# after a bounded longjmp and enables exact service-by-service progress.
find_package(Threads REQUIRED)
# These translation units now expose a stackful worker-backed CODE2 owner,
# even when an individual regression still uses legacy bounded startup.
target_link_libraries(rloft_code2_startup_gate PRIVATE Threads::Threads)
target_link_libraries(rloft_code1_code2_staged_boot PRIVATE Threads::Threads)
# Experimental original CODE.2 28168 -> 27F24 host SFX adapter links to
# actual WinMM APIs. Never link a fake device on non-Windows platforms.
if(WIN32)
    foreach(target IN ITEMS
            rloft_code2_startup_gate
            rloft_code1_code2_staged_boot
            rloft_original_code2_init_resources
            rloft_original_code2_stackful_mouse_gate)
        target_link_libraries(${target} PRIVATE winmm)
    endforeach()
endif()
target_link_libraries(rloft_original_code2_stackful_mouse_gate PRIVATE Threads::Threads)
add_executable(rloft_original_guest_continuation_test
    tests/original_guest_continuation_test.cpp)
target_compile_features(rloft_original_guest_continuation_test PRIVATE cxx_std_17)
target_link_libraries(rloft_original_guest_continuation_test PRIVATE Threads::Threads)
if(MSVC)
    target_compile_options(rloft_original_guest_continuation_test PRIVATE /W4 /permissive-)
else()
    target_compile_options(rloft_original_guest_continuation_test PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME original_guest_native_stackful_two_DOS_service_calls
        COMMAND rloft_original_guest_continuation_test)
    set_tests_properties(original_guest_native_stackful_two_DOS_service_calls
        PROPERTIES TIMEOUT 20)
endif()

# Authenticated 16-bit R.EXE child-process supervisor state machine.
# It is a typed host-side control boundary, never an invented guest jump.
# Persistent native supervisor lifecycle: CODE.2 child stays live across
# steps; actual status10/12 cause transitions, exact 0x920 CHARS byte handoff.
# The synthetic regression does NOT execute a playable CODE.2 engine.
add_executable(rloft_original_supervisor_session_test
    tests/original_r_exe_native_session_test.cpp)
set_target_properties(rloft_original_supervisor_session_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_supervisor_session_test PRIVATE /W4 /permissive-)
else()
    target_compile_options(rloft_original_supervisor_session_test PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME original_r_exe_persistent_code2_child_and_exact_chars
        COMMAND rloft_original_supervisor_session_test)
endif()

add_executable(rloft_original_supervisor_test
    tests/original_r_exe_supervisor_test.cpp)
set_target_properties(rloft_original_supervisor_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME original_r_exe_supervisor_code2_first_status10_status12
        COMMAND rloft_original_supervisor_test)
endif()

# Local owner GOG game.gog ISO9660 CD data extraction. The binary can
# extract only RES9 and RES10 to private scratch; never ships game media.
add_executable(rloft_gog_cd_extract src/gog_cd_iso_extract.cpp)
set_target_properties(rloft_gog_cd_extract PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_gog_cd_extract PRIVATE /W4 /permissive-)
else()
    target_compile_options(rloft_gog_cd_extract PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING AND Python3_Interpreter_FOUND)
    add_test(NAME original_gog_iso9660_private_cd_extract_synthetic
        COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_gog_cd_iso_extract.py"
            "$<TARGET_FILE:rloft_gog_cd_extract>")
    set_tests_properties(original_gog_iso9660_private_cd_extract_synthetic
        PROPERTIES TIMEOUT 35)
endif()

# Original LEVELS.DAT -> CODE.2 334C4 view preparation; stop before 32954 rays.
# This is real input plumbing into original source, not a world frame.
add_executable(rloft_original_code2_level_view src/code2_original_level_view.cpp)
set_target_properties(rloft_original_code2_level_view PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_level_view PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_level_view PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME original_code2_334c4_all_32_level_grids_pre_ray
        COMMAND rloft_original_code2_level_view)
    set_tests_properties(original_code2_334c4_all_32_level_grids_pre_ray
        PROPERTIES TIMEOUT 35)
endif()

# Original CODE.2 181B4 archive read through bounded DOS read-only callbacks.
# This tool takes user-owned LEVELS.DAT and is not a player startup path.
add_executable(rloft_original_code2_level_loader src/code2_original_level_loader.cpp)
set_target_properties(rloft_original_code2_level_loader PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_level_loader PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_level_loader PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()

# Source 181B4/03838/32954 real world record -> original 335DC/3263C
# with interpreted original SINTAB raster code and authentic RES4:0.
# This source diagnostic is not a DOS-equivalent playable renderer.
add_executable(rloft_original_code2_world_frame src/code2_original_world_frame.cpp)
set_target_properties(rloft_original_code2_world_frame PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_world_frame PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_world_frame PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()

# Media-free 36CEC original source-call, EH7 shape and SINTAB plan proof.
# This test certifies input/control flow only, NOT RES3 VGA pixels.
add_executable(rloft_original_code2_36cec_plan_tests
    src/test_original_code2_36cec_columns.cpp)
set_target_properties(rloft_original_code2_36cec_plan_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME original_code2_36cec_source_res3_plan
        COMMAND rloft_original_code2_36cec_plan_tests)
endif()

# Genuine CODE.2 07938/06CAD SAVE.DAT deserializer and chained
# original 181B4 direct LEVnnn loader; owner retail CODE.2 DGROUP only.
add_executable(rloft_original_code2_save_game src/code2_original_save_game.cpp)
set_target_properties(rloft_original_code2_save_game PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_save_game PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_save_game PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()

# Bounded original CODE.2 369D8/36B14 indexed VGA per-plane executor.
add_executable(rloft_original_code2_369d8_raster_test
    src/test_original_code2_369d8_raster.cpp)
set_target_properties(rloft_original_code2_369d8_raster_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_369d8_raster_test PRIVATE /W3)
else()
    target_compile_options(rloft_original_code2_369d8_raster_test PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_369d8_res3_vga_plane_raster
        COMMAND rloft_original_code2_369d8_raster_test)
endif()

# Retail 298F0 had an incorrectly shortened five-argument source ABI.
# Keep this sentinel test independent of any owned RES image contents.
add_executable(rloft_original_code2_298f0_party_abi_test
    src/test_original_code2_298f0_party_masked_abi.cpp)
set_target_properties(rloft_original_code2_298f0_party_abi_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_298f0_party_abi_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_298f0_party_abi_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_298f0_six_argument_res1_masked_abi
        COMMAND rloft_original_code2_298f0_party_abi_test)
endif()

# Authentic CODE.2 373BF signed decimal / 30508 centered original RES0
# font and 303C8 glyph dispatch, isolated from private retail media.
add_executable(rloft_original_code2_status_text_test
    src/test_original_code2_30508_status_text.cpp)
set_target_properties(rloft_original_code2_status_text_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_status_text_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_status_text_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_373bf_30508_status_text_vga_glyphs
        COMMAND rloft_original_code2_status_text_test)
endif()


# Part 26: actual original 1F8E0/1F8FC and 17A68 source control flow,
# with explicitly synthetic callback receipts. No original game assets,
# rendered pixels, or live-gameplay acceptance is asserted by this test.
add_executable(rloft_original_code2_scene_actor_source_test
    src/test_original_code2_1f8fc_17a68.cpp)
set_target_properties(rloft_original_code2_scene_actor_source_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_scene_actor_source_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_scene_actor_source_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_1f8fc_scene_and_17a68_postload_source
        COMMAND rloft_original_code2_scene_actor_source_test)
endif()

# Canonical original CODE.2 sampled SFX software queue (no AIL output).
# Exercise actual 28110 source against isolated synthetic guest memory.
add_executable(rloft_original_code2_28110_sound_queue_test
    src/test_original_code2_28110_audio_queue.cpp)
set_target_properties(rloft_original_code2_28110_sound_queue_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_28110_sound_queue_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_28110_sound_queue_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_28110_sampled_sound_queue
        COMMAND rloft_original_code2_28110_sound_queue_test)
endif()

# Exact compiled original 28168 -> 27F24 sound dispatch to synthetic test
# Miles callbacks. Verifies source ABI, NOT host-audible playback.
add_executable(rloft_original_code2_28168_audio_service_test
    src/test_original_code2_28168_audio_service.cpp)
set_target_properties(rloft_original_code2_28168_audio_service_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_28168_audio_service_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_28168_audio_service_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_28168_27f24_driver_abi
        COMMAND rloft_original_code2_28168_audio_service_test)
endif()

# Owner-private narrow Creative Voice PCM8 and real WinMM playback probe.
# No original VOC assets, silent audio-driver fixtures or software sound
# synthesis are shipped. Only --play requests an actual OS device.
add_executable(rloft_host_original_voc_waveout
    src/host_original_voc_waveout.cpp)
add_executable(rloft_original_voc_pcm8_tests
    src/test_original_voc_pcm8.cpp)
foreach(target IN ITEMS rloft_host_original_voc_waveout
                        rloft_original_voc_pcm8_tests)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    if(MSVC)
        target_compile_options(${target} PRIVATE /Od /W3)
    else()
        target_compile_options(${target} PRIVATE -O0 -Wall -Wextra -Wpedantic)
    endif()
endforeach()
if(WIN32)
    target_link_libraries(rloft_host_original_voc_waveout PRIVATE winmm)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_private_creative_voice_pcm8_strict_decode
        COMMAND rloft_original_voc_pcm8_tests)
endif()

# Preserve the scene-palette selector synthetic regression. This source
# existed without a CMake owner and was never compiled by standard CI.
add_executable(rloft_original_scene_palette_selection_test
    tests/scene_palette_selection.cpp)
set_target_properties(rloft_original_scene_palette_selection_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_scene_palette_selection
        COMMAND rloft_original_scene_palette_selection_test)
endif()

# Source-selected 181B4 archive index policy: every valid original level,
# with no hard-coded 22/23/26 pair selection. No original media needed.
add_executable(rloft_original_code2_saved_level_policy_test
    src/test_original_code2_saved_level_policy.cpp)
set_target_properties(rloft_original_code2_saved_level_policy_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME canonical_code2_source_selected_all_32_level_indices
        COMMAND rloft_original_code2_saved_level_policy_test)
endif()

# Original 1C77C dialogue input text 1C368 indexed C-runtime / DOS reader.
# Does not simulate modal user input or certify finished conversations.
add_executable(rloft_original_code2_1c368_dialogue_text_test
    src/test_original_code2_1c368_dialogue_text.cpp)
set_target_properties(rloft_original_code2_1c368_dialogue_text_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_1c368_dialogue_text_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_1c368_dialogue_text_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_1c368_dialogue_indexed_dos_text_source
        COMMAND rloft_original_code2_1c368_dialogue_text_test)
endif()

# Original 002A8 shared interpreter across mixed guest map/script opcodes.
# Unsupported later dialogue is rejected before earlier guest mutations.
add_executable(rloft_original_code2_002a8_script_source_test
    src/test_original_code2_002a8_script_source.cpp)
set_target_properties(rloft_original_code2_002a8_script_source_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_002a8_script_source_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_002a8_script_source_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_002a8_source_script_multi_opcode_execution
        COMMAND rloft_original_code2_002a8_script_source_test)
endif()

# Original shared actor event chain 1BDAC -> 0D550 -> 0CEE4/0CD44/0AD24.
# Synthetic party state; no proprietary game media or fabricated game returns.
add_executable(rloft_original_code2_0d550_actor_source_test
    src/test_original_code2_1bdac_0d550_actor_source.cpp)
set_target_properties(rloft_original_code2_0d550_actor_source_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_0d550_actor_source_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_0d550_actor_source_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_1bdac_0d550_type15_saving_throw_party_damage
        COMMAND rloft_original_code2_0d550_actor_source_test)
endif()

# Source 1BDAC -> 0D550 close-range target-exhaustion branch.
# All actual targets rejected by original 0D4E8; hit/effects remain guarded.
add_executable(rloft_original_code2_0d550_near_exhausted_test
    src/test_original_code2_0d550_near_exhausted_source.cpp)
set_target_properties(rloft_original_code2_0d550_near_exhausted_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_0d550_near_exhausted_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_0d550_near_exhausted_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_1bdac_0d550_near_0d4e8_exhaustion
        COMMAND rloft_original_code2_0d550_near_exhausted_test)
endif()

# Genuine 0D550 -> 0C9A0 -> 0EFA0/0CEE4 -> 0CD44 source hit path.
# No host damage substitute, lethal/game-over and missed-text branches guarded.
add_executable(rloft_original_code2_0d550_eligible_hit_test
    src/test_original_code2_0d550_eligible_hit_source.cpp)
set_target_properties(rloft_original_code2_0d550_eligible_hit_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_0d550_eligible_hit_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_0d550_eligible_hit_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_0d550_0c9a0_original_nonlethal_hit_effect
        COMMAND rloft_original_code2_0d550_eligible_hit_test)
endif()

# Original CODE2 miss-message formatting through source 3210F/385B0/1C21C.
add_executable(rloft_original_code2_combat_miss_text_test
    src/test_original_code2_combat_miss_source.cpp)
set_target_properties(rloft_original_code2_combat_miss_text_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_combat_miss_text_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_combat_miss_text_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_combat_miss_original_3210f_385b0_1c21c
        COMMAND rloft_original_code2_combat_miss_text_test)
endif()

# Complete compiled original 0CBC8/0CADC game-over choreography, with
# synthetic observer boundaries only. Not a live 0CBC8 host certification.
add_executable(rloft_original_code2_0cbc8_source_sequence_test
    src/test_original_code2_0cbc8_source_sequence.cpp)
set_target_properties(rloft_original_code2_0cbc8_source_sequence_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_0cbc8_source_sequence_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_0cbc8_source_sequence_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_0cbc8_0cadc_source_game_over_sequence
        COMMAND rloft_original_code2_0cbc8_source_sequence_test)
endif()

# Real planar VGA memory copy through original 0CADC/2A078.
add_executable(rloft_original_code2_game_over_vga_strip_test
    src/test_original_code2_game_over_vga_strip.cpp)
set_target_properties(rloft_original_code2_game_over_vga_strip_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_game_over_vga_strip_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_game_over_vga_strip_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_0cadc_2a078_real_vga_strip
        COMMAND rloft_original_code2_game_over_vga_strip_test)
endif()

# Original 2E614 source timing, advanced by the actual CODE2 IRQ8 body.
add_executable(rloft_original_code2_2e614_irq8_timing_test
    src/test_original_code2_2e614_irq8_timing.cpp)
set_target_properties(rloft_original_code2_2e614_irq8_timing_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_2e614_irq8_timing_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_2e614_irq8_timing_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_2e614_original_irq8_timed_transition
        COMMAND rloft_original_code2_2e614_irq8_timing_test)
endif()

# Monotonic host clock drives compiled original 30708 IRQ8, without
# presuming DOS RTC frequency or enabling production game-over.
add_executable(rloft_original_code2_irq8_elapsed_scheduler_test
    src/test_original_code2_irq8_elapsed_scheduler.cpp)
set_target_properties(rloft_original_code2_irq8_elapsed_scheduler_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_irq8_elapsed_scheduler_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_irq8_elapsed_scheduler_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_30708_elapsed_host_irq8_scheduler
        COMMAND rloft_original_code2_irq8_elapsed_scheduler_test)
endif()

# Shared original-code clip/overlay source callbacks, independent of game level.
add_executable(rloft_original_code2_source_clip_overlay_test
    src/test_original_code2_source_clip_overlay.cpp)
set_target_properties(rloft_original_code2_source_clip_overlay_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_source_clip_overlay_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_source_clip_overlay_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_source_296c0_29738_2978c_2a6b0_clip_overlay
        COMMAND rloft_original_code2_source_clip_overlay_test)
endif()

# Per-owner private 171FC/181B4 source file transport contract.
add_executable(rloft_original_code2_live_level_files_test
    src/test_original_code2_live_level_files.cpp)
set_target_properties(rloft_original_code2_live_level_files_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_171fc_181b4_live_private_files
        COMMAND rloft_original_code2_live_level_files_test)
endif()

# Part 30: test the exact compiled original CODE.2 2E120 movement
# integrator, 2DF2C vector, 2DBAC camera and 2D5D0 collision path.
# Synthetic guest RAM is test-only, never an independent DOS capture.
add_executable(rloft_original_code2_2e120_camera_source_test
    src/test_original_code2_2e120_camera.cpp)
set_target_properties(rloft_original_code2_2e120_camera_source_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_original_code2_2e120_camera_source_test PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_original_code2_2e120_camera_source_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_2e120_2dbac_camera_source
        COMMAND rloft_original_code2_2e120_camera_source_test)
endif()

# Source 28168->27F24 real host driver boundary. This test does NOT
# request a host sound device; it proves no fake status/completion on
# invalid/unprepared digital SFX before native output is available.
add_executable(rloft_original_source_winmm_boundary_test
    src/test_original_source_winmm_bridge.cpp)
set_target_properties(rloft_original_source_winmm_boundary_test PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(WIN32)
    target_link_libraries(rloft_original_source_winmm_boundary_test PRIVATE winmm)
endif()
if(MSVC)
    target_compile_options(rloft_original_source_winmm_boundary_test PRIVATE /Od /W3)
else()
    target_compile_options(rloft_original_source_winmm_boundary_test PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
if(BUILD_TESTING)
    add_test(NAME canonical_code2_original_28168_host_sfx_status_boundary
        COMMAND rloft_original_source_winmm_boundary_test)
endif()

# First native Level22 window: original CODE2 module and Win32 VGA scanout.
if(WIN32)
    add_executable(rloft_code2_level22_window
        src/original_code2_level22_window.cpp src/original_le_image.cpp)
    set_target_properties(rloft_code2_level22_window PROPERTIES
        CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    if(MSVC)
        target_compile_options(rloft_code2_level22_window PRIVATE /Od /bigobj /W3)
    else()
        target_compile_options(rloft_code2_level22_window PRIVATE -O0 -Wall -Wextra -Wpedantic)
    endif()
    target_link_libraries(rloft_code2_level22_window
        PRIVATE Threads::Threads winmm user32 gdi32)
endif()
