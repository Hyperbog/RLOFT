cmake_minimum_required(VERSION 3.22)
project(RavenloftReconstructionLinkGate LANGUAGES CXX)

# Whole-source LINK + EXECUTION gate, not a faithful DOS game executable.
add_executable(rloft_recomp_link_gate
    src/code1_compilation_unit.cpp
    src/code2_compilation_unit.cpp
    src/link_gate_main.cpp)
set_target_properties(rloft_recomp_link_gate PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
if(MSVC)
    target_compile_options(rloft_recomp_link_gate PRIVATE /Od /bigobj /W3)
else()
    target_compile_options(rloft_recomp_link_gate PRIVATE -O0 -Wall -Wextra -Wpedantic)
endif()
include(CTest)
if(BUILD_TESTING)
    add_test(NAME canonical_reconstruction_compiles_links_and_runs COMMAND rloft_recomp_link_gate)
endif()
