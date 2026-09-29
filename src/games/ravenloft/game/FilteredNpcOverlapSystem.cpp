#include "games/ravenloft/game/FilteredNpcOverlapSystem.h"
#include "games/ravenloft/game/NpcTypeFilter2ACDC.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
bool inWrappedInclusive(std::int32_t value,
                        std::int32_t center,
                        std::uint32_t radius) {
    const auto v = static_cast<std::uint32_t>(value);
    const auto c = static_cast<std::uint32_t>(center);
    return v >= c - radius && v <= c + radius;
}
}

bool FilteredNpcOverlapSystem::overlaps(std::int32_t candidateYFixed,
                                        std::int32_t candidateXFixed,
                                        int moverId) const {
    for (std::size_t i = 0; i < work_.npcs.size(); ++i) {
        if (static_cast<int>(i) == moverId) continue;
        const auto& actor = work_.npcs[i];
        if (!NpcTypeFilter2ACDC::includes(actor.type)) continue;
        // Runtime byte +0x0B bit 0x02 (state bit 25) excludes the actor.
        if ((actor.state & 0x02000000u) != 0) continue;
        if (actor.type >= npcs_.count())
            throw std::runtime_error("Runtime NPC type outside NPCS.DAT");

        // Unlike CODE.2 0x2C800, 0x2C8E4 uses the definition half-extent
        // verbatim; it does not add 0x3E8.
        const auto radius = static_cast<std::uint32_t>(
            npcs_.at(actor.type).collisionHalfExtent);
        if (inWrappedInclusive(candidateXFixed, actor.xFixed, radius) &&
            inWrappedInclusive(candidateYFixed, actor.yFixed, radius))
            return true;
    }
    return false;
}

} // namespace ravenloft::game
