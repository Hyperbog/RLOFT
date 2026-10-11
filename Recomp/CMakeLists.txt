cmake_minimum_required(VERSION 3.22)
project(RavenloftReconstructionLinkGate LANGUAGES CXX)

# Full canonical CODE.1/CODE.2 compile/link/leaf-execution witness; not yet DOS boot.
add_executable(rloft_recomp_link_gate
    src/code1_compilation_unit.cpp
    src/code2_compilation_unit.cpp
    src/original_le_image.cpp
    src/link_gate_main.cpp)
add_executable(rloft_original_image_gate
    src/original_le_image.cpp
    src/original_image_gate.cpp)
foreach(target IN ITEMS rloft_recomp_link_gate rloft_original_image_gate)
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
    add_test(NAME canonical_reconstruction_compiles_links_and_runs COMMAND rloft_recomp_link_gate)
    add_test(NAME original_le_image_and_fixup_integrity COMMAND rloft_original_image_gate)
endif()
