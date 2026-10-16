#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/combat_hand_dispatch.hpp"
#include "rloft/combat_hand_pair_eligibility.hpp"
#include "rloft/exploration_state.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

// The live CODE.2 214B8 -> 24478 -> 26018 decision using the actual
// current level header +290. No invented OBJECTS[-1] prebase bytes,
// target, dice, IRQ clock, cooldown or NPC damage.
enum class CombatLiveHandStatus : std::uint8_t {
    AttackCandidate,              // positive item; E5F0 still to execute
    ActorBlocked,
    ActionBusy,
    PairVetoed,
    MissingOriginalEmptyHandData,
    SpecialAction,
    CategoryEAction,
    OtherAction,
};

struct CombatLiveHandPlan final {
    CombatLiveHandStatus status{CombatLiveHandStatus::OtherAction};
    CombatHandDispatchPlan dispatch{};
    bool pair_checked{};
    bool pair_selected_eligible{};

    [[nodiscard]] bool may_enter_player_attack() const noexcept {
        return status==CombatLiveHandStatus::AttackCandidate &&
               pair_checked && pair_selected_eligible &&
               !dispatch.definitively_blocked() &&
               dispatch.route==CombatHandDispatchRoute::PlayerAttack;
    }
};

class CombatLiveHandRequestModel final {
public:
    [[nodiscard]] static CombatLiveHandPlan inspect(
        const PartyImage& party,
        const ObjectCatalog& objects,
        const LevelState& level,
        std::size_t actor,
        std::size_t hand);
};

} // namespace rloft
