#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/exploration_state.hpp"

namespace rloft {

// E5F0:E6C9..E768 computes the FINAL bonus parameter for
// ordinary OBJECTS-based 1C328 damage dice. This is not the
// per-die weapon bonus in OBJECTS byte +3Ah.
//
// The original signed table at DGROUP 684Bh was recovered from
// CODE.2 object-2 data at physical file offset 0x7C76F. Scores
// 0..25 are original constant bytes; above 25 the executable
// reads adjacent DGROUP bytes and an observed value is required.
class CombatDamageStatBonusModel final {
public:
    [[nodiscard]] static std::int32_t inspect(
        const PartyImage& party,
        std::size_t actor,
        std::optional<std::int8_t> observed_table_684b_entry =
            std::nullopt);
};

} // namespace rloft
