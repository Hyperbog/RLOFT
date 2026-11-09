#include "rloft/LEV023.hpp"

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
using Bank = rloft::Lev023ResourceBank;
using Group = rloft::Lev019ControlGroup;

void need(bool result, const char* why) {
    if (!result) throw std::runtime_error(why);
}
template<class F> void rejects(F action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    need(failed, "LEV023 invalid edit was accepted");
}
void put16(std::vector<std::uint8_t>& b, std::size_t p, unsigned n) {
    b.at(p) = static_cast<std::uint8_t>(n);
    b.at(p + 1U) = static_cast<std::uint8_t>(n >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, std::uint32_t n) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.at(p + i) = static_cast<std::uint8_t>(n >> (8U * i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t h = 0x1DU, g = h + 0x2A4U;
    constexpr unsigned width = 4U, height = 3U;
    constexpr std::size_t b1 = g + 2U * width * height;
    constexpr std::size_t b2 = b1 + (2U*width + 10U) + 1U + 2U*width;
    constexpr std::size_t b3 = b2 + 22U;
    constexpr std::size_t b4 = b3 + 2U + 22U;
    constexpr std::size_t b5 = b4 + 2U * 22U, b6 = b5 + 0x514U;
    constexpr std::array<std::size_t, 7> bounds{{h,b1,b2,b3,b4,b5,b6}};
    std::vector<std::uint8_t> bytes(b6 + 4U + width*height + 4U, 0U);
    bytes[0] = 0xDBU;
    for (std::size_t i = 0U; i < bounds.size(); ++i)
        put32(bytes, 1U + i * 4U, static_cast<unsigned>(bounds[i]));
    put16(bytes, h + 1U, width);
    put16(bytes, h + 3U, height);
    put16(bytes, h + 0x28AU, 1U); // NPC family
    put16(bytes, h + 0x28CU, 2U); // loose-item family
    put16(bytes, h + 0x28EU, 1U); // placed-object family
    bytes[h + 0x29CU] = 1U;
    put16(bytes, h + 0x00AU + 2U, 0x0093U);
    put16(bytes, h + 0x08AU + 60U*2U, 0x024AU);
    put16(bytes, h + 0x08AU + 62U*2U, 0x0248U);
    put16(bytes, h + 0x18AU + 60U*2U, 0x024AU);
    put16(bytes, h + 0x18AU + 62U*2U, 0x0248U);
    put16(bytes, h + 0x20AU, 0xB5B1U);
    std::size_t p = b1;
    put16(bytes, p, 0U); p += 2U; // column 0
    put16(bytes, p, 1U); p += 2U; // column 1
    bytes[p] = 2U; bytes[p + 1U] = 1U;
    bytes[p + 4U] = 2U; bytes[p + 5U] = 1U; p += 10U;
    put16(bytes, p, 0U); p += 2U;
    put16(bytes, p, 0U); p += 2U;
    bytes[p++] = 0U; // animation
    for (unsigned i = 0U; i < width; ++i) { put16(bytes, p, 0U); p += 2U; }
    need(p == b2, "synthetic 10-byte control layout incorrect");
    put32(bytes, b2, 1U << 16U); put32(bytes, b2 + 4U, 1U << 16U);
    put16(bytes, b2 + 12U, width + 1U); put16(bytes, b2 + 14U, 27U);
    put16(bytes, b2 + 18U, 13U); // NPC current HP
    put16(bytes, b3, 1U); // inline placed count
    put32(bytes, b3 + 2U, 1U << 16U);
    put32(bytes, b3 + 6U, 2U << 16U);
    put16(bytes, b3 + 14U, width + 2U);
    put16(bytes, b3 + 16U, 84U);
    put16(bytes, b3 + 22U, 0x30U);
    for (unsigned i = 0U; i < 2U; ++i) {
        const auto at = b4 + 22U * i;
        const unsigned x = 1U + 2U * i;
        const unsigned y = 1U + i;
        put32(bytes, at, y << 16U);
        put32(bytes, at + 4U, x << 16U);
        put16(bytes, at + 12U, y * width + x);
        put16(bytes, at + 14U, i == 0U ? 12U : 164U);
    }
    bytes[b5] = 1U; bytes[b5+1U] = 12U; // one opcode-12 event
    put16(bytes, b6, width); put16(bytes, b6 + 2U, height);
    put32(bytes, b6 + 4U + width*height, 0U);
    return bytes;
}

void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev023File file(original);
    const auto baseline = file.summary();
    need(file.encode() == original &&
         file.decode().storage == rloft::LevelStorage::DirectSave,
         "LEV023 original was not lossless");
    if (retail) {
        need(original.size() == 20842U && baseline.width == 64 &&
             baseline.height == 64 && baseline.group1_controls == 43U &&
             baseline.group2_controls == 0U &&
             baseline.world_counts == std::array<std::size_t,3>{{34U,213U,19U}} &&
             baseline.active_event_slots == 3U &&
             baseline.active_event_commands == 6U &&
             baseline.music_sequence == 1U &&
             baseline.active_animation_count == 0U &&
             baseline.annotation_bytes == 0U,
             "uploaded LEV023 retail identity mismatch");
        const auto state = file.decode();
        need(std::count_if(state.cell_words.begin(), state.cell_words.end(),
                [](std::uint16_t w) {return (w & 0x0400U) != 0U;}) == 4089 &&
             std::count_if(state.cell_words.begin(), state.cell_words.end(),
                [](std::uint16_t w) {return (w & 0x0040U) != 0U;}) == 4,
             "LEV023 exploration state changed");
        need(file.npc(15U).npc_type_id() == 5U &&
             file.npc(15U).current_hp() == 8 &&
             file.placed_object(187U).object_type_id() == 84U &&
             file.placed_object(187U).packed_runtime_state() == 0x70U &&
             file.loose_item(0U).object_type_id() == 12U,
             "LEV023 original world-state identity changed");
    }
    need(file.resource_word(Bank::AlternateA, 60U) == 0x024AU &&
         file.resource_word(Bank::AlternateA, 62U) == 0x0248U &&
         file.resource_word(Bank::AlternateB, 60U) == 0x024AU &&
         file.resource_word(Bank::AlternateB, 62U) == 0x0248U,
         "LEV023 live lookup-table state changed");
    const int col = retail ? 12 : 1;
    const std::size_t actor_index = retail ? 15U : 0U;
    const std::size_t placed_index = retail ? 187U : 0U;
    const auto original_control = file.controls(Group::Primary, col);
    need(original_control.size() == 1U && original_control[0].kind() == 2U &&
         original_control[0].control() == 1U,
         "LEV023 selected kind-2 source record changed");
    const auto original_npc = file.npc(actor_index);
    const auto original_placed = file.placed_object(placed_index);
    const auto original_loose = file.loose_item(0U);
    const std::size_t loose_count = baseline.world_counts[2];
    const auto original_scripts = file.decode().event_scripts;
    const auto original_grid = file.decode().cell_words;
    const auto original_cache = file.decode().automap_cache;

    auto changed_controls = original_control;
    changed_controls[0].raw[1U] = 4U;
    auto changed_npc = original_npc;
    changed_npc.set_current_hp(static_cast<std::int16_t>(original_npc.current_hp() - 1));
    auto changed_placed = original_placed;
    changed_placed.set_packed_runtime_state(
        static_cast<std::uint16_t>(original_placed.packed_runtime_state() ^ 0x0040U));
    rloft::Lev023Edits edits;
    edits.resources = {
        {Bank::AlternateA,60U,0x024AU,0x0248U},
        {Bank::AlternateB,62U,0x0248U,0x024AU},
    };
    edits.controls = {{Group::Primary,col,original_control,changed_controls}};
    edits.npcs = {{actor_index,original_npc,changed_npc}};
    edits.placed = {{placed_index,original_placed,changed_placed}};
    // A live item is moved from the front to the end; all bytes are retained.
    edits.loose = {{0U,original_loose,std::nullopt},
                   {loose_count,std::nullopt,original_loose}};
    file.apply(edits);
    const auto after = file.decode();
    need(file.encode() != original && file.encode().size() == original.size() &&
         file.summary().boundaries == baseline.boundaries &&
         file.resource_word(Bank::AlternateA,60U) == 0x0248U &&
         file.resource_word(Bank::AlternateB,62U) == 0x024AU &&
         file.controls(Group::Primary,col)[0].control() == 4U &&
         file.npc(actor_index).raw == changed_npc.raw &&
         file.placed_object(placed_index).raw == changed_placed.raw &&
         file.loose_item(loose_count-1U).raw == original_loose.raw &&
         file.bytes()[0] == original[0] &&
         after.cell_words == original_grid &&
         after.automap_cache == original_cache &&
         after.event_scripts == original_scripts,
         "LEV023 atomic cross-domain edit failed or mutated unrelated data");
    const auto committed = file.encode();
    rejects([&]{file.apply(edits);}); // stale resource and record bytes
    auto invalid = rloft::Lev023Edits{};
    invalid.resources = {
        {Bank::AlternateA,60U,0x0248U,0x024AU},
        {Bank::AlternateA,60U,0x0248U,0x024AU},
    };
    rejects([&]{file.apply(invalid);});
    invalid.resources = {{static_cast<Bank>(9U),60U,0U,1U}};
    rejects([&]{file.apply(invalid);});
    invalid.resources = {{Bank::PrimaryA,64U,0U,1U}};
    rejects([&]{file.apply(invalid);});
    invalid.resources = {{Bank::PrimaryA,0U,0xFFFFU,0U}};
    invalid.npcs = {{actor_index,changed_npc,original_npc}};
    rejects([&]{file.apply(invalid);}); // late failure must roll back valid NPC stage
    need(file.encode() == committed, "LEV023 failed transaction modified source");
    rloft::Lev023Edits undo;
    undo.resources = {
        {Bank::AlternateA,60U,0x0248U,0x024AU},
        {Bank::AlternateB,62U,0x024AU,0x0248U},
    };
    undo.controls = {{Group::Primary,col,changed_controls,original_control}};
    undo.npcs = {{actor_index,changed_npc,original_npc}};
    undo.placed = {{placed_index,changed_placed,original_placed}};
    undo.loose = {{0U,std::nullopt,original_loose},
                  {loose_count-1U,original_loose,std::nullopt}};
    file.apply(undo);
    need(file.encode() == original, "LEV023 inverse transaction not byte-identical");
    file.apply({});
    need(file.encode() == original, "LEV023 empty edit was not a no-op");
    rejects([&]{(void)file.resource_word(Bank::PrimaryA,64U);});
    auto truncated = original;
    truncated.pop_back();
    rejects([&]{(void)rloft::Lev023File(truncated);});
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2) throw std::invalid_argument("usage: LEV023_tests [LEV023.DAT]");
    if (argc == 2) {
        std::ifstream stream(argv[1], std::ios::binary);
        if (!stream) throw std::runtime_error("cannot open original LEV023.DAT");
        const std::vector<std::uint8_t> source{
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
        exercise(source, true);
    }
}
