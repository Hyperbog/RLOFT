#include "rloft/LEV019.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
using Column = std::vector<CellControlRecord>;
using Group = std::vector<Column>;

std::size_t group_index(Lev019ControlGroup group) {
    switch (group) {
        case Lev019ControlGroup::Primary: return 0U;
        case Lev019ControlGroup::Secondary: return 1U;
    }
    throw std::invalid_argument("LEV019 invalid control-group selector");
}
void append16(std::vector<std::uint8_t>& out, std::size_t count) {
    if (count > 32767U)
        throw std::invalid_argument("LEV019 signed control count overflow");
    out.push_back(static_cast<std::uint8_t>(count));
    out.push_back(static_cast<std::uint8_t>(count >> 8U));
}
void write32(std::vector<std::uint8_t>& out, std::size_t at,
             std::uint32_t value) {
    for (unsigned byte = 0U; byte < 4U; ++byte)
        out.at(at + byte) = static_cast<std::uint8_t>(value >> (byte * 8U));
}
using Groups = std::array<Group, 2>;
Groups split_groups(const LevelState& source) {
    Groups groups{{Group(static_cast<std::size_t>(source.width)),
                   Group(static_cast<std::size_t>(source.width))}};
    for (std::size_t id = 0; id < 2U; ++id) {
        const auto& records = id == 0U ? source.group1 : source.group2;
        for (const auto& record : records) {
            const auto col = static_cast<std::size_t>(record.raw[5]);
            if (col >= groups[id].size() || record.raw[4] >= source.height)
                throw std::runtime_error("LEV019 original column record invalid");
            groups[id][col].push_back(record);
        }
    }
    return groups;
}
std::vector<std::uint8_t> serialize_groups(const Groups& groups,
                                            std::uint8_t animation,
                                            int height) {
    std::vector<std::uint8_t> out;
    for (std::size_t id = 0; id < 2U; ++id) {
        if (id == 1U) out.push_back(animation);
        for (std::size_t col = 0; col < groups[id].size(); ++col) {
            const auto& records = groups[id][col];
            append16(out, records.size());
            for (const auto& record : records) {
                if (record.raw[5] != col || record.raw[4] >= height)
                    throw std::invalid_argument(
                        "LEV019 replacement control coordinates invalid");
                out.insert(out.end(), record.raw.begin(), record.raw.end());
            }
        }
    }
    return out;
}
}  // namespace

Lev019File::Lev019File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}

const LevDirectSummary& Lev019File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev019File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev019File::encode() const {
    return direct_.encode();
}
LevelState Lev019File::decode() const { return direct_.decode(); }

std::vector<CellControlRecord> Lev019File::controls(
    Lev019ControlGroup group, int column) const {
    const auto id = group_index(group);
    if (column < 0 || column >= summary().width)
        throw std::out_of_range("LEV019 control column outside level");
    const auto groups = split_groups(decode());
    return groups[id][static_cast<std::size_t>(column)];
}

void Lev019File::apply_control_patches(
    const std::vector<Lev019ControlPatch>& patches) {
    if (patches.empty()) return;
    const auto source = decode();
    auto groups = split_groups(source);
    std::array<std::vector<bool>, 2> seen{
        std::vector<bool>(static_cast<std::size_t>(source.width), false),
        std::vector<bool>(static_cast<std::size_t>(source.width), false)};

    for (const auto& patch : patches) {
        const auto id = group_index(patch.group);
        if (patch.column < 0 || patch.column >= source.width)
            throw std::out_of_range("LEV019 patch column outside map");
        const auto col = static_cast<std::size_t>(patch.column);
        if (seen[id][col])
            throw std::invalid_argument("LEV019 duplicate column patch");
        seen[id][col] = true;
        const auto& old = groups[id][col];
        if (old.size() != patch.expected.size())
            throw std::runtime_error("LEV019 stale control column count");
        for (std::size_t j = 0; j < old.size(); ++j) {
            if (old[j].raw != patch.expected[j].raw)
                throw std::runtime_error("LEV019 stale control record");
        }
        groups[id][col] = patch.replacement;
    }

    const auto block = serialize_groups(groups, source.active_animation_count,
                                         source.height);
    const auto begin = static_cast<std::size_t>(source.boundaries[1]);
    const auto end = static_cast<std::size_t>(source.boundaries[2]);
    if (end < begin || end > bytes().size())
        throw std::logic_error("LEV019 invalid source control boundaries");
    const auto old_size = end - begin;
    if (bytes().size() - old_size + block.size() > 4U * 1024U * 1024U)
        throw std::invalid_argument("LEV019 control edit exceeds level limit");
    const auto delta = static_cast<std::int64_t>(block.size()) -
                       static_cast<std::int64_t>(old_size);
    auto updated = encode();
    updated.erase(updated.begin() + static_cast<std::ptrdiff_t>(begin),
                  updated.begin() + static_cast<std::ptrdiff_t>(end));
    updated.insert(updated.begin() + static_cast<std::ptrdiff_t>(begin),
                   block.begin(), block.end());
    for (std::size_t i = 2U; i < source.boundaries.size(); ++i) {
        const auto value = static_cast<std::int64_t>(source.boundaries[i]) + delta;
        if (value < 0 || value > std::numeric_limits<std::uint32_t>::max())
            throw std::overflow_error("LEV019 section offset overflow");
        write32(updated, 1U + 4U * i, static_cast<std::uint32_t>(value));
    }
    Lev001File validated(std::move(updated));
    direct_ = std::move(validated);  // No live mutation before validation.
}

std::uint16_t Lev019File::cell_word(int column, int row) const {
    return direct_.cell_word(column, row);
}
void Lev019File::set_cell_word(int column, int row, std::uint16_t value) {
    direct_.set_cell_word(column, row, value);
}
void Lev019File::set_music_sequence(std::uint8_t resource_id) {
    direct_.set_music_sequence(resource_id);
}
void Lev019File::replace_world_family(
    LevWorldFamily family, const std::vector<std::uint8_t>& records22) {
    direct_.replace_world_family(family, records22);
}
void Lev019File::replace_automap_annotations(
    const std::vector<std::uint8_t>& notes) {
    direct_.replace_automap_annotations(notes);
}
}  // namespace rloft
