#pragma once

#include <cstdint>
#include <optional>

#include "rloft/object_catalog.hpp"

namespace rloft {

// Original CODE.2 E5F0:E76A..E899 chooses the arguments passed
// to 1C328/OriginalRandom::dice. This is a NON-RNG planner; the
// caller must not roll before C80C has actually hit its target.
struct CombatDamageDiceArgs final {
    std::uint8_t count{};
    std::uint8_t sides{};
    std::int32_t per_die_bonus{};
    std::int32_t final_bonus{};
};

enum class CombatDamageAfterRoll : std::uint8_t {
    // Most attacks require original C910 weapon/creature processing
    // before original E284 applies any resulting NPC injury.
    WeaponCreatureGate,
    // Item 4Ah has a different charged-rod/creature-type branch.
    RodOfSmiting,
    // 84h (HARM) clamps the NPC's HP toward the rolled value,
    // sets outgoing injury to zero, and emits effect code 24h.
    HarmClampsNpcHp,
};

struct CombatDamageDicePlan final {
    // Absent ONLY for 7Fh when original 22E40 helper result
    // has not yet been supplied. Never consume a guessed RNG draw.
    std::optional<CombatDamageDiceArgs> dice;
    CombatDamageAfterRoll after_roll{
        CombatDamageAfterRoll::WeaponCreatureGate};
    // Original E899 optional 38110 visual effect discriminator.
    std::optional<std::uint32_t> hit_effect_code;
    bool requires_22e40_helper{};
};

class CombatDamageDiceModel final {
public:
    // Non-pseudo positive weapons require the caller's genuine
    // E6C9..E768 class/stat-derived final bonus; the serialized
    // object record alone does not provide that value.
    // Spell-hand 7Fh requires original logical 22E40's output
    // to derive its variable d6 count; absent output -> no dice.
    [[nodiscard]] static CombatDamageDicePlan inspect(
        const ObjectCatalog& objects,
        std::int32_t dispatched_item_id,
        std::optional<std::int32_t> original_stat_bonus = std::nullopt,
        std::optional<std::int32_t> helper_22e40_value = std::nullopt);
};

} // namespace rloft
