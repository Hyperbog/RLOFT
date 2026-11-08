#include "rloft/assassin_relic_transition.hpp"

#include <algorithm>

namespace rloft {

bool AssassinRelicTransition::is_relic(
    std::uint16_t object_id) noexcept {
    return std::find(
               kRelics.begin(),
               kRelics.end(),
               object_id) != kRelics.end();
}

bool AssassinRelicTransition::completes_on_pickup(
    std::uint16_t picked_object_id,
    const std::function<bool(std::uint16_t)>& party_contains) {
    if (!is_relic(picked_object_id) || !party_contains) {
        return false;
    }

    for (const auto relic : kRelics) {
        if (relic == picked_object_id) continue;
        if (!party_contains(relic)) return false;
    }
    return true;
}

bool AssassinRelicTransition::consumed_after_transition(
    std::uint16_t object_id) noexcept {
    return std::find(
               kConsumedAfterTransition.begin(),
               kConsumedAfterTransition.end(),
               object_id) != kConsumedAfterTransition.end();
}

}  // namespace rloft
