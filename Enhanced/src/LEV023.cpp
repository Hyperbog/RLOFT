#include "rloft/LEV023.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kHeaderAt = 0x1DU;
constexpr std::array<std::size_t, 5> kBankStarts{{
    0x00AU, 0x08AU, 0x10AU, 0x18AU, 0x20AU}};
constexpr std::size_t kResourceCount = 64U;

std::size_t offset(Lev023ResourceBank bank, std::uint8_t code) {
    const auto id = static_cast<std::size_t>(bank);
    if (id >= kBankStarts.size() || code >= kResourceCount)
        throw std::out_of_range("LEV023 invalid resource bank or cell code");
    return kHeaderAt + kBankStarts[id] + 2U * code;
}
std::uint16_t read16(const std::vector<std::uint8_t>& bytes,
                     std::size_t at) {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(bytes.at(at)) |
        (static_cast<unsigned>(bytes.at(at + 1U)) << 8U));
}
void write16(std::vector<std::uint8_t>& bytes, std::size_t at,
             std::uint16_t value) {
    bytes.at(at) = static_cast<std::uint8_t>(value);
    bytes.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
} // namespace

Lev023File::Lev023File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}
const LevDirectSummary& Lev023File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev023File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev023File::encode() const {
    return direct_.encode();
}
LevelState Lev023File::decode() const { return direct_.decode(); }
std::uint16_t Lev023File::resource_word(
    Lev023ResourceBank bank, std::uint8_t code) const {
    return read16(bytes(), offset(bank, code));
}
Lev012NpcRecord Lev023File::npc(std::size_t index) const {
    return Lev012File(encode()).npc(index);
}
Lev018PlacedRecord Lev023File::placed_object(std::size_t index) const {
    return Lev018File(encode()).placed_object(index);
}
Lev021LooseRecord Lev023File::loose_item(std::size_t index) const {
    return Lev021File(encode()).loose_item(index);
}
std::vector<CellControlRecord> Lev023File::controls(
    Lev019ControlGroup group, int column) const {
    return Lev019File(encode()).controls(group, column);
}
std::uint16_t Lev023File::cell_word(int column, int row) const {
    return direct_.cell_word(column, row);
}
Lev005EventSlot Lev023File::event_slot(std::size_t index) const {
    return Lev005File(encode()).event_slot(index);
}

void Lev023File::apply(const Lev023Edits& e) {
    if (e.resources.empty() && e.controls.empty() && e.npcs.empty() &&
        e.placed.empty() && e.loose.empty()) return;

    auto pending = encode();
    // All existing state editors are source-checked, reversible, and use
    // the canonical direct-save parser. We stage, but never commit here.
    if (!e.controls.empty()) {
        Lev019File stage(std::move(pending));
        stage.apply_control_patches(e.controls);
        pending = stage.encode();
    }
    if (!e.npcs.empty()) {
        Lev012File stage(std::move(pending));
        stage.apply_npc_patches(e.npcs);
        pending = stage.encode();
    }
    if (!e.placed.empty()) {
        Lev018File stage(std::move(pending));
        stage.apply_placed_patches(e.placed);
        pending = stage.encode();
    }
    if (!e.loose.empty()) {
        Lev021File stage(std::move(pending));
        stage.apply_loose_patches(e.loose);
        pending = stage.encode();
    }

    if (!e.resources.empty()) {
        std::array<bool, 5U * kResourceCount> seen{};
        // Check all expected words before any header edit. A resource-bank
        // index is a 6-bit cell code, never an index into a RES archive.
        for (const auto& patch : e.resources) {
            const auto at = offset(patch.bank, patch.cell_code);
            const auto key = static_cast<std::size_t>(patch.bank) *
                             kResourceCount + patch.cell_code;
            if (seen[key])
                throw std::invalid_argument("LEV023 duplicate resource patch");
            seen[key] = true;
            if (read16(pending, at) != patch.expected)
                throw std::runtime_error("LEV023 stale resource lookup word");
        }
        for (const auto& patch : e.resources)
            write16(pending, offset(patch.bank, patch.cell_code),
                    patch.replacement);
    }
    // One final validated commit; raw preamble, cache, inactive script
    // commands and unidentified resource words retain their original bytes.
    Lev001File validated(std::move(pending));
    direct_ = std::move(validated);
}
void Lev023File::set_music_sequence(std::uint8_t sequence_id) {
    direct_.set_music_sequence(sequence_id);
}

} // namespace rloft
