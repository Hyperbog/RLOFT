#include "rloft/LEV011.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kNoteHeader = 6U;
constexpr std::size_t kMaximumNotesBytes = 2046U; // CODE.2 0x7FF buffer rule.

std::uint16_t read16(const std::vector<std::uint8_t>& b,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[offset]) |
        (static_cast<unsigned>(b[offset + 1U]) << 8U));
}
std::int16_t signed16(std::uint16_t raw) noexcept {
    return static_cast<std::int16_t>(raw <= 0x7FFFU
        ? static_cast<std::int32_t>(raw)
        : static_cast<std::int32_t>(raw) - 65536);
}
void append16(std::vector<std::uint8_t>& out, std::uint16_t value) {
    out.push_back(static_cast<std::uint8_t>(value));
    out.push_back(static_cast<std::uint8_t>(value >> 8U));
}
std::vector<Lev011MapNote> parse_notes(const std::vector<std::uint8_t>& raw) {
    if (raw.size() > kMaximumNotesBytes)
        throw std::runtime_error("LEV011 automap notes exceed CODE.2 capacity");
    std::vector<Lev011MapNote> result;
    for (std::size_t pos = 0U; pos < raw.size();) {
        if (raw.size() - pos < kNoteHeader)
            throw std::runtime_error("LEV011 truncated annotation header");
        Lev011MapNote note{};
        note.x = signed16(read16(raw, pos));
        note.y = signed16(read16(raw, pos + 2U));
        const auto count = signed16(read16(raw, pos + 4U));
        pos += kNoteHeader;
        if (count <= 0 || static_cast<std::size_t>(count) > raw.size() - pos)
            throw std::runtime_error("LEV011 invalid annotation label length");
        note.label.assign(raw.begin() + static_cast<std::ptrdiff_t>(pos),
                          raw.begin() + static_cast<std::ptrdiff_t>(
                              pos + static_cast<std::size_t>(count)));
        pos += static_cast<std::size_t>(count);
        result.push_back(std::move(note));
    }
    return result;
}
std::vector<std::uint8_t> serialize_notes(
    const std::vector<Lev011MapNote>& notes) {
    std::vector<std::uint8_t> bytes;
    for (const auto& note : notes) {
        if (note.label.empty() || note.label.size() > 0x7FFFU ||
            bytes.size() > kMaximumNotesBytes ||
            note.label.size() > kMaximumNotesBytes - bytes.size() ||
            kNoteHeader > kMaximumNotesBytes - bytes.size() - note.label.size())
            throw std::invalid_argument("LEV011 note has invalid size");
        append16(bytes, static_cast<std::uint16_t>(note.x));
        append16(bytes, static_cast<std::uint16_t>(note.y));
        append16(bytes, static_cast<std::uint16_t>(note.label.size()));
        bytes.insert(bytes.end(), note.label.begin(), note.label.end());
    }
    return bytes;
}
} // namespace

Lev011File::Lev011File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {
    (void)notes(); // Validate the additional original annotation grammar.
}
const LevDirectSummary& Lev011File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev011File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev011File::encode() const {
    return direct_.encode();
}
LevelState Lev011File::decode() const { return direct_.decode(); }
std::vector<Lev011MapNote> Lev011File::notes() const {
    return parse_notes(direct_.decode().automap_annotations);
}
void Lev011File::replace_notes(const std::vector<Lev011MapNote>& changed) {
    auto raw = serialize_notes(changed);
    // Ensure the resulting note grammar is canonical before updating.
    if (parse_notes(raw).size() != changed.size())
        throw std::logic_error("LEV011 note serialization count mismatch");
    direct_.replace_automap_annotations(raw); // Validated transactional commit.
}
void Lev011File::set_note(std::size_t index, const Lev011MapNote& note) {
    auto changed = notes();
    changed.at(index) = note;
    replace_notes(changed);
}
void Lev011File::insert_note(std::size_t index, const Lev011MapNote& note) {
    auto changed = notes();
    if (index > changed.size()) throw std::out_of_range("LEV011 insert index");
    changed.insert(changed.begin() + static_cast<std::ptrdiff_t>(index), note);
    replace_notes(changed);
}
void Lev011File::erase_note(std::size_t index) {
    auto changed = notes();
    if (index >= changed.size()) throw std::out_of_range("LEV011 erase index");
    changed.erase(changed.begin() + static_cast<std::ptrdiff_t>(index));
    replace_notes(changed);
}
std::uint16_t Lev011File::cell_word(int column, int row) const {
    return direct_.cell_word(column, row);
}
std::uint8_t Lev011File::automap_cell(int column, int row) const {
    return direct_.automap_cell(column, row);
}
Lev005EventSlot Lev011File::event_slot(std::size_t index) const {
    return direct_.event_slot(index);
}
void Lev011File::apply_cell_patches(
    const std::vector<Lev010CellPatch>& patches) {
    direct_.apply_cell_patches(patches);
}
void Lev011File::replace_automap_cache(
    const std::vector<std::uint8_t>& cache) {
    direct_.replace_automap_cache(cache);
}
void Lev011File::replace_event_slot(
    std::size_t index, const Lev005EventSlot& slot) {
    direct_.replace_event_slot(index, slot);
}
void Lev011File::set_music_sequence(std::uint8_t res5_id) {
    direct_.set_music_sequence(res5_id);
}
void Lev011File::replace_world_family(
    LevWorldFamily family, const std::vector<std::uint8_t>& records22) {
    direct_.replace_world_family(family, records22);
}
} // namespace rloft
