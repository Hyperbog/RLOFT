#include "rloft/LEV022.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* detail) {
    if (!condition) throw std::runtime_error(detail);
}
template<class Callable> void rejects(Callable action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "invalid LEV022 operation did not fail");
}
void put16(std::vector<std::uint8_t>& bytes, std::size_t at, unsigned value) {
    bytes.at(at) = static_cast<std::uint8_t>(value);
    bytes.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
void put32(std::vector<std::uint8_t>& bytes, std::size_t at, unsigned value) {
    for (unsigned i = 0; i != 4U; ++i)
        bytes.at(at + i) = static_cast<std::uint8_t>(value >> (8U * i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t header = 0x1DU, grid = header + 0x2A4U;
    constexpr unsigned width = 3U, height = 2U;
    constexpr std::size_t b1 = grid + width * height * 2U;
    constexpr std::size_t b2 = b1 + width * 2U + 1U + width * 2U;
    constexpr std::size_t b3 = b2 + 22U, b4 = b3 + 2U;
    constexpr std::size_t b5 = b4, b6 = b5 + 0x514U;
    constexpr std::array<std::size_t, 7> bounds{{header,b1,b2,b3,b4,b5,b6}};
    std::vector<std::uint8_t> bytes(b6 + 4U + width*height + 4U, 0U);
    bytes[0] = 0xD1U; // The original writer leaves this byte unowned.
    for (std::size_t i = 0; i < bounds.size(); ++i)
        put32(bytes, 1U + i*4U, static_cast<unsigned>(bounds[i]));
    put16(bytes, header + 1U, width);
    put16(bytes, header + 3U, height);
    put16(bytes, header + 0x28AU, 1U); // One NPC.
    put16(bytes, header + 0x28EU, 0U); // No placed objects.
    // Both width-indexed control groups are empty. The animation byte is 0.
    put32(bytes, b2, 0x00000000U);       // NPC y in 16.16 format.
    put32(bytes, b2 + 4U, 0x00010000U); // NPC x cell 1.
    put16(bytes, b2 + 12U, 1U);         // Cached map cell.
    put16(bytes, b2 + 14U, 9U);         // Assassin type.
    put16(bytes, b2 + 18U, 12U);        // NPC HP.
    put16(bytes, b3, 0U);               // Placed-object inline count.
    put16(bytes, grid, 0x0400U);        // One explored cell.
    put16(bytes, b6, width);
    put16(bytes, b6 + 2U, height);
    for (unsigned i = 0; i < width*height; ++i)
        bytes[b6 + 4U + i] = 0xFFU;
    put32(bytes, b6 + 4U + width*height, 0U);
    return bytes;
}

void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev022File file(original);
    require(file.encode() == original &&
            file.decode().storage == rloft::LevelStorage::DirectSave,
            "LEV022 lossless original decode/encode");
    const auto before = file.summary();
    const auto state = file.snapshot();
    if (retail) {
        require(original.size() == 6028U && before.width == 30 &&
                before.height == 15 && before.group1_controls == 10U &&
                before.group2_controls == 0U &&
                before.world_counts == std::array<std::size_t, 3>{{1U,110U,0U}} &&
                before.active_event_slots == 0U &&
                before.active_animation_count == 0U &&
                before.annotation_bytes == 0U &&
                before.writer_unowned_byte == 0U,
                "uploaded LEV022 fixture identity differs");
        require(state.cells == 450U && state.explored == 446U &&
                state.currently_visible == 50U &&
                state.cache_unmapped == 0U &&
                state.cache_floor_class == 450U,
                "uploaded LEV022 exploration/cache data differs");
        const auto actor = file.npc(0U);
        require(actor.npc_type_id() == 9U &&
                (actor.x_position_16_16() >> 16U) == 24U &&
                (actor.y_position_16_16() >> 16U) == 7U &&
                actor.current_hp() == -6 &&
                ((actor.packed_runtime_state() >> 3U) & 7U) == 6U,
                "uploaded Level 22 Assassin checkpoint changed");
    } else {
        require(state.cells == 6U && state.explored == 1U &&
                state.currently_visible == 0U && state.cache_unmapped == 6U &&
                file.npc(0U).current_hp() == 12,
                "synthetic checkpoint decoding invalid");
    }

    const auto old_word = file.cell_word(0, 0);
    const auto old_cache = file.automap_cell(0, 0);
    const auto old_npc = file.npc(0U);
    auto updated_npc = old_npc;
    updated_npc.set_action_state_countdown(
        static_cast<std::uint16_t>(old_npc.action_state_countdown() + 1U));
    const rloft::Lev022ExplorationPatch p{
        0, 0, old_word, static_cast<std::uint16_t>(old_word ^ 0x0440U),
        old_cache, static_cast<std::uint8_t>(old_cache ^ 0x03U)};
    file.apply_checkpoint({{p}, {{0U, old_npc, updated_npc}}});
    require(file.cell_word(0, 0) == p.replacement_word &&
            file.automap_cell(0, 0) == p.replacement_cache &&
            file.npc(0U).raw == updated_npc.raw &&
            file.encode().size() == original.size() &&
            file.summary().boundaries == before.boundaries &&
            file.bytes()[0] == original[0],
            "atomic exploration+NPC checkpoint edit failed");
    const auto committed = file.encode();
    // No partial commit on stale references, duplicate cells, bad positions
    // or any attempt to change level geometry/renderer control bits.
    rejects([&]{ file.apply_checkpoint({{p}, {}}); });
    rejects([&]{ file.apply_checkpoint({{p, p}, {}}); });
    auto invalid = p;
    invalid.expected_word = p.replacement_word;
    invalid.expected_cache = p.replacement_cache;
    invalid.replacement_word ^= 0x0001U;
    rejects([&]{ file.apply_checkpoint({{invalid}, {}}); });
    invalid = p;
    invalid.column = before.width;
    rejects([&]{ file.apply_checkpoint({{invalid}, {}}); });
    const auto bad_actor = file.npc(0U);
    auto moved = bad_actor;
    moved.set_position_16_16(0xFFFF0000U, 0U, 0U);
    rejects([&]{ file.apply_checkpoint({{}, {{0U,bad_actor,moved}}}); });
    require(file.encode() == committed, "failed edit modified live state");

    const rloft::Lev022ExplorationPatch undo{
        0, 0, p.replacement_word, old_word, p.replacement_cache, old_cache};
    file.apply_checkpoint({{undo}, {{0U, updated_npc, old_npc}}});
    require(file.encode() == original,
            "inverse checkpoint did not restore every original byte");
    file.apply_checkpoint({});
    require(file.encode() == original, "empty checkpoint changed level");
    auto corrupted = original;
    corrupted[1] = 0U;
    rejects([&]{ (void)rloft::Lev022File(corrupted); });
    corrupted = original;
    corrupted.pop_back();
    rejects([&]{ (void)rloft::Lev022File(corrupted); });
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2) throw std::invalid_argument("usage: LEV022_tests [LEV022.DAT]");
    if (argc == 2) {
        std::ifstream in(argv[1], std::ios::binary);
        if (!in) throw std::runtime_error("cannot open original LEV022.DAT");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
        exercise(bytes, true);
    }
}
