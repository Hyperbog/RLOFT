cmake_minimum_required(VERSION 3.22)
project(RavenloftReconstructionLinkGate LANGUAGES CXX)

# Full canonical CODE.1/CODE.2 compile/link/leaf-execution witness; not yet DOS boot.
add_executable(rloft_recomp_link_gate
    src/code1_compilation_unit.cpp
    src/code2_compilation_unit.cpp
    src/original_le_image.cpp
    src/link_gate_main.cpp)
add_executable(rloft_code2_startup_gate
    src/original_le_image.cpp
    src/code2_startup_gate.cpp)
add_executable(rloft_original_image_gate
    src/original_le_image.cpp
    src/original_image_gate.cpp)
foreach(target IN ITEMS rloft_recomp_link_gate rloft_original_image_gate rloft_code2_startup_gate)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    if(MSVC)
        target_compile_options(${target} PRIVATE /Od /bigobj /W3)
    else()
        target_compile_options(${target} PRIVATE -O0 -Wall -Wextra -Wpedantic)
    endif()
endforeach()
include(CTest)
if(BUILD_TESTING)
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
    add_test(NAME canonical_code2_mouse_absent_source_to_real_dos_warning_boundary
        COMMAND rloft_code2_startup_gate --mouse-absent-to-warning)
    add_test(NAME canonical_code2_original_termination_table_to_exit_boundary COMMAND rloft_code2_startup_gate --termination-probe)
    add_test(NAME canonical_code2_original_arena_split_coalesce_reuse COMMAND rloft_code2_startup_gate --arena-roundtrip-probe)
    add_test(NAME canonical_code2_env_allocation_failure_frees_original_arena_block COMMAND rloft_code2_startup_gate --env-rollback-probe)
endif()
