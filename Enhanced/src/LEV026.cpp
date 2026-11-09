#include "rloft/LEV026.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kSerializedThresholdAt = 0x1DU + 0x29AU;

std::int16_t read_signed16(const std::vector<std::uint8_t>& bytes) {
    const auto word = static_cast<unsigned>(bytes.at(kSerializedThresholdAt)) |
                      (static_cast<unsigned>(bytes.at(kSerializedThresholdAt + 1U)) << 8U);
    const auto signed_word = word <= 0x7FFFU
        ? static_cast<std::int32_t>(word)
        : static_cast<std::int32_t>(word) - 65536;
    return static_cast<std::int16_t>(signed_word);
}
void write_signed16(std::vector<std::uint8_t>& bytes, std::int16_t value) {
    const auto word = static_cast<std::uint16_t>(value);
    bytes.at(kSerializedThresholdAt) = static_cast<std::uint8_t>(word);
    bytes.at(kSerializedThresholdAt + 1U) = static_cast<std::uint8_t>(word >> 8U);
}
bool has_level_edits(const Lev025Edits& edits) {
    const auto& world = edits.world;
    return !world.notes.empty() || !world.resources.empty() ||
           !world.controls.empty() || !world.npcs.empty() ||
           !world.placed.empty() || !world.loose.empty() ||
           !edits.terrain.empty() || !edits.cache.empty() ||
           !edits.events.empty();
}
} // namespace

Lev026File::Lev026File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}
const LevDirectSummary& Lev026File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev026File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev026File::encode() const { return direct_.encode(); }
LevelState Lev026File::decode() const { return direct_.decode(); }
std::int16_t Lev026File::serialized_render_floor() const {
    return read_signed16(bytes());
}
std::int16_t Lev026File::effective_render_floor() const {
    const auto original = serialized_render_floor();
    return original == 0 ? 1 : original; // CODE.2's verified load-time rule.
}
std::vector<CellControlRecord> Lev026File::controls(
    Lev019ControlGroup group, int column) const {
    return direct_.controls(group, column);
}
Lev012NpcRecord Lev026File::npc(std::size_t i) const { return direct_.npc(i); }
Lev018PlacedRecord Lev026File::placed_object(std::size_t i) const {
    return direct_.placed_object(i);
}
Lev021LooseRecord Lev026File::loose_item(std::size_t i) const {
    return direct_.loose_item(i);
}
std::uint16_t Lev026File::cell_word(int x, int y) const {
    return direct_.cell_word(x, y);
}
std::uint8_t Lev026File::automap_cell(int x, int y) const {
    return direct_.automap_cell(x, y);
}
Lev005EventSlot Lev026File::event_slot(std::size_t i) const {
    return direct_.event_slot(i);
}
std::uint16_t Lev026File::resource_word(
    Lev023ResourceBank bank, std::uint8_t code) const {
    return direct_.resource_word(bank, code);
}
std::vector<Lev011MapNote> Lev026File::notes() const { return direct_.notes(); }

void Lev026File::apply(const Lev026Edits& edits) {
    if (!edits.render_floor && !has_level_edits(edits.level)) return;
    if (edits.render_floor &&
        serialized_render_floor() != edits.render_floor->expected_serialized)
        throw std::runtime_error("LEV026 stale renderer floor threshold");

    Lev025File stage(encode());
    stage.apply(edits.level); // Existing source-checked world, scripts, map.
    auto pending = stage.encode();
    if (edits.render_floor) {
        if (read_signed16(pending) != edits.render_floor->expected_serialized)
            throw std::runtime_error("LEV026 renderer floor changed during staging");
        write_signed16(pending, edits.render_floor->replacement_serialized);
    }
    Lev001File validated(std::move(pending)); // Canonical direct-save check.
    direct_ = Lev025File(validated.encode()); // Commit only after all success.
}
void Lev026File::set_music_sequence(std::uint8_t resource_id) {
    direct_.set_music_sequence(resource_id);
}

} // namespace rloft
