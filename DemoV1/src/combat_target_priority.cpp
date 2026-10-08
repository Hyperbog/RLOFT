#include "rloft/combat_target_priority.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

std::int32_t CombatTargetPriorityModel::choose(
    std::uint32_t preference_flags,
    const GroupScan& scan) {
    if (!scan) {
        throw std::invalid_argument("CODE.2 DB94 requires a DACC scanner");
    }

    // DB94 tests only the low bit. Low=0 searches group0 first,
    // low=1 searches group1 first. On no target (-1), try the
    // other group exactly once; do NOT merge/sort both groups.
    const auto primary = static_cast<std::uint8_t>(
        preference_flags & 1U);
    const auto selected = scan(primary);
    if (selected != -1) return selected;
    return scan(static_cast<std::uint8_t>(primary ^ 1U));
}

} // namespace rloft
