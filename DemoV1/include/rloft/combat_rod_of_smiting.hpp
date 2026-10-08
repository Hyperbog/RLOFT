#pragma once

#include <cstdint>
#include <optional>

#include "rloft/combat_damage_dice.hpp"

namespace rloft {

// CODE.2 E8BB..E957 reachable path when called by E5F0.
// Its "critical = 20" condition compares the *return value*
// from C80C, NOT the underlying d20. C80C returns only 0 or 1,
// so the 20 branch is unreachable during a normal successful hit.
struct CombatRodOfSmitingPlan final {
    // Extra 1C328 roll occurs AFTER the initial ordinary item
    // dice, consuming two ADDITIONAL original RNG draws.
    std::optional<CombatDamageDiceArgs> additional_dice;
    // With actual C80C return=1, the E8F4/E93D charge decrement
    // paths are unreachable; the original +31 indexed charge
    // byte is not even read.
    bool charge_consumed{};
    bool skip_c910{true};
};

class CombatRodOfSmitingModel final {
public:
    // Only call on a successful E5F0/C80C hit; miss returns zero
    // and never enters the E6C9..E963 post-hit damage branch.
    [[nodiscard]] static CombatRodOfSmitingPlan inspect(
        std::uint16_t npc_type_id,
        std::uint32_t actual_c80c_return_value);
};

} // namespace rloft
