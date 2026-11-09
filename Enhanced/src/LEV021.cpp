#include "rloft/LEV021.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
using Record = Lev021LooseRecord;

std::uint16_t read16(const Record& r, std::size_t p) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(r.raw[p]) |
        (static_cast<unsigned>(r.raw[p + 1U]) << 8U));
}
std::uint32_t read32(const Record& r, std::size_t p) noexcept {
    return static_cast<std::uint32_t>(r.raw[p]) |
        (static_cast<std::uint32_t>(r.raw[p + 1U]) << 8U) |
        (static_cast<std::uint32_t>(r.raw[p + 2U]) << 16U) |
        (static_cast<std::uint32_t>(r.raw[p + 3U]) << 24U);
}
void write16(Record& r, std::size_t p, std::uint16_t v) noexcept {
    r.raw[p] = static_cast<std::uint8_t>(v);
    r.raw[p + 1U] = static_cast<std::uint8_t>(v >> 8U);
}
void write32(Record& r, std::size_t p, std::uint32_t v) noexcept {
    for (unsigned i = 0U; i < 4U; ++i)
        r.raw[p + i] = static_cast<std::uint8_t>(v >> (i * 8U));
}
void check_position(const Record& r, int width, int height) {
    const auto x = r.x_position_16_16() >> 16U;
    const auto y = r.y_position_16_16() >> 16U;
    if (x >= static_cast<unsigned>(width) ||
        y >= static_cast<unsigned>(height) ||
        r.cached_cell_index() != y * static_cast<unsigned>(width) + x)
        throw std::invalid_argument("LEV021 loose-item position/cache mismatch");
}
} // namespace

std::uint32_t Record::y_position_16_16() const noexcept { return read32(*this, 0U); }
std::uint32_t Record::x_position_16_16() const noexcept { return read32(*this, 4U); }
std::uint32_t Record::runtime_flags() const noexcept { return read32(*this, 8U); }
std::uint16_t Record::cached_cell_index() const noexcept { return read16(*this, 12U); }
std::uint16_t Record::object_type_id() const noexcept { return read16(*this, 14U); }
std::uint16_t Record::runtime_word_10() const noexcept { return read16(*this, 16U); }
std::uint16_t Record::runtime_word_12() const noexcept { return read16(*this, 18U); }
std::uint16_t Record::packed_runtime_state() const noexcept { return read16(*this, 20U); }
void Record::set_position_16_16(std::uint32_t x, std::uint32_t y,
                                std::uint16_t cell) noexcept {
    write32(*this, 0U, y); write32(*this, 4U, x); write16(*this, 12U, cell);
}
void Record::set_object_type_id(std::uint16_t id) noexcept { write16(*this, 14U, id); }
void Record::set_runtime_flags(std::uint32_t flags) noexcept { write32(*this, 8U, flags); }
void Record::set_runtime_word_10(std::uint16_t v) noexcept { write16(*this, 16U, v); }
void Record::set_runtime_word_12(std::uint16_t v) noexcept { write16(*this, 18U, v); }
void Record::set_packed_runtime_state(std::uint16_t v) noexcept { write16(*this, 20U, v); }

Lev021File::Lev021File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}
const LevDirectSummary& Lev021File::summary() const noexcept { return direct_.summary(); }
const std::vector<std::uint8_t>& Lev021File::bytes() const noexcept { return direct_.bytes(); }
std::vector<std::uint8_t> Lev021File::encode() const { return direct_.encode(); }
LevelState Lev021File::decode() const { return direct_.decode(); }

Lev021LooseRecord Lev021File::loose_item(std::size_t index) const {
    if (index >= summary().world_counts[2])
        throw std::out_of_range("LEV021 loose-item index");
    const auto begin = static_cast<std::size_t>(summary().boundaries[4]);
    const auto at = begin + index * Record::kBytes;
    Record result{};
    std::copy_n(bytes().begin() + static_cast<std::ptrdiff_t>(at),
                Record::kBytes, result.raw.begin());
    return result;
}

void Lev021File::apply_loose_patches(
    const std::vector<Lev021LoosePatch>& patches) {
    if (patches.empty()) return;
    const auto count = summary().world_counts[2];
    // CODE.2's live loose-item array has a 300-instance capacity.
    constexpr std::size_t kLiveLimit = 300U;
    std::vector<const Lev021LoosePatch*> by_index(count + 1U, nullptr);
    const auto state = decode();
    const auto& source = state.family_c_raw;
    if (source.size() != count * Record::kBytes)
        throw std::logic_error("LEV021 family-C byte count is inconsistent");

    std::size_t new_count = count;
    for (const auto& patch : patches) {
        const bool insert = !patch.expected && patch.replacement;
        const bool erase = patch.expected && !patch.replacement;
        const bool replace = patch.expected && patch.replacement;
        if (!insert && !erase && !replace)
            throw std::invalid_argument("LEV021 empty loose-item patch");
        if (patch.index > count || (!insert && patch.index == count))
            throw std::out_of_range("LEV021 loose-item patch index");
        if (by_index[patch.index])
            throw std::invalid_argument("LEV021 duplicate loose-item patch index");
        by_index[patch.index] = &patch;
        if (patch.expected) {
            const auto at = patch.index * Record::kBytes;
            if (!std::equal(patch.expected->raw.begin(), patch.expected->raw.end(),
                            source.begin() + static_cast<std::ptrdiff_t>(at)))
                throw std::runtime_error("LEV021 stale original loose-item bytes");
        }
        if (patch.replacement) {
            // Changing only flags on an existing actor must never silently
            // normalize its raw, possibly inactive, original coordinates.
            if (insert ||
                patch.replacement->x_position_16_16() != patch.expected->x_position_16_16() ||
                patch.replacement->y_position_16_16() != patch.expected->y_position_16_16() ||
                patch.replacement->cached_cell_index() != patch.expected->cached_cell_index())
                check_position(*patch.replacement, state.width, state.height);
        }
        if (insert) ++new_count;
        if (erase) --new_count;
    }
    if (new_count > kLiveLimit)
        throw std::invalid_argument("LEV021 loose-item count exceeds CODE.2 capacity");

    std::vector<std::uint8_t> revised;
    revised.reserve(new_count * Record::kBytes);
    for (std::size_t i = 0U; i <= count; ++i) {
        const auto* patch = by_index[i];
        if (patch) {
            if (patch->replacement)
                revised.insert(revised.end(), patch->replacement->raw.begin(),
                               patch->replacement->raw.end());
            // A replacement or deletion consumes the original at index i.
            // An insertion does not: it precedes the original record.
            if (patch->expected || i == count) continue;
        }
        if (i < count) {
            const auto at = source.begin() + static_cast<std::ptrdiff_t>(i * Record::kBytes);
            revised.insert(revised.end(), at, at + Record::kBytes);
        }
    }
    // Lev001File owns header count and post-family section offset repairs.
    // Never modify live state until its canonical parser accepts the result.
    Lev001File validated(encode());
    validated.replace_world_family(LevWorldFamily::Loose, revised);
    direct_ = std::move(validated);
}

std::uint16_t Lev021File::cell_word(int x, int y) const {return direct_.cell_word(x, y);}
void Lev021File::set_cell_word(int x, int y, std::uint16_t value) {
    direct_.set_cell_word(x, y, value);
}
void Lev021File::set_music_sequence(std::uint8_t id) {
    direct_.set_music_sequence(id);
}
void Lev021File::replace_automap_annotations(const std::vector<std::uint8_t>& notes) {
    direct_.replace_automap_annotations(notes);
}

} // namespace rloft
