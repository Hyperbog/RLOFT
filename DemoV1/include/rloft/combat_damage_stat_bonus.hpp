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
// The original signed table at DGROUP 684Bh must be decoded
// from original loaded memory: a raw CODE.2 file offset is NOT
// proof of the table's contents.
class CombatDamageStatBonusModel final {
public:
    [[nodiscard]] static std::int32_t inspect(
        const PartyImage& party,
        std::size_t actor,
        std::optional<std::int8_t> observed_table_684b_entry =
            std::nullopt);
};

} // namespace rloft
