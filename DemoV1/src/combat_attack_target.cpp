#include "rloft/combat_attack_target.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

CombatAttackTargetChoice CombatAttackTargetModel::choose(
    const ObjectCatalog& objects,
    std::size_t actor,
    std::int32_t hand_item_id,
    std::int32_t requested_target_id,
    std::uint8_t party_mode_258a8,
    const CombatTargetPriorityModel::GroupScan& projected_group_scan) {
    if (actor>=4U || hand_item_id<-1 || requested_target_id<-1) {
        throw std::out_of_range("CODE.2 E5F0 actor/item/target");
    }
    CombatAttackTargetChoice result{};

    // E5F0:1E5FB..1E614: explicit target survives unchanged.
    // Otherwise call DB94(actor), which scans group(actor&1)
    // then its opposite group *only if no target is found*.
    if (requested_target_id==-1) {
        result.used_db94_search=true;
        result.target_id=CombatTargetPriorityModel::choose(
            static_cast<std::uint32_t>(actor),
            projected_group_scan);
    } else {
        result.target_id=requested_target_id;
    }

    // E5F0:1E621..1E669: only in party mode FFh, when target
    // exists and actor has bit1 set (actors 2 and 3), reject
    // target if the attacking item is -1 OR its original
    // OBJECTS WORD +35h low two bits are NOT exactly 2.
    //
    // This happens BEFORE C80C and does not mean "attack input
    // denied": animation and other action-state paths still run.
    if (party_mode_258a8==0xFFU &&
        result.target_id!=-1 &&
        (actor&2U)!=0U) {
        const bool qualified=hand_item_id>=0 &&
            objects.at(static_cast<std::size_t>(hand_item_id))
                .raw_byte_35_low2==2U;
        if (!qualified) {
            result.target_id=-1;
            result.blocked_by_rear_party_rule=true;
        }
    }
    return result;
}

} // namespace rloft
