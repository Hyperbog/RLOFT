#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/exploration_state.hpp"

namespace rloft {

// Original CODE.2 logical 50DC..518B, called from DDA4:DF1F.
// The original accepts (mode, actor_index), then returns -1 or an
// ordinal class slot 0..2. It is NOT the class-level side effect.
// Unlike a modern eligibility test, it has mode-dependent class
// preference and a first-class level-9 exception.
class CombatLevelChangeGateModel final {
public:
    // nullopt represents an invalid host actor index, not a valid
    // original return. Any nonzero mode selects the original "4" path.
    [[nodiscard]] static std::optional<std::int32_t> inspect(
        const PartyImage& party,
        std::size_t actor_index,
        std::int32_t mode) noexcept;
};

} // namespace rloft
