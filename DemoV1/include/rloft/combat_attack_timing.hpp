#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/exploration_state.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

// Original CODE.2 E5F0:1E9D0..1EA75 timing *plan*, expressed in
// the ORIGINAL pauseable gameplay-maintenance clock A7E4. It is
// not VGA refresh, the IRQ F58C tick, milliseconds, or host frames.
struct CombatAttackTimingPlan final {
    // Integer interval passed to CODE.2 logical 24B48 for
    // presentation and subsequent action-state scheduling.
    std::uint32_t animation_interval{};
    // Only a nonzero C80C hit writes +19E hand-ready and damage.
    std::optional<std::uint32_t> hand_ready_deadline;
    std::optional<std::int32_t> stored_damage;
    // 24B48 writes the OTHER action-state dword (+10B) for any
    // nonzero interval, EVEN ON A MISS, after graphics presentation.
    std::optional<std::uint32_t> action_state_deadline;
};

class CombatAttackTimingModel final {
public:
    // This pure function does not roll the original RNG, select a
    // target, write party fields, emit sounds, or advance a clock.
    // The caller supplies the item ID captured by 26018 and passed
    // to E5F0, two separate A7E4 values from 29CC4 (before
    // the hit-only write and after action presentation), plus the
    // C80C hit result and post-weapon damage.
    [[nodiscard]] static CombatAttackTimingPlan inspect(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t hand,
        std::int32_t dispatched_item_id,
        std::uint32_t gameplay_tick_a7e4_before_hit_write,
        std::uint32_t gameplay_tick_a7e4_after_presentation,
        bool hit_succeeded,
        std::int32_t computed_damage);
};

} // namespace rloft
