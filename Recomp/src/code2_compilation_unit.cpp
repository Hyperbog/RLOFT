// Canonical CODE.2 source included unchanged: authenticated image and entry boundary.
#include "../../Code2/Code2.cpp"
#include "original_le_image.h"
#include <vector>
namespace {
static bool startup_edge_observed=false;
static void observe_startup_edge(code2::Machine& m) {
    startup_edge_observed=(m.cpu.eax==0x134ABCD9u);
}
}
extern "C" bool rloft_recomp_code2_memory_probe() {
    std::vector<code2::u8> data(code2::kObject2VirtualSize,0);
    code2::Machine state{};state.data=data.data();
    constexpr code2::u32 value=0x93E9B117u;
    code2::store_le32(data.data()+0x12640u,value);
    return code2::function_3a80a(state)==value;
}
extern "C" bool rloft_recomp_code2_entry_edge_probe() {
    std::vector<code2::u8> data(code2::kObject2VirtualSize,0);
    code2::Machine state{};state.data=data.data();state.cpu.eax=0x134ABCD9u;
    code2::Runtime111Boundary entry{};startup_edge_observed=false;
    entry.tail_35418=&observe_startup_edge;
    code2::function_353a0(state,entry);
    return startup_edge_observed;
}
extern "C" bool rloft_recomp_code2_original_data_probe(const rloft_recomp::OriginalLeImage& img) {
    if(img.objects.size()!=2 || img.objects[1].virtual_size!=code2::kObject2VirtualSize ||
       img.entry_offset!=0x353A0u)return false;
    std::vector<code2::u8> data=img.objects[1].raw;
    code2::Machine state{};state.data=data.data();
    return code2::function_3a80a(state)==code2::load_le32(data.data()+0x12640u);
}
