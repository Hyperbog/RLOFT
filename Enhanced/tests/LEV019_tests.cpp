#include "rloft/LEV019.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void need(bool yes, const char* why) {
    if (!yes) throw std::runtime_error(why);
}
template<class F> void rejects(F action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    need(rejected, "LEV019 accepted malformed control edit");
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned n) {
    b.at(at) = static_cast<std::uint8_t>(n);
    b.at(at + 1U) = static_cast<std::uint8_t>(n >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned n) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(n >> (8U * i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t header = 0x1DU;
    constexpr std::size_t grid = header + 0x2A4U;
    constexpr unsigned width = 3U, height = 2U;
    constexpr std::size_t b1 = grid + 2U * width * height;
    constexpr std::size_t b2 = b1 + (2U * width + 10U) + 1U +
                               (2U * width + 10U);
    constexpr std::size_t b3 = b2, b4 = b3 + 2U;
    constexpr std::size_t b5 = b4, b6 = b5 + 0x514U;
    std::vector<std::uint8_t> b(b6 + 4U + width * height + 4U, 0U);
    b[0] = 0xDFU;  // Original writer does not own byte zero.
    for (std::size_t i = 0U; i < 7U; ++i) {
        constexpr std::array<std::size_t, 7> bounds{
            {header, b1, b2, b3, b4, b5, b6}};
        put32(b, 1U + 4U * i, static_cast<unsigned>(bounds[i]));
    }
    put16(b, header + 1U, width); put16(b, header + 3U, height);
    b[header + 0x29CU] = 41U;
    std::size_t at = b1;
    // Primary: one record in column 0.
    put16(b, at, 1U); at += 2U;
    b[at] = 2U; b[at + 4U] = 1U; b[at + 5U] = 0U; at += 10U;
    put16(b, at, 0U); at += 2U;
    put16(b, at, 0U); at += 2U;
    b[at++] = 3U;  // Preserve active-animation byte.
    // Secondary: one record in column 1.
    put16(b, at, 0U); at += 2U;
    put16(b, at, 1U); at += 2U;
    b[at] = 1U; b[at + 4U] = 0U; b[at + 5U] = 1U; at += 10U;
    put16(b, at, 0U); at += 2U;
    need(at == b2, "synthetic control section misbuilt");
    put16(b, b3, 0U);  // 16-bit family-B count.
    put16(b, b6, width); put16(b, b6 + 2U, height);
    put32(b, b6 + 4U + width * height, 0U);
    return b;
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    using G = rloft::Lev019ControlGroup;
    rloft::Lev019File file(original);
    const auto baseline = file.summary();
    need(file.encode() == original &&
         file.decode().storage == rloft::LevelStorage::DirectSave,
         "LEV019 initial round trip mismatch");
    if (retail) {
        need(original.size() == 2996U && baseline.width == 10 &&
             baseline.height == 15 && baseline.group1_controls == 3U &&
             baseline.group2_controls == 2U &&
             baseline.world_counts ==
                 std::array<std::size_t, 3>{{2U, 16U, 2U}} &&
             baseline.active_event_slots == 1U &&
             baseline.active_event_commands == 2U &&
             baseline.music_sequence == 41U &&
             baseline.annotation_bytes == 0U &&
             baseline.active_animation_count == 0U,
             "retail LEV019.DAT inventory changed");
    }
    const int column = retail ? 3 : 0;
    auto prior = file.controls(G::Primary, column);
    need(!prior.empty(), "expected a source control list");
    auto expanded = prior;
    auto new_record = prior.front();
    new_record.raw[4U] = 0U;
    new_record.raw[5U] = static_cast<std::uint8_t>(column);
    new_record.raw[7U] ^= 0x5AU;
    expanded.push_back(new_record);
    const auto inactive = file.controls(G::Secondary, column);
    file.apply_control_patches({{G::Primary, column, prior, expanded},
                                {G::Secondary, column, inactive, inactive}});
    const auto changed = file.encode();
    need(changed.size() == original.size() + 10U &&
         file.controls(G::Primary, column).size() == prior.size() + 1U &&
         file.decode().active_animation_count == baseline.active_animation_count &&
         file.summary().group1_controls == baseline.group1_controls + 1U,
         "control growth failed or animation byte was lost");
    const rloft::Lev019File reloaded(changed);
    need(reloaded.controls(G::Primary, column).size() == expanded.size() &&
         reloaded.summary().boundaries[6] == baseline.boundaries[6] + 10U,
         "control shift broke section offsets");
    const auto old_tail = static_cast<std::size_t>(baseline.boundaries[2]);
    need(std::equal(original.begin() + static_cast<std::ptrdiff_t>(old_tail),
                    original.end(),
                    changed.begin() + static_cast<std::ptrdiff_t>(old_tail + 10U)),
         "editing controls modified later world/scripts/map bytes");
    need(file.bytes()[0U] == original[0U],
         "the writer-unowned first byte was overwritten");
    // Expected-byte guards and duplicate checks are atomic.
    const auto committed = file.encode();
    rejects([&]{file.apply_control_patches({{G::Primary,column,prior,{}}});});
    rejects([&]{file.apply_control_patches({
        {G::Primary,column,expanded,prior},
        {G::Primary,column,expanded,prior}});});
    auto invalid = expanded;
    invalid[0U].raw[5U] = static_cast<std::uint8_t>(column + 1);
    rejects([&]{file.apply_control_patches({{G::Primary,column,expanded,invalid}});});
    invalid = expanded;
    invalid[0U].raw[4U] = static_cast<std::uint8_t>(baseline.height);
    rejects([&]{file.apply_control_patches({{G::Primary,column,expanded,invalid}});});
    rejects([&]{file.apply_control_patches({
        {static_cast<G>(9U),column,expanded,prior}});});
    rejects([&]{(void)file.controls(G::Primary,baseline.width);});
    need(file.encode() == committed, "failed edits mutated LEV019");
    file.apply_control_patches({{G::Primary,column,expanded,prior}});
    need(file.encode() == original,
         "expanded control group did not restore byte-exactly");
    // Changing secondary controls independently tests the animation split.
    const int second_column = retail ? 4 : 1;
    const auto secondary = file.controls(G::Secondary, second_column);
    need(!secondary.empty(), "expected secondary control list");
    file.apply_control_patches({{G::Secondary, second_column, secondary, {}}});
    need(file.encode().size() == original.size() - 10U &&
         file.summary().group2_controls == baseline.group2_controls - 1U,
         "secondary column shrink failed");
    file.apply_control_patches({{G::Secondary, second_column, {}, secondary}});
    need(file.encode() == original,
         "secondary group restore not byte-exact");
    const auto word = file.cell_word(0, 0);
    file.set_cell_word(0, 0, static_cast<std::uint16_t>(word ^ 0x0400U));
    file.set_cell_word(0, 0, word);
    file.set_music_sequence(static_cast<std::uint8_t>(baseline.music_sequence + 1U));
    file.set_music_sequence(baseline.music_sequence);
    need(file.encode() == original, "shared cell/music codec changed bytes");
    auto corrupted = original;
    corrupted[1U] = 0U;
    rejects([&]{(void)rloft::Lev019File(corrupted);});
    corrupted = original;
    corrupted.pop_back();
    rejects([&]{(void)rloft::Lev019File(corrupted);});
}
}  // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2) throw std::invalid_argument("usage: LEV019_tests [LEV019.DAT]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("cannot open original LEV019.DAT");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
        exercise(bytes, true);
    }
}
