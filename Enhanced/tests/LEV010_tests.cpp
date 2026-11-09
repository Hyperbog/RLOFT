#include "rloft/LEV010.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool ok, const char* reason) {
    if (!ok) throw std::runtime_error(reason);
}
template<class F> void rejects(F&& f) {
    bool rejected = false;
    try { f(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "invalid LEV010 change was accepted");
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned word) {
    b.at(at) = static_cast<std::uint8_t>(word);
    b.at(at + 1U) = static_cast<std::uint8_t>(word >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned word) {
    for (unsigned i = 0; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(word >> (i * 8U));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t header = 0x1DU;
    constexpr std::size_t grid = header + 0x2A4U;
    constexpr unsigned width = 3U, height = 2U;
    constexpr std::size_t b1 = grid + 2U * width * height;
    constexpr std::size_t b2 = b1 + 2U * width + 1U + 2U * width;
    constexpr std::size_t b3 = b2;
    constexpr std::size_t b4 = b3 + 2U; // Original family-B count word.
    constexpr std::size_t b5 = b4;
    constexpr std::size_t b6 = b5 + 0x514U;
    std::vector<std::uint8_t> b(b6 + 4U + width * height + 4U, 0U);
    b[0] = 0xDFU; // Writer does not own byte 0.
    constexpr std::array<std::size_t,7> bounds{{header,b1,b2,b3,b4,b5,b6}};
    for (std::size_t i = 0; i < bounds.size(); ++i)
        put32(b, 1U + i * 4U, static_cast<unsigned>(bounds[i]));
    put16(b, header + 1U, width); put16(b, header + 3U, height);
    b[header + 0x29CU] = 3U;
    put16(b, grid, 0x003FU); put16(b, grid + 10U, 0x0440U);
    put16(b, b3, 0U);
    put16(b, b6, width); put16(b, b6 + 2U, height);
    for (unsigned i = 0; i < width * height; ++i)
        b[b6 + 4U + i] = static_cast<std::uint8_t>(0x40U + i);
    put32(b, b6 + 4U + width * height, 0U);
    return b;
}
void exercise(const std::vector<std::uint8_t>& source, bool original) {
    rloft::Lev010File file(source);
    const auto initial = file.summary();
    require(file.encode() == source && file.decode().storage ==
                rloft::LevelStorage::DirectSave, "direct level round trip failed");
    if (original) {
        require(source.size() == 65052U && initial.width == 128 &&
                initial.height == 128 && initial.group1_controls == 121U &&
                initial.group2_controls == 26U &&
                initial.world_counts == std::array<std::size_t,3>{{54U,414U,73U}} &&
                initial.active_event_slots == 15U &&
                initial.active_event_commands == 19U &&
                initial.annotation_bytes == 0U &&
                initial.music_sequence == 3U &&
                initial.writer_unowned_byte == 0U,
                "uploaded Level 10 layout/source counts mismatch");
    }
    const int last_col = initial.width - 1;
    const int last_row = initial.height - 1;
    const auto first = file.cell_word(0,0);
    const auto last = file.cell_word(last_col,last_row);
    const auto middle = file.cell_word(1,1);
    const std::vector<rloft::Lev010CellPatch> patches{
        {0,0,first,static_cast<std::uint16_t>(first ^ 0x0440U)},
        {last_col,last_row,last,static_cast<std::uint16_t>(last ^ 0x0010U)},
        {1,1,middle,static_cast<std::uint16_t>(middle ^ 0x8000U)}};
    file.apply_cell_patches(patches);
    require(file.cell_word(0,0) == patches[0].replacement &&
            file.cell_word(last_col,last_row) == patches[1].replacement &&
            file.cell_word(1,1) == patches[2].replacement,
            "bulk map patches not applied");
    const auto edited = file.encode();
    const rloft::Lev010File reopened(edited);
    require(reopened.cell_word(0,0) == patches[0].replacement &&
            reopened.decode().cell_words.size() ==
                static_cast<std::size_t>(initial.width * initial.height),
            "patched Level 10 grid did not reload");
    for (std::size_t i = 0; i < source.size(); ++i) {
        const auto grid = 0x1DU + 0x2A4U;
        const auto is_word = [i](std::size_t index) {
            return i >= grid + 2U*index && i < grid + 2U*index + 2U;
        };
        const auto cells = static_cast<std::size_t>(initial.width*initial.height);
        if (!is_word(0U) && !is_word(cells-1U) &&
            !is_word(static_cast<std::size_t>(initial.width+1)))
            require(edited[i] == source[i], "bulk grid change touched unrelated byte");
    }
    rejects([&] { file.apply_cell_patches(patches); }); // Stale source bytes.
    rejects([&] { file.apply_cell_patches({patches[0],patches[0]}); });
    rejects([&] { file.apply_cell_patches({{initial.width,0,0,1}}); });
    rejects([&] { file.apply_cell_patches({{0,-1,0,1}}); });
    require(file.encode() == edited, "failed batch partially changed Level 10");
    std::vector<rloft::Lev010CellPatch> undo;
    for (const auto& p : patches)
        undo.push_back({p.column,p.row,p.replacement,p.expected});
    file.apply_cell_patches(undo);
    require(file.encode() == source, "cell patches did not reverse exactly");
    file.apply_cell_patches({});

    auto cache = file.decode().automap_cache;
    const auto previous = cache;
    cache[0U] ^= 0xFFU; cache.back() ^= 0x7FU;
    file.replace_automap_cache(cache);
    require(file.automap_cell(0,0) == cache[0] &&
            file.automap_cell(last_col,last_row) == cache.back() &&
            rloft::Lev010File(file.encode()).decode().automap_cache == cache,
            "automap cache edit not persisted");
    auto short_cache = cache; short_cache.pop_back();
    rejects([&] { file.replace_automap_cache(short_cache); });
    rejects([&] { (void)file.automap_cell(-1,0); });
    file.replace_automap_cache(previous);
    require(file.encode() == source, "automap restore changed opaque data");

    const auto original_event = file.event_slot(0);
    auto event = original_event;
    event.raw[0] = 1U; event.raw[1] = 4U;
    event.raw[2] ^= 0x55U;
    file.replace_event_slot(0U,event);
    require(file.event_slot(0U).raw == event.raw &&
            rloft::Lev010File(file.encode()).event_slot(0U).raw == event.raw,
            "event script failed to reload");
    const auto event_edit = file.encode();
    event.raw[0] = 6U;
    rejects([&] { file.replace_event_slot(0U,event); });
    event.raw[0] = 1U; event.raw[1] = 13U;
    rejects([&] { file.replace_event_slot(0U,event); });
    rejects([&] { (void)file.event_slot(50U); });
    require(file.encode() == event_edit, "failed event edit partially committed");
    file.replace_event_slot(0U,original_event);

    const auto original_loose = file.decode().family_c_raw;
    auto resized = original_loose;
    resized.insert(resized.end(), 22U, 0U);
    file.replace_world_family(rloft::LevWorldFamily::Loose,resized);
    require(file.summary().world_counts[2] == initial.world_counts[2]+1U &&
            file.event_slot(0U).raw == original_event.raw,
            "world-family resize damaged event offset");
    file.replace_world_family(rloft::LevWorldFamily::Loose, original_loose);
    file.set_music_sequence(0x22U);
    require(file.summary().music_sequence == 0x22U, "music edit failed");
    file.set_music_sequence(initial.music_sequence);
    const auto notes = file.decode().automap_annotations;
    auto extended = notes;
    extended.insert(extended.end(), {0x00U,0x01U,0x02U});
    file.replace_automap_annotations(extended);
    require(file.decode().automap_annotations == extended, "annotation resize failed");
    file.replace_automap_annotations(notes);
    require(file.encode() == source, "all source-accurate edits failed to reverse");

    auto bad = source; bad[1U] = 0U;
    rejects([&] { (void)rloft::Lev010File(bad); });
    bad = source; bad[static_cast<std::size_t>(initial.boundaries[5])] = 6U;
    rejects([&] { (void)rloft::Lev010File(bad); });
    bad = source; bad.pop_back();
    rejects([&] { (void)rloft::Lev010File(bad); });
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false); // CTest works without commercial media.
    if (argc > 2) throw std::invalid_argument("usage: LEV010_tests [LEV010.DAT]");
    if (argc == 2) {
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) throw std::runtime_error("cannot open original LEV010.DAT");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
        exercise(bytes, true);
    }
}
