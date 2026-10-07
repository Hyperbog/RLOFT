#pragma once

#include <cstdint>
#include <vector>

#include "rloft/exploration_state.hpp"
#include "rloft/exploration_world.hpp"

namespace rloft {

class AutomapPresentationModel final {
public:
    // CODE.2 009B0: resolve generated cache classes into connected-map
    // display values. Values >= 0x20 are already resolved.
    [[nodiscard]] static std::uint8_t resolve_cell(
        const LevelState& level,
        CellPosition cell);

    // CODE.2 026E8..02A33 base export layer. This intentionally excludes
    // family-B item overlays and annotation labels; those are separate layers.
    [[nodiscard]] static std::uint8_t cp437_base_glyph(
        const LevelState& level,
        CellPosition cell);

    [[nodiscard]] static std::vector<std::uint8_t> cp437_base_map(
        const LevelState& level);

private:
    [[nodiscard]] static std::uint8_t cache_at(
        const LevelState& level,
        CellPosition cell);
};

}  // namespace rloft
