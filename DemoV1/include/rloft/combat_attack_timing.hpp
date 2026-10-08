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
    // Integer argument passed to 34B48 for the action animation.
    std::uint32_t animation_interval{};
    // Only C80C-successful attacks write a hand-ready deadline and
    // the resulting damage state. Miss/no-target leaves them alone.
    std::optional<std::uint32_t> hand_ready_deadline;
    std::optional<std::int32_t> stored_damage;
};

class CombatAttackTimingModel final {
public:
    // This pure function does not roll the original RNG, select a
    // target, write party fields, emit sounds, or advance a clock.
    // The caller supplies the item ID captured by 26018 and passed
    // to E5F0, A7E4 from 29CC4, the C80C hit result and damage.
    [[nodiscard]] static CombatAttackTimingPlan inspect(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t hand,
        std::int32_t dispatched_item_id,
        std::uint32_t gameplay_maintenance_tick_a7e4,
        bool hit_succeeded,
        std::int32_t computed_damage);
};

} // namespace rloft
