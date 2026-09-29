#include "games/ravenloft/game/SceneryCollisionSystem.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
bool inWrappedInclusive(std::int32_t value, std::int32_t center, std::uint32_t radius) {
    // CODE.2 uses unsigned JB/JBE comparisons after 32-bit add/subtract.
    const auto v = static_cast<std::uint32_t>(value);
    const auto c = static_cast<std::uint32_t>(center);
    const auto lo = c - radius;
    const auto hi = c + radius;
    return v >= lo && v <= hi;
}
}

SceneryCollisionResult SceneryCollisionSystem::query(
    std::int32_t candidateYFixed,
    std::int32_t candidateXFixed,
    int moverId,
    std::uint8_t moverNpcTraits39,
    std::uint16_t currentLevel,
    const SceneryCollisionCallbacks& callbacks) const {

    for (const auto& p : work_.scenery) {
        if (p.type >= init_.scenery().size())
            throw std::runtime_error("Runtime scenery type outside INIT table");
        const auto radius = static_cast<std::uint32_t>(init_.scenery()[p.type].radius);
        if (radius <= 1u) continue; // 0x2CCF4..0x2CCF7.

        // Runtime record +4 is X and +0 is Y.  The original routine compares
        // its second argument against X and first argument against Y.
        if (!inWrappedInclusive(candidateXFixed, p.xFixed, radius) ||
            !inWrappedInclusive(candidateYFixed, p.yFixed, radius))
            continue;

        if (moverId == -1) {
            if (p.type == 0x54u) {
                // 0x2CD7F..0x2CE39: this scenery is nonblocking unless its
                // runtime field +0x14 is in state 0x40 and the current level is
                // 0x17 or 0x0A, in which case it performs a special transition.
                if ((p.field14 & 0x01C0u) != 0x0040u) continue;
                if (currentLevel == 0x17u || currentLevel == 0x0Au) {
                    if (callbacks.playerState) {
                        auto& ps=*callbacks.playerState;
                        if (currentLevel == 0x17u) {
                            ps.partyXFixed=static_cast<std::int32_t>(0x00038000u);
                            ps.partyYFixed=static_cast<std::int32_t>(0x000A8000u);
                            ps.facingDegrees=180;
                            if (callbacks.setFacing1C0C0) callbacks.setFacing1C0C0(180);
                            if (callbacks.loadLevel181B4) callbacks.loadLevel181B4(0x13);
                            ps.currentLevel=0x13;
                        } else {
                            ps.partyXFixed=static_cast<std::int32_t>(0x00068000u);
                            ps.partyYFixed=static_cast<std::int32_t>(0x003E8000u);
                            ps.facingDegrees=180;
                            if (callbacks.setFacing1C0C0) callbacks.setFacing1C0C0(180);
                            if (callbacks.loadLevel181B4) callbacks.loadLevel181B4(0x06);
                            ps.currentLevel=0x06;
                        }
                    }
                    if (callbacks.onType54Special) callbacks.onType54Special(currentLevel);
                    return SceneryCollisionResult::Special;
                }
                continue;
            }
            if (p.type == 0x7Au) {
                if (callbacks.onType7ASpecial) callbacks.onType7ASpecial();
                return SceneryCollisionResult::Special;
            }
            return SceneryCollisionResult::Blocked;
        }

        if (moverId >= 0 && moverId < 0x190) {
            // 0x2CE75..0x2CE97: NPCS.DAT +0x39 bit 0x80 makes scenery
            // collision transparent for this mover.
            return (moverNpcTraits39 & 0x80u)
                ? SceneryCollisionResult::Clear
                : SceneryCollisionResult::Blocked;
        }

        return SceneryCollisionResult::Blocked;
    }

    return SceneryCollisionResult::Clear;
}

} // namespace ravenloft::game
