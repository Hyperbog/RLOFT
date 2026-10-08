#pragma once

#include <cstdint>

#include "rloft/object_catalog.hpp"

namespace rloft {

// Original CODE.2 logical C910: post-dice weapon/creature damage
// gate. NPC definition DWORD at +31h is supplied by the caller,
// not inferred from the live projected actor cache.
// This is read-only and does not apply damage or consume RNG.
struct CombatWeaponDamageGateResult final {
    std::int32_t damage_after_gate{};
    bool creature_restriction_triggered{};
    bool minimum_clamped{};
};

class CombatWeaponDamageGateModel final {
public:
    [[nodiscard]] static CombatWeaponDamageGateResult inspect(
        const ObjectCatalog& objects,
        std::int32_t dispatched_item_id,
        std::uint32_t npc_definition_flags_31,
        std::int32_t damage_before_gate);
};

} // namespace rloft
