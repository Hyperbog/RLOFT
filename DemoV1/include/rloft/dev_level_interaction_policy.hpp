#pragma once

namespace rloft {

// Explicit native-only testing permission. The source gameplay
// handlers do NOT change: this only removes the default safety
// suppression of two existing preview viewport actions.
// Independent of --dev-inspect, which remains read-only.
struct DevLevelInteractionPolicy final {
    // Never enable mutation for normal boot/character creation
    // by interpreting its nonexistent preview flag implicitly.
    [[nodiscard]] static constexpr bool valid(
        bool preview_requested,bool interactions_requested) noexcept {
        return !interactions_requested || preview_requested;
    }

    // Original production gameplay can use the source-backed
    // item and hand handlers as before. Preview may use those
    // exact handlers only after explicit --dev-interact opt-in.
    [[nodiscard]] static constexpr bool allow(
        bool is_preview,bool interactions_requested) noexcept {
        return !is_preview || interactions_requested;
    }
};
} // namespace rloft
