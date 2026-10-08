#pragma once

#include <cstdint>
#include <functional>

#include "rloft/combat_original_overlay.hpp"

namespace rloft {

// Original 1A3B8 reads and can mutate DOS DGROUP mouse globals.
// Its OUT arguments are three caller-provided WORD pointers:
// 9E31,9E33,9E35 when called from the overlay 1A288.
struct CombatOriginalOverlayPositionState final {
    std::uint8_t disabled_d680{};
    std::uint16_t clamp_mode_a6be{};
    std::uint16_t constrain_mode_a6bc{};
    std::int16_t row_a6b4{};
    std::int16_t column_a6b6{};
    std::int16_t third_word_a6b8{};
};

struct CombatOriginalOverlayPositionServices final {
    // Original 33C90 called with two SIGN-EXTENDED DWORD coordinates.
    std::function<void(std::int32_t,std::int32_t)> pointer_move_33c90;
    // Original 1F820 runs AFTER both mode flags are reset when D680!=0.
    std::function<void()> disabled_side_effect_1f820;
};

struct CombatOriginalOverlayPositionResult final {
    bool requested_position_update{};
    std::int16_t output_row{};
    std::int16_t output_column{};
    std::int16_t output_third_word{};
};

class CombatOriginalOverlayPositionModel final {
public:
    [[nodiscard]] static CombatOriginalOverlayPositionResult
    run_1a3b8(
        CombatOriginalOverlayPositionState& live_mouse,
        const CombatOriginalOverlayPositionServices& original_services);

    // Inject this verified 1A3B8 operation into the existing
    // 1A288 callback, preserving the other page/overlay services.
    [[nodiscard]] static CombatOriginalOverlayServices bind_1a288(
        CombatOriginalOverlayPositionState& live_mouse,
        CombatOriginalOverlayPositionServices original_services,
        CombatOriginalOverlayServices remaining_overlay);
};

} // namespace rloft
