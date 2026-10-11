// Canonical-source compilation unit. Compile the complete Code2.cpp unchanged;
// tests are restricted to proven memory and entry-dispatch leaf behavior.
#include "../../Code2/Code2.cpp"

#include <vector>

namespace {
static bool startup_edge_observed=false;
static void observe_startup_edge(code2::Machine& m) {
    // A source-level boundary witness, NOT a substituted 0x35418 implementation.
    startup_edge_observed=(m.cpu.eax==0x134ABCD9u);
}
}
extern "C" bool rloft_recomp_code2_memory_probe() {
    std::vector<code2::u8> image(code2::kObject2VirtualSize, 0);
    code2::Machine state{};
    state.data=image.data();
    constexpr code2::u32 expected=0x93E9B117u;
    code2::store_le32(image.data()+0x12640u, expected);
    return code2::function_3a80a(state)==expected;
}
extern "C" bool rloft_recomp_code2_entry_edge_probe() {
    std::vector<code2::u8> image(code2::kObject2VirtualSize, 0);
    code2::Machine state{};
    state.data=image.data();
    state.cpu.eax=0x134ABCD9u;
    code2::Runtime111Boundary entry{};
    startup_edge_observed=false;
    entry.tail_35418=&observe_startup_edge;
    code2::function_353a0(state,entry);
    return startup_edge_observed;
}
