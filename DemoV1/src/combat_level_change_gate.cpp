#include "rloft/combat_level_change_gate.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {
std::int16_t signed_word(const PartyImage& party,
                         std::size_t offset) noexcept {
    const auto bits=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(party.bytes[offset]) |
        (static_cast<std::uint16_t>(party.bytes[offset+1U])<<8U));
    // Avoid implementation-defined unsigned->signed out-of-range cast.
    return static_cast<std::int16_t>(
        bits<=0x7FFFU ? static_cast<std::int32_t>(bits) :
                        static_cast<std::int32_t>(bits)-0x10000);
}
} // namespace

std::optional<std::int32_t> CombatLevelChangeGateModel::inspect(
    const PartyImage& party,
    std::size_t actor_index,
    std::int32_t mode) noexcept {
    if(actor_index>=PartyImage::kActorCount) return std::nullopt;
    const auto base=actor_index*PartyImage::kActorSize;

    // 50E5..50F7: the exact original status exclusion mask.
    if ((static_cast<std::uint16_t>(signed_word(
             party,base+0x10U)) & 0xF240U)!=0U)
        return -1;

    // 50FD..514F uses signed CLASS code 3 and the class-code-1
    // first-slot/level>=9 exception ONLY when mode == 0.
    if(mode==0) {
        for(std::size_t i=0U;i<3U;++i) {
            if(party.bytes[base+0x15U+i]==0x03U)
                return static_cast<std::int32_t>(i);
        }
        if(party.bytes[base+0x15U]==0x01U &&
           signed_word(party,base+0x20U)>=9)
            return 0;
        return -1;
    }

    // 514F..5184: any nonzero mode uses signed class code 4,
    // in slot order, and has no level>=9 exception.
    for(std::size_t i=0U;i<3U;++i) {
        if(party.bytes[base+0x15U+i]==0x04U)
            return static_cast<std::int32_t>(i);
    }
    return -1;
}

} // namespace rloft
