// Canonical CODE.1 source included unchanged: imported-original-image and isolated witnesses.
#include "../../Code1/Code1.cpp"
#include "original_le_image.h"
#include <vector>
extern "C" bool rloft_recomp_code1_memory_probe() {
    std::vector<code1::u8> data(code1::kDataObjectVirtualSize,0);
    code1::Machine state{};state.data=data.data();
    constexpr code1::u32 value=0x8A7031D2u;
    code1::store_le32(data.data()+0x2934u,value);
    return code1::function_7f50(state)==value;
}
extern "C" bool rloft_recomp_code1_original_data_probe(const rloft_recomp::OriginalLeImage& img) {
    if(img.objects.size()!=2 || img.objects[1].virtual_size!=code1::kDataObjectVirtualSize ||
       img.entry_offset!=code1::kEntryOffset)return false;
    std::vector<code1::u8> data=img.objects[1].raw;
    code1::Machine state{};state.data=data.data();
    return code1::function_7f50(state)==code1::load_le32(data.data()+0x2934u);
}
