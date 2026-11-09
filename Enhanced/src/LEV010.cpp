#include "rloft/LEV010.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kGridAt = 0x1DU + 0x2A4U;

void write16(std::vector<std::uint8_t>& bytes, std::size_t at,
             std::uint16_t value) {
    bytes.at(at) = static_cast<std::uint8_t>(value);
    bytes.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
} // namespace

Lev010File::Lev010File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}

const LevDirectSummary& Lev010File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev010File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev010File::encode() const {
    return direct_.encode();
}
LevelState Lev010File::decode() const {
    return direct_.decode();
}
std::uint16_t Lev010File::cell_word(int column, int row) const {
    return direct_.cell_word(column, row);
}
Lev005EventSlot Lev010File::event_slot(std::size_t index) const {
    return direct_.event_slot(index);
}

std::size_t Lev010File::cell_index(int column, int row) const {
    if (column < 0 || row < 0 || column >= summary().width ||
        row >= summary().height)
        throw std::out_of_range("LEV010 map coordinate outside original grid");
    return static_cast<std::size_t>(row) *
        static_cast<std::size_t>(summary().width) +
        static_cast<std::size_t>(column);
}

std::uint8_t Lev010File::automap_cell(int column, int row) const {
    const auto index = cell_index(column, row);
    const auto at = static_cast<std::size_t>(summary().boundaries[6]) + 4U + index;
    return bytes().at(at);
}

void Lev010File::apply_cell_patches(
    const std::vector<Lev010CellPatch>& patches) {
    if (patches.empty()) return;
    const auto count = static_cast<std::size_t>(summary().width) *
                       static_cast<std::size_t>(summary().height);
    std::vector<bool> seen(count, false);
    // Validate all source values before writing to a candidate buffer.
    for (const auto& patch : patches) {
        const auto index = cell_index(patch.column, patch.row);
        if (seen[index])
            throw std::invalid_argument("LEV010 duplicate cell patch");
        seen[index] = true;
        if (direct_.cell_word(patch.column, patch.row) != patch.expected)
            throw std::runtime_error("LEV010 stale cell patch");
    }
    auto updated = encode();
    for (const auto& patch : patches) {
        const auto at = kGridAt + 2U * cell_index(patch.column, patch.row);
        write16(updated, at, patch.replacement);
    }
    Lev005File checked(std::move(updated)); // Reuse original grammar.
    direct_ = std::move(checked); // Commit only on success.
}

void Lev010File::replace_automap_cache(
    const std::vector<std::uint8_t>& cache) {
    const auto cells = static_cast<std::size_t>(summary().width) *
                       static_cast<std::size_t>(summary().height);
    if (cache.size() != cells)
        throw std::invalid_argument("LEV010 automap cache size differs from map");
    auto updated = encode();
    const auto begin = static_cast<std::size_t>(summary().boundaries[6]) + 4U;
    std::copy(cache.begin(), cache.end(),
              updated.begin() + static_cast<std::ptrdiff_t>(begin));
    Lev005File checked(std::move(updated));
    direct_ = std::move(checked);
}

void Lev010File::replace_event_slot(std::size_t index,
                                    const Lev005EventSlot& slot) {
    direct_.replace_event_slot(index, slot);
}
void Lev010File::set_music_sequence(std::uint8_t res5_id) {
    direct_.set_music_sequence(res5_id);
}
void Lev010File::replace_world_family(
    LevWorldFamily family, const std::vector<std::uint8_t>& records22) {
    direct_.replace_world_family(family, records22);
}
void Lev010File::replace_automap_annotations(
    const std::vector<std::uint8_t>& blob) {
    direct_.replace_automap_annotations(blob);
}

} // namespace rloft
