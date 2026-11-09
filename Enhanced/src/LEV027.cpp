#include "rloft/LEV027.hpp"

#include "rloft/LEV019.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

void check_transfer(const Lev027ControlTransfer& edit) {
    if (edit.source == edit.destination)
        throw std::invalid_argument("LEV027 transfer must change control group");
    if (edit.column < 0 || edit.column > 127 ||
        edit.expected.column() != edit.column ||
        edit.replacement.column() != edit.column ||
        edit.expected.row() != edit.replacement.row() ||
        edit.expected.raw[0] != edit.replacement.raw[0])
        throw std::invalid_argument("LEV027 transfer changes control identity or column");
}

} // namespace

Lev027File::Lev027File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}
const LevDirectSummary& Lev027File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev027File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev027File::encode() const {
    return direct_.encode();
}
LevelState Lev027File::decode() const { return direct_.decode(); }
std::vector<CellControlRecord> Lev027File::controls(
    Lev019ControlGroup group, int column) const {
    return direct_.controls(group, column);
}
std::uint16_t Lev027File::resource_word(
    Lev023ResourceBank bank, std::uint8_t code) const {
    return direct_.resource_word(bank, code);
}
Lev012NpcRecord Lev027File::npc(std::size_t index) const {
    return direct_.npc(index);
}
Lev018PlacedRecord Lev027File::placed_object(std::size_t index) const {
    return direct_.placed_object(index);
}
Lev021LooseRecord Lev027File::loose_item(std::size_t index) const {
    return direct_.loose_item(index);
}
Lev005EventSlot Lev027File::event_slot(std::size_t slot) const {
    return direct_.event_slot(slot);
}
std::uint16_t Lev027File::cell_word(int x, int y) const {
    return direct_.cell_word(x, y);
}
std::uint8_t Lev027File::automap_cell(int x, int y) const {
    return direct_.automap_cell(x, y);
}
std::int16_t Lev027File::serialized_render_floor() const {
    return direct_.serialized_render_floor();
}

void Lev027File::apply(const Lev027Edits& edits) {
    // Never update the live level during staging. The canonical parser and
    // Lev026File validate all other world/header edits before the transfer.
    Lev026File candidate(encode());
    candidate.apply(edits.state);
    if (edits.transfer) {
        const auto& t = *edits.transfer;
        check_transfer(t);
        Lev019File staged(candidate.encode());
        const auto old_source = staged.controls(t.source, t.column);
        const auto old_destination = staged.controls(t.destination, t.column);
        if (t.source_index >= old_source.size() ||
            t.destination_index > old_destination.size())
            throw std::out_of_range("LEV027 control transfer index");
        if (old_source[t.source_index].raw != t.expected.raw)
            throw std::runtime_error("LEV027 stale control transfer source");
        auto new_source = old_source;
        auto new_destination = old_destination;
        new_source.erase(new_source.begin() +
                         static_cast<std::ptrdiff_t>(t.source_index));
        new_destination.insert(new_destination.begin() +
                               static_cast<std::ptrdiff_t>(t.destination_index),
                               t.replacement);
        staged.apply_control_patches({
            {t.source, t.column, old_source, new_source},
            {t.destination, t.column, old_destination, new_destination},
        });
        candidate = Lev026File(staged.encode());
    }
    direct_ = std::move(candidate);
}

} // namespace rloft
