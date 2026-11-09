#include "rloft/LEV022.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
constexpr std::uint16_t kVisibilityMask = 0x0440U;
} // namespace

Lev022File::Lev022File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}

const LevDirectSummary& Lev022File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev022File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev022File::encode() const {
    return direct_.encode();
}
LevelState Lev022File::decode() const {
    return direct_.decode();
}
Lev012NpcRecord Lev022File::npc(std::size_t index) const {
    return direct_.npc(index);
}
std::uint16_t Lev022File::cell_word(int x, int y) const {
    return direct_.cell_word(x, y);
}
std::uint8_t Lev022File::automap_cell(int x, int y) const {
    return direct_.automap_cell(x, y);
}

Lev022SnapshotSummary Lev022File::snapshot() const {
    const auto state = decode();
    Lev022SnapshotSummary out{};
    out.cells = state.cell_words.size();
    out.npc_instances = summary().world_counts[0];
    out.placed_instances = summary().world_counts[1];
    for (const auto word : state.cell_words) {
        out.explored += (word & 0x0400U) != 0U ? 1U : 0U;
        out.currently_visible += (word & 0x0040U) != 0U ? 1U : 0U;
    }
    out.cache_unmapped = static_cast<std::size_t>(std::count(
        state.automap_cache.begin(), state.automap_cache.end(), 0xFFU));
    out.cache_floor_class = static_cast<std::size_t>(std::count(
        state.automap_cache.begin(), state.automap_cache.end(), 0x2EU));
    return out;
}

void Lev022File::apply_checkpoint(const Lev022CheckpointEdits& edits) {
    if (edits.exploration.empty() && edits.npcs.empty()) return;

    Lev020Edits combined{};
    combined.npcs = edits.npcs;
    combined.cells.reserve(edits.exploration.size());
    combined.cache.reserve(edits.exploration.size());
    for (const auto& patch : edits.exploration) {
        if (((patch.expected_word ^ patch.replacement_word) &
              static_cast<std::uint16_t>(~kVisibilityMask)) != 0U)
            throw std::invalid_argument(
                "LEV022 exploration edit changes topology or renderer flags");
        combined.cells.push_back({patch.column, patch.row,
                                  patch.expected_word, patch.replacement_word});
        combined.cache.push_back({patch.column, patch.row,
                                  patch.expected_cache, patch.replacement_cache});
    }
    // Lev020File checks all expected values, duplicate coordinates and NPC
    // validity. Build a candidate so this class also owns one atomic commit.
    Lev020File verified(encode());
    verified.apply(combined);
    direct_ = std::move(verified);
}

} // namespace rloft
