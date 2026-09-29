#include "games/ravenloft/game/NpcTargetExclusionD4E8.h"

namespace ravenloft::game {
namespace {
std::uint16_t u16(const std::array<std::uint8_t,0x248>& r, std::size_t o) {
    return static_cast<std::uint16_t>(r[o]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[o+1]) << 8u);
}
}

int NpcTargetExclusionD4E8::evaluate(const data::CharacterRecord& c,
                                     const data::NpcDefinition& npc) {
    // movzx status; cmp 0x4000; jl continue
    if (u16(c.raw,0x10) >= 0x4000u) return 1;
    if ((c.raw[0x132] & 0x20u) != 0) return 1;
    if ((c.raw[0x133] & 0x02u) != 0 &&
        npc.raw[0x1D] <= 4u &&
        (npc.raw[0x3A] & 0x20u) != 0) return 1;
    return 0;
}

} // namespace ravenloft::game
