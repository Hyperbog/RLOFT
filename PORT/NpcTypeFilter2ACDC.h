#pragma once
#include <cstdint>

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0x2ACDC.
// Keep the address-oriented name until every caller establishes a stronger
// semantic label. The original returns true for type indices < 0x22 and 0x50.
struct NpcTypeFilter2ACDC {
    static bool includes(std::uint16_t npcType) noexcept {
        return npcType < 0x22u || npcType == 0x50u;
    }
};

} // namespace ravenloft::game
