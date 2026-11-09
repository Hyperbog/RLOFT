#include "rloft/LEV005.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void need(bool ok, const char* why) {
    if (!ok) throw std::runtime_error(why);
}
template <class F> void rejects(F&& f) {
    bool rejected = false;
    try { f(); } catch (const std::exception&) { rejected = true; }
    need(rejected, "LEV005 accepted an invalid edit");
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned n) {
    b.at(at) = static_cast<std::uint8_t>(n);
    b.at(at + 1U) = static_cast<std::uint8_t>(n >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned n) {
    for (unsigned i = 0; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(n >> (8U * i));
}
std::vector<std::uint8_t> synthetic_save() {
    constexpr std::size_t header = 0x1DU;
    constexpr std::size_t grid = header + 0x2A4U;
    constexpr std::size_t b1 = grid + 2U * 2U * 2U;
    constexpr std::size_t b2 = b1 + 4U + 1U + 4U;
    constexpr std::size_t b3 = b2;
    constexpr std::size_t b4 = b3 + 2U;
    constexpr std::size_t b5 = b4;
    constexpr std::size_t b6 = b5 + 0x514U;
    std::vector<std::uint8_t> b(b6 + 4U + 4U + 4U, 0U);
    b[0] = 0xDFU; // Writer-unowned first byte.
    constexpr std::array<std::size_t, 7> bounds{
        {header, b1, b2, b3, b4, b5, b6}};
    for (std::size_t i = 0; i < bounds.size(); ++i)
        put32(b, 1U + 4U * i, static_cast<unsigned>(bounds[i]));
    put16(b, header + 1U, 2U); put16(b, header + 3U, 2U);
    b[header + 0x29CU] = 8U;
    put16(b, b3, 0U);
    put16(b, b6, 2U); put16(b, b6 + 2U, 2U);
    put32(b, b6 + 8U, 0U);
    return b;
}
void exercise(std::vector<std::uint8_t> bytes, bool original) {
    rloft::Lev005File f(bytes);
    need(f.encode() == bytes && f.decode().storage == rloft::LevelStorage::DirectSave,
         "direct level did not round trip byte-exactly");
    const auto baseline = f.summary();
    if (original) {
        need(bytes.size() == 22372U && baseline.width == 64 &&
             baseline.height == 64 && baseline.group1_controls == 61U &&
             baseline.group2_controls == 0U && baseline.music_sequence == 8U &&
             baseline.world_counts == std::array<std::size_t, 3>{{90U, 163U, 73U}} &&
             baseline.active_event_slots == 26U &&
             baseline.active_event_commands == 54U &&
             baseline.annotation_bytes == 30U &&
             baseline.writer_unowned_byte == 0U,
             "uploaded Level 5 source inventory mismatch");
    }
    auto event = f.event_slot(0U);
    need(event.active_count() == (original ? 2U : 0U),
         "original event slot decoding changed");
    need(event.command(0U).size() == 5U, "event command size incorrect");
    rejects([&] { (void)event.command(5U); });
    rejects([&] { (void)f.event_slot(50U); });
    auto changed = event;
    changed.raw[0] = 1U;
    changed.raw[1] = 4U; // Original CODE.2 opcode 4.
    changed.raw[2] ^= 0x44U; // Preserve all inactive command bytes.
    f.replace_event_slot(0U, changed);
    need(f.event_slot(0U).raw == changed.raw &&
         f.summary().active_event_commands ==
             baseline.active_event_commands - event.active_count() + 1U,
         "event slot edit did not update summary");
    const auto reload = rloft::Lev005File(f.encode());
    need(reload.event_slot(0U).raw == changed.raw,
         "modified event command failed to reload");
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (i < baseline.boundaries[5] ||
            i >= baseline.boundaries[5] + rloft::Lev005EventSlot::kBytes)
            need(f.bytes()[i] == bytes[i], "edited event changed unrelated bytes");
    }
    const auto committed = f.encode();
    changed.raw[0] = 6U;
    rejects([&] { f.replace_event_slot(0U, changed); });
    changed.raw[0] = 1U; changed.raw[1] = 13U;
    rejects([&] { f.replace_event_slot(0U, changed); });
    rejects([&] { f.replace_event_slot(50U, event); });
    need(f.encode() == committed, "invalid event edit partially committed");
    f.replace_event_slot(0U, event);
    need(f.encode() == bytes, "event slot restoration differs from original");
    const auto word = f.cell_word(1, 1);
    f.set_cell_word(1, 1, static_cast<std::uint16_t>(word ^ 0x0440U));
    need(f.cell_word(1, 1) == (word ^ 0x0440U), "cell editor failed");
    f.set_cell_word(1, 1, word);
    f.set_music_sequence(22U);
    need(f.summary().music_sequence == 22U, "music editor failed");
    f.set_music_sequence(baseline.music_sequence);
    const auto npc = f.decode().family_a_raw;
    auto expanded = npc;
    expanded.insert(expanded.end(), 22U, 0U);
    f.replace_world_family(rloft::LevWorldFamily::Npcs, expanded);
    need(f.summary().world_counts[0] == baseline.world_counts[0] + 1U &&
         f.event_slot(0U).raw == event.raw,
         "resized world family moved event table incorrectly");
    f.replace_world_family(rloft::LevWorldFamily::Npcs, npc);
    const auto notes = f.decode().automap_annotations;
    auto edited_notes = notes;
    edited_notes.insert(edited_notes.end(), {0x00U, 0x01U, 0x02U});
    f.replace_automap_annotations(edited_notes);
    need(f.decode().automap_annotations == edited_notes,
         "Level 5 automap annotations did not update");
    f.replace_automap_annotations(notes);
    need(f.encode() == bytes, "cell/world/notes restoration is not byte-exact");
    auto damaged = bytes;
    damaged[1U] = 0U;
    rejects([&] { (void)rloft::Lev005File(damaged); });
    damaged = bytes;
    damaged[static_cast<std::size_t>(baseline.boundaries[5])] = 6U;
    rejects([&] { (void)rloft::Lev005File(damaged); });
    damaged = bytes;
    damaged.pop_back();
    rejects([&] { (void)rloft::Lev005File(damaged); });
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic_save(), false);
    if (argc > 2) throw std::invalid_argument("usage: LEV005_tests [LEV005.DAT]");
    if (argc == 2) {
        std::ifstream in(argv[1], std::ios::binary);
        if (!in) throw std::runtime_error("cannot open original LEV005.DAT");
        std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
        exercise(std::move(bytes), true);
    }
}
