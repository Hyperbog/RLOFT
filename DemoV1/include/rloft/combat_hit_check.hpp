#pragma once

#include <cstdint>

namespace rloft {

// Input to the original CODE.2 C80C player-vs-NPC hit check. Inputs
// are decoded original-record fields or outputs of already separate
// engine helpers (Strength modifier, burden rank and raw 1d20).
struct CombatHitCheckInputs final {
    std::uint8_t live_npc_runtime_flags{};
    std::uint8_t base_attack_target{};
    std::int8_t npc_armor_class{};
    std::uint8_t weapon_enchantment{};
    int strength_attack_adjustment{};
    bool apply_strength_adjustment{true};
    std::uint8_t burden_rank{};
    bool bless{};
    bool aid{};
    bool stone_giant_gauntlets{};
    // Absent/unconsumed on held-NPC auto-hit. A non-held attack
    // requires an explicit, previously rolled 1..20 original die.
    std::uint8_t raw_d20{};
};

struct CombatHitCheckResult final {
    bool hit{};
    // Exact DWORD EAX return at CODE.2 C80C. Original returns
    // only 0/1, NOT its raw or modified d20. E5F0 stores this
    // field and Rod of Smiting incorrectly compares it to 20.
    std::uint32_t original_eax_return{};
    bool auto_hit_held{};
    int burden_penalty{};
    int threshold{};
    int roll_bonus{};
    int modified_d20{};
};

class CombatHitCheckModel final {
public:
    // CODE.2 C80C..C90C, preserving x86 branch ordering.
    // Does NOT roll dice, apply damage, dispatch audio, trigger attacks,
    // schedule hands or change world state.
    [[nodiscard]] static CombatHitCheckResult resolve(
        const CombatHitCheckInputs& inputs);
};

} // namespace rloft
