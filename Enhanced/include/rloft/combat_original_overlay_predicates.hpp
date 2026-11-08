#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "rloft/combat_original_overlay_probe.hpp"

namespace rloft {

// Original 1C1DC samples a DWORD bitset at DGROUP 22200.
// Read the actual DWORD at the moment of the original CALL:
// the program may change flags between two classifications.
struct CombatOriginalObjectTypeBitServices final {
    std::function<std::optional<std::uint32_t>(std::uint32_t)>
        original_dgroup_dword;
};

class CombatOriginalOverlayPredicatesModel final {
public:
    [[nodiscard]] static std::optional<bool> bit_1c1dc(
        std::uint32_t bit_number,
        const CombatOriginalObjectTypeBitServices&);

    [[nodiscard]] static std::optional<bool> type_2ad18(
        std::uint32_t type,
        const CombatOriginalObjectTypeBitServices&);

    [[nodiscard]] static bool type_2ade0(
        std::uint32_t type) noexcept;

    // Installs the two reconstructed original predicates
    // into 2AE30 without replacing its live scene-record reader.
    [[nodiscard]] static CombatOriginalOverlayProbeServices
    bind_2ae30(
        CombatOriginalObjectTypeBitServices live_game_bits,
        CombatOriginalOverlayProbeServices existing);
};

} // namespace rloft
