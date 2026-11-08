#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/combat_level_change_gate.hpp"

namespace rloft {

// DDA4:DF0E..DF86: the exact synchronous call *order* immediately after
// writing a changed class level. This is a list of required services,
// not an implementation of those services or their timer/RNG effects.
enum class CombatLevelChangeService : std::uint8_t {
    Select50DC,
    Class3EA4,
    Class3DE4,
    Format3210F,
    Queue1C21C,
    HpIncreaseDC7C,
};

struct CombatLevelChangeSequence final {
    std::array<CombatLevelChangeService,5> events{};
    std::size_t count{};
    std::int32_t selected_50dc_track{-1};
    std::size_t actor{};
    std::size_t changed_track{};
    std::int16_t old_level{};
    std::int16_t new_level{};
};

class CombatLevelChangeSequenceModel final {
public:
    // observed_party must be captured *after* DDA4 writes the new
    // level WORD but BEFORE invoking 50DC. Earlier callbacks can
    // mutate later reads; do not precompute all transitions from a
    // pre-level-up party and claim them to be live timing.
    [[nodiscard]] static std::optional<CombatLevelChangeSequence> inspect(
        const PartyImage& observed_party,
        std::size_t actor,
        std::size_t changed_track,
        std::int16_t old_level,
        std::int16_t new_level) noexcept;
};

} // namespace rloft
