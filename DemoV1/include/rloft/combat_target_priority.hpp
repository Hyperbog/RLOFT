#pragma once

#include <cstdint>
#include <functional>

namespace rloft {

// CODE.2 DB94 is a strict two-pass preference/fallback wrapper around
// DACC. The callback models DACC's still-separate live NPC visibility
// scan and returns an original actor index or -1 if none qualifies.
class CombatTargetPriorityModel final {
public:
    using GroupScan = std::function<std::int32_t(std::uint8_t)>;

    [[nodiscard]] static std::int32_t choose(
        std::uint32_t preference_flags,
        const GroupScan& scan);
};

} // namespace rloft
