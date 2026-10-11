#include <iostream>

extern "C" bool rloft_recomp_code1_memory_probe();
extern "C" bool rloft_recomp_code2_memory_probe();
extern "C" bool rloft_recomp_code2_entry_edge_probe();

int main() {
    const bool code1=rloft_recomp_code1_memory_probe();
    const bool code2=rloft_recomp_code2_memory_probe();
    const bool entry=rloft_recomp_code2_entry_edge_probe();
    std::cout << "CANONICAL_CODE1_CPP_COMPILE_LINK_MEMORY=" << (code1 ? "PASS" : "FAIL") << '\n';
    std::cout << "CANONICAL_CODE2_CPP_COMPILE_LINK_MEMORY=" << (code2 ? "PASS" : "FAIL") << '\n';
    std::cout << "CODE2_ENTRY_353A0_TO_35418_BOUNDARY=" << (entry ? "PASS" : "FAIL") << '\n';
    std::cout << "ORIGINAL_DOS_STARTUP_EXECUTED=NO\n"
                 "COMPLETE_GAME_RECOMPILATION=NO\n";
    return (code1 && code2 && entry) ? 0 : 1;
}
