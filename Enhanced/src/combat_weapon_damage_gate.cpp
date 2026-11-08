#include "rloft/combat_weapon_damage_gate.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

CombatWeaponDamageGateResult CombatWeaponDamageGateModel::inspect(
    const ObjectCatalog& objects,
    std::int32_t dispatched_item_id,
    std::uint32_t npc_definition_flags_31,
    std::int32_t damage_before_gate) {
    if (dispatched_item_id < -1) {
        throw std::out_of_range("CODE.2 C910 invalid weapon ID");
    }

    // C910:C91C..C94C: empty hand uses original category 9.
    // Nonempty items use record +2F category, but ANY nonzero
    // enchantment byte +37 makes the effective category 11h.
    // The raw category/enchantment are independent fields.
    std::uint8_t category=9U;
    std::uint8_t enchantment=0U;
    if (dispatched_item_id>=0) {
        const auto& item=objects.at(
            static_cast<std::size_t>(dispatched_item_id));
        category=item.item_category;
        enchantment=item.enchantment;
        if (enchantment!=0U) category=11U;
    }

    std::int32_t damage=damage_before_gate;
    bool vetoed=false;
    switch(category) {
        case 9U:
            // C95F: TEST CH,80h = DWORD bit15.
            if ((npc_definition_flags_31&0x00008000U)!=0U) {
                damage=0;
                vetoed=true;
            }
            break;
        case 10U:
            // C966: TEST CH,40h = DWORD bit14.
            if ((npc_definition_flags_31&0x00004000U)!=0U) {
                damage=0;
                vetoed=true;
            }
            break;
        case 11U:
            // C96D: NPC bit17, then MOVZX enchantment BYTE and
            // CMP DWORD reg,-2 / JL. MOVZX gives 0..255, which
            // can NEVER compare signed-less than -2, so the
            // original fallthrough zeroes damage whenever bit17
            // is set. This is an x86 fact, not tabletop rules.
            if ((npc_definition_flags_31&0x00020000U)!=0U) {
                damage=0;
                vetoed=true;
            }
            // C980: NPC bit18, MOVZX same byte, CMP -2 / JGE.
            // Every 0..255 value satisfies signed >= -2, so this
            // branch can never add a second veto.
            break;
        default:
            // C958 jumps directly to final minimum for others.
            break;
    }

    // C993..C99D clamps even a zeroed or negative result to 1.
    const bool minimum_clamped=damage<1;
    if (minimum_clamped) damage=1;
    return {damage,vetoed,minimum_clamped};
}

} // namespace rloft
