#include "rloft/LEV001.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void need(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
template <class F> void rejects(F f) {
    bool rejected = false;
    try { f(); } catch (const std::exception&) { rejected = true; }
    need(rejected, "invalid LEVnnn operation accepted");
}
std::vector<std::uint8_t> load(const char* name) {
    std::ifstream stream(name, std::ios::binary);
    if (!stream) throw std::runtime_error("cannot open original level file");
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}
void check(const std::vector<std::uint8_t>& bytes, bool first_level) {
    rloft::Lev001File original(bytes);
    need(original.encode() == bytes, "original standalone level not byte-exact");
    const auto info = original.summary();
    const auto level = original.decode();
    need(level.storage == rloft::LevelStorage::DirectSave &&
         info.width == level.width && info.height == level.height &&
         info.world_counts[0] == level.family_a_raw.size()/22U &&
         info.world_counts[1] == (level.family_b_raw.size()-2U)/22U &&
         info.world_counts[2] == level.family_c_raw.size()/22U,
         "direct-level metadata not derived from original decoded state");
    if (first_level) {
        need(bytes.size() == 7723U && info.width == 29 && info.height == 33 &&
             info.group1_controls == 17U && info.group2_controls == 13U &&
             info.world_counts == std::array<std::size_t,3>{{21U,77U,12U}} &&
             info.active_event_slots == 8U && info.active_event_commands == 13U &&
             info.music_sequence == 34U && info.writer_unowned_byte == 0U &&
             info.annotation_bytes == 0U, "uploaded LEV001.DAT inventory mismatch");
    } else {
        need(bytes.size() == 6028U && info.width == 30 && info.height == 15 &&
             info.world_counts == std::array<std::size_t,3>{{1U,110U,0U}} &&
             info.writer_unowned_byte == 0xDFU,
             "original LEV022.DAT companion sample differs");
    }
    auto modified = original;
    const auto original_word = modified.cell_word(1, 2);
    modified.set_cell_word(1, 2, static_cast<std::uint16_t>(original_word ^ 0x0440U));
    modified.set_music_sequence(68U);
    const rloft::Lev001File reload(modified.encode());
    need(reload.cell_word(1,2) == (original_word ^ 0x0440U) &&
         reload.summary().music_sequence == 68U &&
         reload.summary().world_counts == info.world_counts &&
         reload.bytes()[0] == bytes[0], "cell/music edit failed or changed world state");
    // Append a real 22-byte runtime-shaped record to family A, then remove
    // the most recently appended one. Verify full byte-for-byte restoration.
    auto a = level.family_a_raw;
    a.insert(a.end(), 22U, 0U);
    modified = original;
    modified.replace_world_family(rloft::LevWorldFamily::Npcs, a);
    need(modified.summary().world_counts[0] == info.world_counts[0] + 1U &&
         modified.encode().size() == bytes.size() + 22U,
         "NPC family growth did not relocate section boundaries");
    modified.replace_world_family(rloft::LevWorldFamily::Npcs, level.family_a_raw);
    need(modified.encode() == bytes, "NPC family restoration not byte exact");
    // Placed records contain two-byte serialized count before 22-byte entries.
    const std::vector<std::uint8_t> b(level.family_b_raw.begin()+2U,
                                       level.family_b_raw.end());
    modified.replace_world_family(rloft::LevWorldFamily::Placed, {});
    need(modified.summary().world_counts[1] == 0U &&
         modified.decode().family_b_raw.size() == 2U,
         "empty placed family lacks original two-byte count prefix");
    modified.replace_world_family(rloft::LevWorldFamily::Placed, b);
    need(modified.encode() == bytes, "placed family restoration changed original");
    auto c = level.family_c_raw;
    c.insert(c.end(), 22U, 0U);
    modified.replace_world_family(rloft::LevWorldFamily::Loose, c);
    need(modified.summary().world_counts[2] == info.world_counts[2]+1U,
         "loose family size not updated");
    modified.replace_world_family(rloft::LevWorldFamily::Loose, level.family_c_raw);
    need(modified.encode() == bytes, "loose family restoration not byte exact");
    modified.replace_automap_annotations({0x10U,0x20U,0x30U,0x40U});
    need(modified.summary().annotation_bytes == 4U &&
         modified.decode().automap_annotations ==
           std::vector<std::uint8_t>({0x10U,0x20U,0x30U,0x40U}) &&
         modified.encode().size() == bytes.size()+4U,
         "automap annotation growth failed");
    modified.replace_automap_annotations({});
    need(modified.encode() == bytes, "annotation restoration changed source");
    const auto preserved = modified.encode();
    rejects([&] { modified.set_cell_word(-1,0,1U); });
    rejects([&] { modified.set_cell_word(info.width,0,1U); });
    rejects([&] { modified.replace_world_family(rloft::LevWorldFamily::Npcs,{1U}); });
    rejects([&] { modified.replace_world_family(static_cast<rloft::LevWorldFamily>(6),{}); });
    rejects([&] { modified.replace_automap_annotations(
        std::vector<std::uint8_t>(2047U,0U)); });
    need(modified.encode() == preserved, "failed edit mutated original file");
    auto damaged = bytes;
    damaged[1] = 0U; // Must still begin with boundary 0x1D.
    rejects([&] { (void)rloft::Lev001File(damaged); });
    damaged = bytes;
    damaged[static_cast<std::size_t>(info.boundaries[5])] = 6U;
    rejects([&] { (void)rloft::Lev001File(damaged); });
    damaged = bytes;
    damaged.pop_back();
    rejects([&] { (void)rloft::Lev001File(damaged); });
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned n) {
    b.at(at) = static_cast<std::uint8_t>(n);
    b.at(at+1U) = static_cast<std::uint8_t>(n>>8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned n) {
    for (unsigned i=0;i<4U;++i)
        b.at(at+i) = static_cast<std::uint8_t>(n>>(8U*i));
}
void synthetic() {
    constexpr std::size_t header = 0x1DU, grid = header + 0x2A4U;
    constexpr std::size_t b1 = grid + 2U*2U*2U;
    constexpr std::size_t b2 = b1 + 2U*2U + 1U + 2U*2U;
    constexpr std::size_t b3 = b2, b4 = b3 + 2U, b5 = b4;
    constexpr std::size_t b6 = b5 + 0x514U;
    std::vector<std::uint8_t> bytes(b6 + 4U + 4U + 4U, 0U);
    bytes[0] = 0xA5U; // Original writer doesn't own the first byte.
    const std::array<std::size_t,7> bounds{{header,b1,b2,b3,b4,b5,b6}};
    for (std::size_t i=0;i<bounds.size();++i)
        put32(bytes, 1U + i*4U, static_cast<unsigned>(bounds[i]));
    put16(bytes,header+1U,2U); put16(bytes,header+3U,2U);
    bytes[header+0x290U] = 0xFFU;
    bytes[header+0x29CU] = 7U;
    put16(bytes,b3,0U); // Family B has a mandatory 2-byte count.
    put16(bytes,b6,2U); put16(bytes,b6+2U,2U);
    put32(bytes,b6+8U,0U);
    rloft::Lev001File level(bytes);
    need(level.encode()==bytes && level.summary().world_counts ==
         std::array<std::size_t,3>{{0U,0U,0U}} &&
         level.summary().writer_unowned_byte==0xA5U,
         "synthetic direct-level layout invalid");
    level.replace_world_family(rloft::LevWorldFamily::Placed,
                               std::vector<std::uint8_t>(22U,0U));
    need(level.summary().world_counts[1]==1U &&
         level.encode().size()==bytes.size()+22U,
         "synthetic placed-world record change failed");
    level.replace_world_family(rloft::LevWorldFamily::Placed,{});
    need(level.encode()==bytes,"synthetic modified world not reversible");
    level.set_cell_word(1,1,0x0440U);
    need(level.cell_word(1,1)==0x0440U,"synthetic cell edit failed");
    rejects([&]{ level.replace_world_family(rloft::LevWorldFamily::Npcs,
                                              std::vector<std::uint8_t>(23U,0U)); });
}
} // namespace

int main(int argc, char** argv) {
    synthetic();
    if (argc > 3)
        throw std::invalid_argument("usage: LEV001_tests [LEV001.DAT [LEV022.DAT]]");
    if (argc >= 2) check(load(argv[1]), true);
    if (argc == 3) check(load(argv[2]), false);
}
