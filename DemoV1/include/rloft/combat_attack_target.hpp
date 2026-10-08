#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/combat_target_priority.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

// CODE.2 E5F0:1E5FB..1E669: target resolver before C80C hit
// arithmetic. This does NOT test projected NPC validity for an
// explicit index, nor run 1E578 actor-status side effects.
struct CombatAttackTargetChoice final {
    std::int32_t target_id{-1};
    bool used_db94_search{};
    bool blocked_by_rear_party_rule{};
};

class CombatAttackTargetModel final {
public:
    [[nodiscard]] static CombatAttackTargetChoice choose(
        const ObjectCatalog& objects,
        std::size_t actor,
        std::int32_t hand_item_id,
        std::int32_t requested_target_id,
        std::uint8_t party_mode_258a8,
        const CombatTargetPriorityModel::GroupScan& projected_group_scan);
};

} // namespace rloft
