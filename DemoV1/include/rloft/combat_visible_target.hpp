#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Minimal decoded fields from the original 22-byte projected target
// record at CODE.2 DGROUP 27148 + group*2260 + index*16h.
// The other record bytes remain owned by the scene renderer.
struct CombatProjectedTarget final {
    std::uint16_t horizontal{};
    std::uint16_t vertical{};
    std::uint16_t live_actor_index{};
    std::uint16_t blocked_marker{};
    std::uint16_t draw_rank{};
};

class CombatVisibleTargetModel final {
public:
    // CODE.2 DACC: choose highest-ranked eligible projected live
    // actor in group 0/1. Actor_flags are the original byte at
    // DGROUP 14D8B+actor_id*16h, decoded by the caller. This routine
    // does NOT choose world-distance or apply damage.
    [[nodiscard]] static std::int32_t scan(
        std::uint8_t group,
        const std::vector<CombatProjectedTarget>& records,
        const std::vector<std::uint8_t>& actor_flags);
};

} // namespace rloft
