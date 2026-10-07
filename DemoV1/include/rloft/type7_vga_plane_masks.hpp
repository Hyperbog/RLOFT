#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/game_source.hpp"

namespace rloft {

// CODE.2 369D8 uses three 32-byte dgroup tables to choose the VGA plane
// masks for 36B14's run of 1..7 destination columns. The first plane pass is
// always invoked; the second and third run only for nonzero masks.
struct Type7VgaRunMasks final {
    std::array<std::uint8_t, 3> planes{};
    std::uint8_t screen_phase{};
    std::uint8_t column_count{};

    [[nodiscard]] std::size_t active_call_count() const noexcept {
        return 1U +
               static_cast<std::size_t>(planes[1] != 0U) +
               static_cast<std::size_t>(planes[2] != 0U);
    }
};

class Type7VgaPlaneMaskModel final {
public:
    // Original CODE.2 object-2 image, not invented tables. 369D8 reads
    // c668/c688/c6a8 using ((destination_x & 3) << 3) + run_length.
    [[nodiscard]] static Type7VgaPlaneMaskModel from_code2(
        const std::vector<std::uint8_t>& code2_bytes);
    [[nodiscard]] static Type7VgaPlaneMaskModel load(
        const GameSource& source);

    [[nodiscard]] Type7VgaRunMasks for_run(
        std::uint32_t viewport_x,
        std::uint8_t column_count) const;

    // Original bound data-object byte C029 is 1 in the supplied CODE.2.
    // The game can toggle it at 6617, so the initial value is not a
    // substitute for mutable renderer state during live play.
    [[nodiscard]] bool initial_individual_columns_mode() const noexcept {
        return initial_individual_columns_mode_;
    }

private:
    std::array<std::array<std::uint8_t, 32>, 3> plane_tables_{};
    bool initial_individual_columns_mode_{};
};

} // namespace rloft
