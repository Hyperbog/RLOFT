#include "rloft/LEV024.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

bool same_note(const Lev011MapNote& lhs, const Lev011MapNote& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.label == rhs.label;
}

std::vector<Lev011MapNote> apply_note_edits(
    const std::vector<Lev011MapNote>& original,
    const std::vector<Lev024NotePatch>& patches) {
    const auto count = original.size();
    std::vector<const Lev024NotePatch*> edits(count + 1U, nullptr);
    for (const auto& patch : patches) {
        const bool inserting = !patch.expected && patch.replacement;
        const bool erasing = patch.expected && !patch.replacement;
        const bool replacing = patch.expected && patch.replacement;
        if (!inserting && !erasing && !replacing)
            throw std::invalid_argument("LEV024 empty map annotation patch");
        if (patch.index > count || (patch.index == count && !inserting))
            throw std::out_of_range("LEV024 map annotation index");
        if (edits[patch.index])
            throw std::invalid_argument("LEV024 duplicate map annotation index");
        if (patch.expected && !same_note(original[patch.index], *patch.expected))
            throw std::runtime_error("LEV024 stale map annotation");
        edits[patch.index] = &patch;
    }

    std::vector<Lev011MapNote> updated;
    updated.reserve(count + patches.size());
    for (std::size_t i = 0U; i <= count; ++i) {
        const auto* patch = edits[i];
        if (patch) {
            if (patch->replacement) updated.push_back(*patch->replacement);
            // Replacement/erasure consumes original; insertion doesn't.
            if (patch->expected || i == count) continue;
        }
        if (i < count) updated.push_back(original[i]);
    }
    return updated;
}

} // namespace

Lev024File::Lev024File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {
    (void)Lev011File(direct_.encode()).notes(); // Validate note grammar.
}

const LevDirectSummary& Lev024File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev024File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev024File::encode() const {
    return direct_.encode();
}
LevelState Lev024File::decode() const { return direct_.decode(); }
std::vector<Lev011MapNote> Lev024File::notes() const {
    return Lev011File(encode()).notes();
}
std::uint16_t Lev024File::resource_word(
    Lev023ResourceBank bank, std::uint8_t code) const {
    return direct_.resource_word(bank, code);
}
Lev012NpcRecord Lev024File::npc(std::size_t index) const {
    return direct_.npc(index);
}
Lev018PlacedRecord Lev024File::placed_object(std::size_t index) const {
    return direct_.placed_object(index);
}
Lev021LooseRecord Lev024File::loose_item(std::size_t index) const {
    return direct_.loose_item(index);
}
std::vector<CellControlRecord> Lev024File::controls(
    Lev019ControlGroup group, int column) const {
    return direct_.controls(group, column);
}
std::uint16_t Lev024File::cell_word(int x, int y) const {
    return direct_.cell_word(x, y);
}

void Lev024File::apply(const Lev024Edits& edits) {
    if (edits.notes.empty() && edits.resources.empty() &&
        edits.controls.empty() && edits.npcs.empty() &&
        edits.placed.empty() && edits.loose.empty()) return;

    // Stage existing original-format world and header changes without
    // modifying the live object. LEV023 validates each expected record.
    Lev023File candidate(encode());
    Lev023Edits world{};
    world.resources = edits.resources;
    world.controls = edits.controls;
    world.npcs = edits.npcs;
    world.placed = edits.placed;
    world.loose = edits.loose;
    candidate.apply(world);

    if (!edits.notes.empty()) {
        Lev011File annotations(candidate.encode());
        const auto previous = annotations.notes();
        const auto updated = apply_note_edits(previous, edits.notes);
        // LEV011 validates signed coordinates, 16-bit label lengths and
        // CODE.2's 2046-byte note buffer; it owns size bookkeeping.
        annotations.replace_notes(updated);
        candidate = Lev023File(annotations.encode());
    }
    direct_ = std::move(candidate); // Commit only after every check.
}

void Lev024File::set_music_sequence(std::uint8_t resource_id) {
    direct_.set_music_sequence(resource_id);
}

} // namespace rloft
