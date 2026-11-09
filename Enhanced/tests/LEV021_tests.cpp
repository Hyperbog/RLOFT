#include "rloft/LEV021.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <vector>

namespace {
void need(bool ok, const char* issue) {
    if (!ok) throw std::runtime_error(issue);
}
template<class F> void rejects(F action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    need(rejected, "LEV021 invalid edit accepted");
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    b.at(at) = static_cast<std::uint8_t>(value);
    b.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, std::uint32_t value) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(value >> (8U * i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr unsigned width = 4U, height = 3U;
    constexpr std::size_t header = 0x1DU;
    constexpr std::size_t grid = header + 0x2A4U;
    constexpr std::size_t b1 = grid + 2U * width * height;
    constexpr std::size_t b2 = b1 + 2U * width + 1U + 2U * width;
    constexpr std::size_t b3 = b2, b4 = b3 + 2U;
    constexpr std::size_t b5 = b4 + 3U * 22U, b6 = b5 + 0x514U;
    std::vector<std::uint8_t> b(b6 + 4U + width * height + 4U, 0U);
    constexpr std::array<std::size_t, 7> bounds{{header, b1, b2, b3, b4, b5, b6}};
    b[0] = 0xC7U; // Original writer does not own this byte.
    for (std::size_t i = 0U; i < bounds.size(); ++i)
        put32(b, 1U + 4U * i, static_cast<std::uint32_t>(bounds[i]));
    put16(b, header + 1U, width);
    put16(b, header + 3U, height);
    put16(b, header + 0x28CU, 3U); // Family C.
    b[header + 0x29CU] = 40U;
    // The two empty width-indexed control groups and animation byte are zero.
    put16(b, b3, 0U); // Family B inline count.
    for (unsigned i = 0U; i < 3U; ++i) {
        const auto at = b4 + static_cast<std::size_t>(i) * 22U;
        const unsigned x = i + 1U, y = (i == 1U ? 2U : 1U);
        put32(b, at, y << 16U);
        put32(b, at + 4U, x << 16U);
        put16(b, at + 12U, y * width + x);
        put16(b, at + 14U, i == 0U ? 65U : (i == 1U ? 79U : 142U));
        put16(b, at + 20U, i == 0U ? 0x0040U : 0x0200U);
    }
    put16(b, b6, width);
    put16(b, b6 + 2U, height);
    put32(b, b6 + 4U + width * height, 0U);
    return b;
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev021File file(original);
    const auto before = file.summary();
    need(file.encode() == original, "LEV021 input not byte-exact on encode");
    if (retail) {
        need(original.size() == 10740U && before.width == 60 &&
             before.height == 30 && before.group1_controls == 15U &&
             before.group2_controls == 14U &&
             before.world_counts == std::array<std::size_t, 3>{{2U, 114U, 11U}} &&
             before.music_sequence == 40U && before.active_animation_count == 0U &&
             before.active_event_slots == 1U && before.active_event_commands == 1U &&
             before.annotation_bytes == 0U,
             "retail LEV021 format/identity mismatch");
        const std::array<std::uint16_t, 11> expected_ids{{
            168U, 93U, 65U, 65U, 65U, 301U, 142U, 179U, 169U, 234U, 43U}};
        for (std::size_t i = 0U; i < expected_ids.size(); ++i)
            need(file.loose_item(i).object_type_id() == expected_ids[i],
                 "retail loose-item order/type mismatch");
        const auto state = file.decode();
        const auto explored = std::count_if(
            state.cell_words.begin(), state.cell_words.end(),
            [](std::uint16_t word) { return (word & 0x0400U) != 0U; });
        need(explored == 532, "retail explored-cell evidence changed");
    } else {
        need(before.world_counts[2] == 3U && file.loose_item(1).object_type_id() == 79U,
             "synthetic loose-item fixture failed");
    }

    const auto old_count = before.world_counts[2];
    const auto remove = file.loose_item(1U);
    file.apply_loose_patches({{1U, remove, std::nullopt}});
    need(file.summary().world_counts[2] == old_count - 1U &&
         file.encode().size() == original.size() - 22U &&
         file.summary().boundaries[5] == before.boundaries[5] - 22U &&
         file.summary().boundaries[6] == before.boundaries[6] - 22U,
         "family-C erase did not update count and boundaries");
    file.apply_loose_patches({{1U, std::nullopt, remove}});
    need(file.encode() == original, "loose item erase+restore not byte-exact");

    const auto first = file.loose_item(0U);
    auto changed = first;
    changed.set_runtime_flags(first.runtime_flags() ^ 0x01020000U);
    changed.set_runtime_word_10(17U);
    changed.set_packed_runtime_state(first.packed_runtime_state() ^ 0x0040U);
    rloft::Lev021LooseRecord appended{};
    appended.set_object_type_id(79U);
    const auto x = static_cast<std::uint32_t>(2U << 16U);
    const auto y = static_cast<std::uint32_t>(1U << 16U);
    appended.set_position_16_16(x, y, static_cast<std::uint16_t>(before.width + 2));
    const auto old_second = file.loose_item(1U);
    file.apply_loose_patches({
        {0U, first, changed},
        {1U, old_second, std::nullopt},
        {old_count, std::nullopt, appended},
    });
    need(file.encode().size() == original.size() &&
         file.summary().boundaries == before.boundaries &&
         file.summary().world_counts[2] == old_count &&
         file.loose_item(0U).raw == changed.raw &&
         file.loose_item(old_count - 1U).raw == appended.raw &&
         file.bytes().at(0U) == original.at(0U),
         "mixed family-C insert/erase/replace was not exact");
    const auto committed = file.encode();
    rejects([&] { file.apply_loose_patches({{0U, first, changed}}); });
    rejects([&] { file.apply_loose_patches({{old_count + 1U, std::nullopt, appended}}); });
    rejects([&] { file.apply_loose_patches({{0U, std::nullopt, std::nullopt}}); });
    rejects([&] { file.apply_loose_patches({{0U, changed, first},
                                            {0U, changed, first}}); });
    auto invalid = appended;
    invalid.set_position_16_16(0xFFFF0000U, y, 0U);
    rejects([&] { file.apply_loose_patches({{old_count, std::nullopt, invalid}}); });
    invalid = appended;
    invalid.set_position_16_16(x, y, 0U);
    rejects([&] { file.apply_loose_patches({{old_count, std::nullopt, invalid}}); });
    need(file.encode() == committed, "failed patch partially committed");

    file.apply_loose_patches({
        {0U, changed, first},
        {1U, std::nullopt, old_second},
        {old_count - 1U, appended, std::nullopt},
    });
    need(file.encode() == original, "mixed family-C inverse not byte-exact");
    const auto word = file.cell_word(0, 0);
    file.set_cell_word(0, 0, static_cast<std::uint16_t>(word ^ 0x0400U));
    file.set_cell_word(0, 0, word);
    file.set_music_sequence(static_cast<std::uint8_t>(before.music_sequence ^ 1U));
    file.set_music_sequence(before.music_sequence);
    file.apply_loose_patches({});
    need(file.encode() == original, "shared codec or empty operation changed source");
    rejects([&] { (void)file.loose_item(before.world_counts[2]); });
    auto corrupted = original;
    corrupted[1U] = 0U;
    rejects([&] { (void)rloft::Lev021File(corrupted); });
    corrupted = original;
    corrupted.pop_back();
    rejects([&] { (void)rloft::Lev021File(corrupted); });
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2) throw std::invalid_argument("usage: LEV021_tests [LEV021.DAT]");
    if (argc == 2) {
        std::ifstream in(argv[1], std::ios::binary);
        if (!in) throw std::runtime_error("unable to open original LEV021.DAT");
        const std::vector<std::uint8_t> original{
            std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
        exercise(original, true);
    }
}
