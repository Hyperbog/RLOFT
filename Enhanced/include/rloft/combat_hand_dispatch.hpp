#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/exploration_state.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

// Immutable reconstruction of the *bounded* CODE.2 214B8 and 26018
// hand-action gate and dispatch classification. This does NOT grant
// approval from the full two-hand 24478 legality routine.
enum class CombatHandDispatchGate : std::uint8_t {
    AvailableForPairValidation,
    InactiveActor,
    ActorConditionBlocked,
    ActionStateBusy
};

enum class CombatHandDispatchRoute : std::uint8_t {
    // 26018 260B2..260D6 dispatches category 9,10,11 to E5F0.
    PlayerAttack,
    // The known 23F50 special predicate branches *before* normal
    // category dispatch. Effects/containers must use their own path.
    SpecialItem,
    // 26018 category E hardcoded-item dispatch.
    CategoryE,
    // Other item categories use later 26018 branches; no guessed attack.
    OtherAction
};

struct CombatHandDispatchPlan final {
    CombatHandDispatchGate gate{};
    CombatHandDispatchRoute route{};
    std::int16_t object_id{-1};
    std::uint8_t category{0x0AU};
    std::uint32_t original_action_state{};
    bool special_item{};
    // 214B8 calls 24478 *before* entering 26018; this plan alone
    // must never be treated as permission to execute an attack.
    bool requires_24478_approval{true};

    [[nodiscard]] bool definitively_blocked() const noexcept {
        return gate != CombatHandDispatchGate::AvailableForPairValidation;
    }
};

class CombatHandDispatchModel final {
public:
    // No RNG, world mutation, item exchange, cooldown writes or damage.
    [[nodiscard]] static CombatHandDispatchPlan inspect(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t hand);
};

} // namespace rloft
