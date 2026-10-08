#include "rloft/combat_class_level.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

std::int32_t CombatClassLevelModel::for_class(
    const PartyImage& party,
    std::size_t actor,
    std::int32_t class_code) {
    if (actor>=PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 22E40 actor index");
    }
    const auto base=actor*PartyImage::kActorSize;
    // 22E48..22E87 compares MOVSX signed class bytes. The
    // signed level WORD in the same lane is also MOVSX.
    for (std::size_t lane=0U;lane<3U;++lane) {
        const auto candidate=static_cast<std::int8_t>(
            party.bytes[base+0x15U+lane]);
        if (static_cast<std::int32_t>(candidate)!=class_code)
            continue;
        const auto index=base+0x20U+lane*2U;
        const auto word=static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(party.bytes[index]) |
            (static_cast<std::uint16_t>(party.bytes[index+1U])<<8U));
        return static_cast<std::int16_t>(word);
    }
    // Literal return at original 22E87, not class level zero.
    return 1;
}

} // namespace rloft
