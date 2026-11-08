#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/exploration_state.hpp"

namespace rloft {

// Original CODE.2 logical 3DE4..3EA3 and 3EA4..3F74.
// Each routine consumes its OWN 20-by-8 level/group table, but shares
// eight signed DWORD group counts. Never replace these tables with
// assumed tabletop spell progression.
struct CombatLevelSpellTables final {
    static constexpr std::size_t kLevels=20U;
    static constexpr std::size_t kGroups=8U;
    std::array<std::array<std::uint8_t,kGroups>,kLevels>
        class_3de4_level_groups{};
    std::array<std::array<std::uint8_t,kGroups>,kLevels>
        class_3ea4_level_groups{};
    std::array<std::int32_t,kGroups> group_entry_counts{};

    // Reads original CODE.2 object-2 at file 75F24 + offsets
    // 62B0, 6350, 6420. Refuses unmatched asset fingerprints.
    [[nodiscard]] static std::optional<CombatLevelSpellTables> from_code2(
        const std::vector<std::uint8_t>& code2) noexcept;
};

enum class CombatLevelSpellRoutine : std::uint8_t {
    Original3DE4,
    Original3EA4,
};

struct CombatLevelSpellProjection final {
    PartyImage projected_party{}; // all OTHER actors and bytes unchanged
    std::size_t actor{};
    std::int32_t original_new_level{};
    CombatLevelSpellRoutine routine{CombatLevelSpellRoutine::Original3DE4};
    std::uint32_t spell_bytes_written{};
};

// Read-only model of one original 3DE4 or 3EA4 call. The caller must
// observe the party AFTER prior synchronous DDA4 callbacks. Invalid
// source indexes are unsupported, NOT invented original behavior.
class CombatLevelSpellModel final {
public:
    [[nodiscard]] static std::optional<CombatLevelSpellProjection> inspect(
        const PartyImage& observed_party,
        std::size_t actor,
        std::int32_t new_level,
        CombatLevelSpellRoutine routine,
        const CombatLevelSpellTables& tables) noexcept;
};

} // namespace rloft
