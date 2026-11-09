#include "rloft/LEV030.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

std::uint16_t checked_cell(std::uint32_t x, std::uint32_t y,
                           int width, int height) {
    const auto column = x >> 16U;
    const auto row = y >> 16U;
    if (width <= 0 || height <= 0 ||
        column >= static_cast<unsigned>(width) ||
        row >= static_cast<unsigned>(height))
        throw std::out_of_range("LEV030 fixed-point position outside map");
    const auto cell = row * static_cast<unsigned>(width) + column;
    if (cell > std::numeric_limits<std::uint16_t>::max())
        throw std::out_of_range("LEV030 cached cell exceeds 16 bits");
    return static_cast<std::uint16_t>(cell);
}

} // namespace

Lev030File::Lev030File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}
const LevDirectSummary& Lev030File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev030File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev030File::encode() const { return direct_.encode(); }
LevelState Lev030File::decode() const { return direct_.decode(); }
Lev012NpcRecord Lev030File::npc(std::size_t i) const { return direct_.npc(i); }
Lev021LooseRecord Lev030File::loose_item(std::size_t i) const {
    return direct_.loose_item(i);
}
std::vector<CellControlRecord> Lev030File::controls(
    Lev019ControlGroup group, int column) const {
    return direct_.controls(group, column);
}
Lev005EventSlot Lev030File::event_slot(std::size_t slot) const {
    return direct_.event_slot(slot);
}
std::uint16_t Lev030File::cell_word(int x, int y) const {
    return direct_.cell_word(x, y);
}
std::uint8_t Lev030File::automap_cell(int x, int y) const {
    return direct_.automap_cell(x, y);
}
std::int16_t Lev030File::serialized_render_floor() const {
    return direct_.serialized_render_floor();
}

void Lev030File::apply(const Lev030Edits& edits) {
    Lev027Edits staged = edits.state;
    std::vector<bool> seen_npcs(summary().world_counts[0], false);
    std::vector<bool> seen_loose(summary().world_counts[2], false);

    for (const auto& patch : edits.npc_positions) {
        if (patch.index >= seen_npcs.size())
            throw std::out_of_range("LEV030 NPC index");
        if (seen_npcs[patch.index])
            throw std::invalid_argument("LEV030 duplicate NPC movement");
        seen_npcs[patch.index] = true;
        if (npc(patch.index).raw != patch.expected.raw)
            throw std::runtime_error("LEV030 stale NPC source record");
        auto replacement = patch.expected;
        replacement.set_position_16_16(
            patch.replacement_x_16_16, patch.replacement_y_16_16,
            checked_cell(patch.replacement_x_16_16,
                         patch.replacement_y_16_16,
                         summary().width, summary().height));
        staged.state.level.world.npcs.push_back(
            {patch.index, patch.expected, replacement});
    }
    for (const auto& patch : edits.loose_positions) {
        if (patch.index >= seen_loose.size())
            throw std::out_of_range("LEV030 loose-item index");
        if (seen_loose[patch.index])
            throw std::invalid_argument("LEV030 duplicate loose-item movement");
        seen_loose[patch.index] = true;
        if (loose_item(patch.index).raw != patch.expected.raw)
            throw std::runtime_error("LEV030 stale loose-item source record");
        auto replacement = patch.expected;
        replacement.set_position_16_16(
            patch.replacement_x_16_16, patch.replacement_y_16_16,
            checked_cell(patch.replacement_x_16_16,
                         patch.replacement_y_16_16,
                         summary().width, summary().height));
        staged.state.level.world.loose.push_back(
            {patch.index, patch.expected, replacement});
    }
    Lev027File candidate(encode());
    candidate.apply(staged); // Staged shared codecs validate all domains.
    direct_ = std::move(candidate);
}

} // namespace rloft
