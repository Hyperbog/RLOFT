#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/combat_xp_distribution.hpp"
#include "rloft/exploration_state.hpp"

namespace rloft {

// Source: CODE.2 object-2 at 0x75F24+7D9C (6x20 WORD thresholds)
// and 0x75F24+6827 (6 numerator/denominator pairs).
// Only six class codes 0..5 have a proved 20-entry table.
// The original lacks bounds checking for unsupported class bytes.
struct CombatProgressionTables final {
    static constexpr std::size_t kClassCount=6U;
    static constexpr std::size_t kThresholdCount=20U;
    std::array<std::array<std::uint16_t,kThresholdCount>,kClassCount>
        thresholds_in_thousands{};
    std::array<std::uint8_t,kClassCount> attack_numerator{};
    std::array<std::uint8_t,kClassCount> attack_denominator{};

    // Accept only the exact original-table fingerprint confirmed from
    // immutable supplied CODE.2. Never invent a fallback progression table.
    [[nodiscard]] static std::optional<CombatProgressionTables> from_code2(
        const std::vector<std::uint8_t>& code2) noexcept;
};

struct CombatXpClassLevelProjection final {
    bool visited{};
    std::uint8_t class_code{};
    std::uint32_t projected_experience{};
    std::int16_t original_level{};
    std::uint8_t table_level{};  // 0..20
    // If true, DDA4 must call 50DC and conditionally 3EA4/3DE4,
    // then 3210F, 1C21C, DC7C in original time/state order.
    bool requires_original_level_change_services{};
};

struct CombatXpLevelProjection final {
    std::array<std::array<CombatXpClassLevelProjection,3>,
               PartyImage::kActorCount> actors{};
    bool has_unexecuted_level_change_services{};
};

class CombatXpLevelModel final {
public:
    // Consumes a *separately verified* DDA4 XP projection.
    // A missing or malformed class table/actor lane is unsupported,
    // not interpreted as zero experience or unchanged level.
    [[nodiscard]] static std::optional<CombatXpLevelProjection> inspect(
        const PartyImage& original_party,
        const CombatXpDistributionPlan& award,
        const CombatProgressionTables& tables) noexcept;
};

struct CombatAttackTargetProjection final {
    std::size_t winning_class_track{};
    std::int16_t winning_class_level{};
    std::uint8_t class_code{};
    std::uint8_t stored_target_byte{};
};

class CombatAttackTargetModel final {
public:
    // CODE.2 DBD4..DC63: uses OBSERVED class levels after any
    // original level-up callbacks, never speculative pre-callback levels.
    [[nodiscard]] static std::optional<CombatAttackTargetProjection> inspect(
        const PartyImage& observed_party,
        std::size_t actor_index,
        const CombatProgressionTables& tables) noexcept;
};

} // namespace rloft
