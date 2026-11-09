#include "rloft/LEV025.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::uint16_t kTerrainMask = 0x003FU;
} // namespace

Lev025File::Lev025File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}

const LevDirectSummary& Lev025File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev025File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev025File::encode() const {
    return direct_.encode();
}
LevelState Lev025File::decode() const { return direct_.decode(); }
std::uint16_t Lev025File::cell_word(int x, int y) const {
    return direct_.cell_word(x, y);
}
std::uint8_t Lev025File::automap_cell(int x, int y) const {
    return Lev020File(encode()).automap_cell(x, y);
}
Lev005EventSlot Lev025File::event_slot(std::size_t slot) const {
    return Lev020File(encode()).event_slot(slot);
}
std::uint16_t Lev025File::resource_word(
    Lev023ResourceBank bank, std::uint8_t code) const {
    return direct_.resource_word(bank, code);
}
Lev012NpcRecord Lev025File::npc(std::size_t i) const {
    return direct_.npc(i);
}
Lev018PlacedRecord Lev025File::placed_object(std::size_t i) const {
    return direct_.placed_object(i);
}
Lev021LooseRecord Lev025File::loose_item(std::size_t i) const {
    return direct_.loose_item(i);
}
std::vector<CellControlRecord> Lev025File::controls(
    Lev019ControlGroup group, int column) const {
    return direct_.controls(group, column);
}
std::vector<Lev011MapNote> Lev025File::notes() const {
    return direct_.notes();
}

void Lev025File::apply(const Lev025Edits& edits) {
    const auto& w = edits.world;
    if (w.notes.empty() && w.resources.empty() && w.controls.empty() &&
        w.npcs.empty() && w.placed.empty() && w.loose.empty() &&
        edits.terrain.empty() && edits.cache.empty() &&
        edits.events.empty()) return;

    Lev024File staged(encode());
    staged.apply(w); // Checked resource, control, world-record and note edits.

    Lev020Edits map{};
    map.cache = edits.cache;
    map.events = edits.events;
    map.cells.reserve(edits.terrain.size());
    for (const auto& t : edits.terrain) {
        if (t.replacement_code > kTerrainMask)
            throw std::invalid_argument("LEV025 cell code exceeds 6 bits");
        const auto changed = static_cast<std::uint16_t>(
            (t.expected_word & static_cast<std::uint16_t>(~kTerrainMask)) |
            t.replacement_code);
        map.cells.push_back({t.column, t.row, t.expected_word, changed});
    }
    Lev020File candidate(staged.encode());
    candidate.apply(map); // Stale/duplicate terrain, cache and events checked.

    // Single commit only after both validators accept the full result.
    direct_ = Lev024File(candidate.encode());
}

void Lev025File::set_music_sequence(std::uint8_t resource_id) {
    direct_.set_music_sequence(resource_id);
}

} // namespace rloft
