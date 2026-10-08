#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "rloft/combat_original_overlay.hpp"

namespace rloft {

// CODE.2 2AE30: hit testing is performed against the OTHER
// scene/render page (DWORD C050 XOR low bit 1), not against
// the active native window. The source uses 22-byte DOS records.
struct CombatOriginalOverlayProbeState final {
    std::uint8_t skip_probing_d680{};
    std::uint32_t active_scene_page_c050{};
};

struct CombatOriginalOverlayProbeServices final {
    // Read an unsigned original WORD from a logical DOS address.
    // Missing memory is distinct from a genuine zero WORD.
    std::function<std::optional<std::uint16_t>(std::uint32_t)>
        original_word;
    // Original 2AD18 and 2ADE0 predicates are evaluated in
    // source order only on a visible flag-zero matching record.
    std::function<std::optional<bool>(std::uint32_t)>
        predicate_2ad18;
    std::function<std::optional<bool>(std::uint32_t)>
        predicate_2ade0;
};

class CombatOriginalOverlayProbeModel final {
public:
    // Returns classification 0, 1, or 2 observed in 2AE30,
    // or nullopt if the original source table/predicate is absent.
    [[nodiscard]] static std::optional<std::int32_t> probe_2ae30(
        const CombatOriginalOverlayProbeState&,
        std::uint32_t original_row_bits,
        std::uint32_t original_column_bits,
        const CombatOriginalOverlayProbeServices&);

    [[nodiscard]] static CombatOriginalOverlayServices bind_1a288(
        CombatOriginalOverlayProbeState& live_state,
        CombatOriginalOverlayProbeServices original_services,
        CombatOriginalOverlayServices remaining_overlay);
};

} // namespace rloft
