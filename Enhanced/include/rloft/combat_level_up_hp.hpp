#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/exploration_state.hpp"

namespace rloft {

// Original CODE.2 logical DC7C..DDA0. The 6 supported classes use
// DGROUP:6821 die sides and 2x19 signed Constitution bonuses at 67FA.
// These arrays are extracted from an owned original CODE.2, not invented.
struct CombatLevelUpHpTables final {
    static constexpr std::size_t kClassCount=6U;
    static constexpr std::size_t kConstitutionValues=19U;
    std::array<std::uint8_t,kClassCount> die_sides{};
    std::array<std::array<std::int8_t,kConstitutionValues>,2U> constitution_bonus{};

    [[nodiscard]] static std::optional<CombatLevelUpHpTables> from_code2(
        const std::vector<std::uint8_t>& code2) noexcept;
};

struct CombatLevelUpHpProjection final {
    std::uint8_t class_code{};
    std::uint8_t constitution{};
    std::uint8_t die_sides{};
    std::uint16_t original_die_roll{};
    std::int16_t constitution_modifier{};
    std::int32_t original_level_argument{};
    std::int16_t final_hp_increase{};
    std::uint16_t current_hp_before{};
    std::uint16_t max_hp_before{};
    std::uint16_t current_hp_after{};
    std::uint16_t max_hp_after{};
    std::uint32_t rng_state_after{};
};

class CombatLevelUpHpModel final {
public:
    // Read-only, exact single-call DC7C projection. party must be
    // observed at DC7C ENTRY, after 50DC, optional 3EA4/3DE4,
    // 3210F and timed 1C21C calls. A copy of the supplied original
    // RNG state consumes the single 30CA2 die draw; caller must later
    // apply state in its verified original temporal order.
    // Unsupported class/constitution/lane snapshots return nullopt.
    [[nodiscard]] static std::optional<CombatLevelUpHpProjection> inspect(
        const PartyImage& party_at_dc7c_entry,
        std::size_t actor_index,
        std::size_t class_lane,
        std::int32_t original_level_argument,
        std::uint32_t original_rng_state,
        const CombatLevelUpHpTables& original_tables) noexcept;
};

} // namespace rloft
