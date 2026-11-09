#include "rloft/LEV005.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kSlots = 50U;
constexpr std::size_t kCommandBytes = 5U;
constexpr std::uint8_t kMaxOriginalOpcode = 12U;

void validate_slot(const Lev005EventSlot& slot) {
    if (slot.active_count() > Lev005EventSlot::kCommands)
        throw std::invalid_argument("LEV005 event count exceeds five commands");
    for (std::size_t i = 0; i < slot.active_count(); ++i) {
        if (slot.raw[1U + i * kCommandBytes] > kMaxOriginalOpcode)
            throw std::invalid_argument("LEV005 active event opcode exceeds CODE.2 range");
    }
}
} // namespace

std::array<std::uint8_t, 5> Lev005EventSlot::command(std::size_t ordinal) const {
    if (ordinal >= kCommands)
        throw std::out_of_range("LEV005 event command index");
    std::array<std::uint8_t, 5> result{};
    std::copy_n(raw.begin() + static_cast<std::ptrdiff_t>(1U + ordinal * 5U),
                result.size(), result.begin());
    return result;
}

Lev005File::Lev005File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}

const LevDirectSummary& Lev005File::summary() const noexcept {
    return direct_.summary();
}

const std::vector<std::uint8_t>& Lev005File::bytes() const noexcept {
    return direct_.bytes();
}

std::vector<std::uint8_t> Lev005File::encode() const {
    return direct_.encode();
}

LevelState Lev005File::decode() const {
    return direct_.decode();
}

std::uint16_t Lev005File::cell_word(int column, int row) const {
    return direct_.cell_word(column, row);
}

std::size_t Lev005File::event_offset(std::size_t index) const {
    if (index >= kSlots)
        throw std::out_of_range("LEV005 event slot index");
    return static_cast<std::size_t>(summary().boundaries[5]) +
           index * Lev005EventSlot::kBytes;
}

Lev005EventSlot Lev005File::event_slot(std::size_t index) const {
    const auto offset = event_offset(index);
    Lev005EventSlot slot{};
    std::copy_n(bytes().begin() + static_cast<std::ptrdiff_t>(offset),
                slot.raw.size(), slot.raw.begin());
    return slot;
}

void Lev005File::replace_event_slot(std::size_t index,
                                    const Lev005EventSlot& slot) {
    const auto offset = event_offset(index); // Reject index first.
    validate_slot(slot);
    auto updated = encode();
    std::copy(slot.raw.begin(), slot.raw.end(),
              updated.begin() + static_cast<std::ptrdiff_t>(offset));
    Lev001File checked(std::move(updated)); // Existing authoritative decoder.
    direct_ = std::move(checked); // Commit only after full validation.
}

void Lev005File::set_cell_word(int column, int row, std::uint16_t word) {
    direct_.set_cell_word(column, row, word);
}
void Lev005File::set_music_sequence(std::uint8_t res5_id) {
    direct_.set_music_sequence(res5_id);
}
void Lev005File::replace_world_family(
    LevWorldFamily family, const std::vector<std::uint8_t>& records22) {
    direct_.replace_world_family(family, records22);
}
void Lev005File::replace_automap_annotations(
    const std::vector<std::uint8_t>& blob) {
    direct_.replace_automap_annotations(blob);
}

} // namespace rloft
