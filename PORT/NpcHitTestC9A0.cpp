#include "games/ravenloft/game/NpcHitTestC9A0.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint32_t u32(const std::array<std::uint8_t,0x248>& r, std::size_t o) {
    return static_cast<std::uint32_t>(r[o]) |
           (static_cast<std::uint32_t>(r[o+1]) << 8u) |
           (static_cast<std::uint32_t>(r[o+2]) << 16u) |
           (static_cast<std::uint32_t>(r[o+3]) << 24u);
}
}

int NpcHitTestC9A0::evaluate(std::size_t characterIndex,
                             const data::CharacterRecord& c,
                             const data::NpcDefinition& npc,
                             const data::RuntimePlacement22& actor,
                             const NpcHitTestContextC9A0& ctx) {
    if (!ctx.rollD20 || !ctx.characterArmorClassEFA0)
        throw std::runtime_error("NpcHitTestC9A0 requires rollD20 and characterArmorClassEFA0");

    const int rolled = ctx.rollD20();
    int adjustedRoll = rolled;
    if (rolled == 20) return 1;
    if (rolled == 1) return 0;

    const int armorClass = ctx.characterArmorClassEFA0(characterIndex);
    const std::uint32_t flags = u32(c.raw,0x132);

    if ((flags & 0x40u) != 0) {
        if (ctx.effect871C) ctx.effect871C(characterIndex,6);
        return 0;
    }
    if ((flags & 0x20u) != 0) adjustedRoll = rolled - 2;
    if ((flags & 0x100u) != 0) --adjustedRoll;
    if ((flags & 0x08u) != 0) adjustedRoll -= 2;

    int required = static_cast<int>(npc.raw[0x22]) - armorClass;
    if ((actor.raw[0x08] & 0x80u) != 0) required += 4;
    return adjustedRoll >= required ? 1 : 0;
}

} // namespace ravenloft::game
