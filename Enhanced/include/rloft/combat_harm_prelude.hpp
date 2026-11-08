#pragma once

#include <cstdint>

namespace rloft {

// Original CODE.2 E5F0:E815..E83B, strictly after C80C hit
// and actual 1C328(1,4,0,0) RNG result, BEFORE C910/E284.
// Pure plan; does not write NPC memory, consume RNG or play FX.
struct CombatHarmPreludeResult final {
    std::int16_t hp_after_direct_clamp{};
    bool direct_clamp_applied{};
    std::int32_t outgoing_damage_before_c910{};
    std::uint32_t visual_effect_code{};
};

class CombatHarmPreludeModel final {
public:
    [[nodiscard]] static CombatHarmPreludeResult inspect(
        std::int16_t npc_hp_before,
        std::int32_t actual_1d4_roll);
};

} // namespace rloft
