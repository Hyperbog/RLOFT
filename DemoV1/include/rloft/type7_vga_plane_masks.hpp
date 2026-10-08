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

    // Original 3263C main surface compositor shares these CODE.2
    // tables with 369D8 but does not clamp C6F4 to seven. The
    // unmodified shipped tables additionally support an EXACT
    // 8-column mask for phases 0..2 (indices 8/16/24). Phase 3
    // width 8 and all larger widths are unverified table overreads:
    // do not split or replace their native behavior with guesses.
    [[nodiscard]] Type7VgaRunMasks for_surface_main_run(
        std::uint32_t viewport_x,
        std::uint8_t column_count) const;

    // Original bound data-object byte C029 is 1 in the supplied CODE.2.
    // The game can toggle it at 6617, so the initial value is not a
    // substitute for mutable renderer state during live play.
    [[nodiscard]] bool initial_individual_columns_mode() const noexcept {
        return initial_individual_columns_mode_;
    }

    // CODE.2 original data object byte EF4D is the initial 0/1 page.
    [[nodiscard]] std::uint8_t initial_video_page() const noexcept {
        return initial_video_page_;
    }

private:
    std::array<std::array<std::uint8_t, 32>, 3> plane_tables_{};
    bool initial_individual_columns_mode_{};
    std::uint8_t initial_video_page_{};
};

} // namespace rloft
