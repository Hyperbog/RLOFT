#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/exploration_session.hpp"
#include "rloft/exploration_state.hpp"

namespace rloft {

struct CellPosition final {
    int column{};
    int row{};

    friend bool operator==(const CellPosition& a, const CellPosition& b) noexcept {
        return a.column == b.column && a.row == b.row;
    }
};

class ExplorationWorldModel final {
public:
    static constexpr std::uint16_t kCurrentVisible = 0x0040U;
    static constexpr std::uint16_t kExplored = 0x0400U;
    static constexpr std::uint16_t kVisibleAndExplored =
        kCurrentVisible | kExplored;

    [[nodiscard]] static CellPosition party_cell(
        const ExplorationSessionState& session) noexcept;
    [[nodiscard]] static bool in_bounds(
        const LevelState& level, CellPosition cell) noexcept;
    [[nodiscard]] static std::size_t cell_index(
        const LevelState& level, CellPosition cell);
    [[nodiscard]] static std::uint16_t cell_word(
        const LevelState& level, CellPosition cell);

    // CODE.2 334EC clears only bit 0x0040 across the map.
    static void clear_current_visibility(LevelState& level);

    // The first-person traversal ORs 0x0440 into every reached cell.
    static void mark_visible_and_explored(LevelState& level, CellPosition cell);

    // Rebuilds the CODE.2 01558 automap/display cache. This intentionally
    // retains the encoded 0x0093 shortcut for unexplored cells even though
    // the supplied LEV022.DAT cache has the documented 40-cell discrepancy.
    static void rebuild_automap_cache(
        LevelState& level,
        std::optional<CellPosition> party_cell = std::nullopt);

    [[nodiscard]] static std::uint8_t classify_automap_cell(
        const LevelState& level,
        CellPosition cell,
        std::optional<CellPosition> party_cell = std::nullopt);

private:
    [[nodiscard]] static const CellControlRecord* find_record(
        const std::vector<CellControlRecord>& records,
        CellPosition cell) noexcept;
};

}  // namespace rloft
