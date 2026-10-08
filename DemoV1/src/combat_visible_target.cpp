#include "rloft/combat_visible_target.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

std::int32_t CombatVisibleTargetModel::scan(
    std::uint8_t group,
    const std::vector<CombatProjectedTarget>& records,
    const std::vector<std::uint8_t>& actor_flags) {
    if (group > 1U) {
        throw std::out_of_range("CODE.2 DACC group must be 0 or 1");
    }
    if (records.empty()) return -1;
    // The DOS DACC loop obtains the count from F038(group), then
    // searches each draw rank count-1 .. 0 by matching record+0Ah.
    // No assumed array sort order is permitted.
    for (std::size_t rank=records.size(); rank-- > 0U;) {
        const CombatProjectedTarget* chosen=nullptr;
        for (const auto& record : records) {
            if (record.draw_rank == rank) {
                chosen=&record;
                break;
            }
        }
        if (!chosen) {
            // Original code would index past the record buffer on
            // malformed ranks. Reject instead of inventing a target.
            throw std::invalid_argument(
                "CODE.2 DACC projected target rank missing");
        }
        if (chosen->blocked_marker != 0U) continue;
        const auto actor=static_cast<std::size_t>(
            chosen->live_actor_index);
        if (actor >= actor_flags.size()) {
            throw std::out_of_range(
                "CODE.2 DACC projected target actor index");
        }
        const auto flags=actor_flags[actor];
        if ((flags & 0x02U) != 0U) continue;

        // Group 0 has horizontal values <= 128; group 1 > 128.
        if (group == 0U && chosen->horizontal > 0x80U) continue;
        if (group == 1U && chosen->horizontal <= 0x80U) continue;

        // CODE.2 1DB61..1DB6D: ((u16 vertical - 0x7D00)
        // arithmetic-shift-right 3) >= 0x3B. Test the signed
        // difference before division, not unsigned wraparound.
        const auto delta=
            static_cast<std::int32_t>(chosen->vertical)-0x7D00;
        if ((delta >> 3) < 0x3B) continue;
        if ((flags & 0x10U) != 0U)
            return static_cast<std::int32_t>(actor);
    }
    return -1;
}

} // namespace rloft
