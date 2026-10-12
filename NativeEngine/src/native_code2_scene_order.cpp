#include "rloft_native/native_code2_scene_order.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
std::uint16_t word(const NativeCode2SceneRecord22& b,
                   std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(
        std::uint16_t(b[offset]) | (std::uint16_t(b[offset+1U])<<8U));
}
void put_word(NativeCode2SceneRecord22& b,std::size_t offset,
              std::uint16_t v) noexcept {
    b[offset]=static_cast<std::uint8_t>(v);
    b[offset+1U]=static_cast<std::uint8_t>(v>>8U);
}
}

NativeCode2SceneOrder order_native_code2_scene_records_2c3f1(
    const std::vector<NativeCode2SceneRecord22>& records) {
    constexpr std::uint16_t kUntouchedSelectionCeiling=0x7D00U;
    constexpr std::size_t kOriginalDoubleBufferBytes=0x1130U;
    constexpr std::size_t kRecordSize=22U;
    if(records.size()>kOriginalDoubleBufferBytes/kRecordSize)
        throw std::invalid_argument("CODE.2 scene record buffer exceeded");
    for(const auto& record:records)
        if(word(record,2U)>=kUntouchedSelectionCeiling)
            throw std::invalid_argument(
                "CODE.2 scene projection violates source selection bound");

    NativeCode2SceneOrder out;
    out.marked_records=records;
    out.source_indices.reserve(records.size());
    for(std::size_t draw=0;draw<records.size();++draw) {
        std::uint32_t best=kUntouchedSelectionCeiling;
        std::size_t selected=records.size();
        for(std::size_t i=0;i<out.marked_records.size();++i) {
            const auto height=word(out.marked_records[i],2U);
            // x86 2C430 JGE: a tie does not displace the earlier entry.
            if(height<best) {best=height;selected=i;}
        }
        if(selected==records.size())
            throw std::runtime_error("CODE.2 scene selection lost source invariant");
        out.source_indices.push_back(selected);
        auto& chosen=out.marked_records[selected];
        // Source 2C7B6 writes WORD [record+0x0A] before 2C7BD adds
        // BYTE 0x7D to [record+3], not a WORD add to [record+2].
        put_word(chosen,10U,static_cast<std::uint16_t>(draw));
        chosen[3]=static_cast<std::uint8_t>(chosen[3]+0x7DU);
    }
    return out;
}
} // namespace rloft_native
