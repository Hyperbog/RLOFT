#include "rloft/LEV025.hpp"

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

void require(bool good, const char* detail) {
    if (!good) throw std::runtime_error(detail);
}
template<class Action> void rejects(Action action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "LEV025 accepted malformed or stale data");
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    b.at(at) = static_cast<std::uint8_t>(value);
    b.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(value >> (i * 8U));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t h = 0x1DU, grid = h + 0x2A4U;
    constexpr unsigned width = 4U, height = 3U;
    constexpr std::size_t b1 = grid + 2U * width * height;
    constexpr std::size_t b2 = b1 + 2U * width + 10U + 1U + 2U * width;
    constexpr std::size_t b3 = b2 + 22U;
    constexpr std::size_t b4 = b3 + 2U + 22U;
    constexpr std::size_t b5 = b4 + 2U * 22U;
    constexpr std::size_t b6 = b5 + 0x514U;
    constexpr std::array<std::size_t, 7> bounds{{h,b1,b2,b3,b4,b5,b6}};
    std::vector<std::uint8_t> b(b6 + 4U + width*height + 4U, 0U);
    b[0] = 0xA5U; // Writer-unowned original preamble byte.
    for (std::size_t i = 0; i < bounds.size(); ++i)
        put32(b, 1U + 4U*i, static_cast<unsigned>(bounds[i]));
    put16(b, h + 1U, width); put16(b, h + 3U, height);
    put16(b, h + 0x28AU, 1U); // NPC count.
    put16(b, h + 0x28CU, 2U); // Loose count.
    put16(b, h + 0x28EU, 1U); // Placed count.
    b[h + 0x29CU] = 2U; // Music sequence.
    put16(b, h + 0x00AU + 2U*60U, 582U);
    put16(b, h + 0x08AU + 2U*60U, 588U);
    put16(b, h + 0x10AU + 2U*60U, 582U);
    put16(b, h + 0x18AU + 2U*60U, 588U);
    put16(b, grid + 2U*(width + 2U), 0x0400U);
    std::size_t at = b1;
    for (unsigned column = 0; column < width; ++column) {
        put16(b, at, column == 2U ? 1U : 0U); at += 2U;
        if (column == 2U) {
            b[at] = 3U; b[at + 4U] = 1U; b[at + 5U] = 2U;
            at += 10U;
        }
    }
    b[at++] = 0U; // Source animation state.
    for (unsigned column = 0; column < width; ++column) {
        put16(b, at, 0U); at += 2U;
    }
    require(at == b2, "bad synthetic LEV025 control lists");
    put32(b,b2,1U<<16U); put32(b,b2+4U,1U<<16U);
    put16(b,b2+12U,width+1U); put16(b,b2+14U,6U);
    put16(b,b2+18U,15U);
    put16(b,b3,1U);
    put32(b,b3+2U,1U<<16U); put32(b,b3+6U,2U<<16U);
    put16(b,b3+14U,width+2U); put16(b,b3+16U,579U);
    for (unsigned i = 0; i < 2U; ++i) {
        const auto off = b4 + i*22U;
        put32(b,off,1U<<16U); put32(b,off+4U,(1U+i)<<16U);
        put16(b,off+12U,width+1U+i); put16(b,off+14U,i?164U:27U);
    }
    b[b5] = 1U; b[b5+1U] = 1U; b[b5+2U] = 1U;
    put16(b,b6,width); put16(b,b6+2U,height);
    for (unsigned i = 0; i < width*height; ++i)
        b[b6+4U+i] = 0xFFU;
    put32(b,b6+4U+width*height,0U); // No map notes.
    return b;
}

void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev025File file(original);
    const auto before = file.summary();
    const auto state = file.decode();
    require(file.encode() == original &&
            state.storage == rloft::LevelStorage::DirectSave,
            "LEV025 original direct-save decode/encode changed bytes");
    const int column = retail ? 22 : 2;
    const int row = retail ? 20 : 1;
    const int control_column = retail ? 23 : 2;
    const auto count = before.world_counts[2];
    if (retail) {
        require(original.size() == 15864U && before.width == 80 &&
                before.height == 32 && before.group1_controls == 26U &&
                before.group2_controls == 0U &&
                before.world_counts == std::array<std::size_t,3>{{53U,178U,23U}} &&
                before.active_event_slots == 4U &&
                before.active_event_commands == 13U &&
                before.annotation_bytes == 0U &&
                before.music_sequence == 2U &&
                before.active_animation_count == 0U,
                "uploaded LEV025 structure/identity mismatch");
        require(std::count_if(state.cell_words.begin(),state.cell_words.end(),
                [](std::uint16_t w) { return (w & 0x0400U) != 0U; }) == 1931 &&
                std::count_if(state.cell_words.begin(),state.cell_words.end(),
                [](std::uint16_t w) { return (w & 0x0040U) != 0U; }) == 7,
                "uploaded LEV025 explored/visible state mismatch");
        require(file.npc(0U).npc_type_id() == 6U &&
                file.placed_object(0U).object_type_id() == 67U &&
                file.loose_item(0U).object_type_id() == 27U &&
                file.event_slot(0U).active_count() == 5U &&
                file.event_slot(1U).active_count() == 4U &&
                file.event_slot(2U).active_count() == 3U &&
                file.event_slot(3U).active_count() == 1U,
                "uploaded LEV025 world/script source mismatch");
    }
    require(file.cell_word(column,row) == 0x0400U &&
            file.resource_word(Bank::PrimaryA,60U) == 582U &&
            file.resource_word(Bank::AlternateA,60U) == 588U &&
            file.resource_word(Bank::PrimaryB,60U) == 582U &&
            file.resource_word(Bank::AlternateB,60U) == 588U &&
            file.notes().empty(),
            "LEV025 terrain/resource source does not match fixture");
    const auto tile = file.cell_word(column,row);
    const auto cache = file.automap_cell(column,row);
    const auto event = file.event_slot(0U);
    auto changed_event = event;
    changed_event.raw[2U] ^= 1U; // Event payload, not opcode or command count.
    const auto npc = file.npc(0U);
    auto changed_npc = npc;
    changed_npc.set_current_hp(static_cast<std::int16_t>(npc.current_hp()-1));
    const auto placed = file.placed_object(0U);
    auto changed_placed = placed;
    changed_placed.set_packed_runtime_state(
        static_cast<std::uint16_t>(placed.packed_runtime_state() ^ 0x0040U));
    const auto controls = file.controls(Group::Primary,control_column);
    require(!controls.empty(), "LEV025 original control column not found");
    auto new_controls = controls;
    new_controls[0U].raw[1U] ^= 1U;
    rloft::Lev021LooseRecord added{};
    added.set_position_16_16(1U<<16U,1U<<16U,
                             static_cast<std::uint16_t>(before.width + 1));
    added.set_object_type_id(164U);
    const rloft::Lev011MapNote annotation{17,-9,{'L','E','V','2','5'}};

    rloft::Lev025Edits changed{};
    changed.world.resources = {{Bank::PrimaryA,60U,582U,583U}};
    changed.world.controls = {{Group::Primary,control_column,controls,new_controls}};
    changed.world.npcs = {{0U,npc,changed_npc}};
    changed.world.placed = {{0U,placed,changed_placed}};
    changed.world.loose = {{count,std::nullopt,added}};
    changed.world.notes = {{0U,std::nullopt,annotation}};
    changed.terrain = {{column,row,tile,14U}};
    changed.cache = {{column,row,cache,static_cast<std::uint8_t>(cache ^ 1U)}};
    changed.events = {{0U,event,changed_event}};
    file.apply(changed);
    require(file.cell_word(column,row) == 0x040EU &&
            file.automap_cell(column,row) == (cache ^ 1U) &&
            file.event_slot(0U).raw == changed_event.raw &&
            file.resource_word(Bank::PrimaryA,60U) == 583U &&
            file.npc(0U).raw == changed_npc.raw &&
            file.placed_object(0U).raw == changed_placed.raw &&
            file.controls(Group::Primary,control_column)[0U].raw ==
                new_controls[0U].raw &&
            file.loose_item(count).raw == added.raw &&
            file.notes().size() == 1U &&
            file.notes()[0U].label == annotation.label &&
            file.summary().world_counts[2] == count + 1U &&
            file.summary().annotation_bytes == annotation.label.size() + 6U &&
            file.encode().size() == original.size() + 22U + 6U + annotation.label.size() &&
            file.summary().boundaries[5] == before.boundaries[5] + 22U &&
            file.summary().boundaries[6] == before.boundaries[6] + 22U &&
            file.bytes()[0U] == original[0U],
            "LEV025 atomic multi-domain transaction failed");
    const auto committed = file.encode();
    rejects([&] { file.apply(changed); }); // Stale resource and cell.
    auto bad = rloft::Lev025Edits{};
    bad.terrain = {{column,row,0x040EU,64U}}; // Outside 6-bit cell code.
    rejects([&] { file.apply(bad); });
    bad.terrain = {{column,row,0x040EU,14U},{column,row,0x040EU,14U}};
    rejects([&] { file.apply(bad); });
    bad.terrain = {{-1,row,0x040EU,0U}};
    rejects([&] { file.apply(bad); });
    bad = {};
    bad.events = {{0U,changed_event,event},{0U,changed_event,event}};
    rejects([&] { file.apply(bad); });
    bad.events = {{0U,changed_event,event}};
    bad.events[0U].replacement.raw[1U] = 13U; // Invalid CODE.2 opcode.
    rejects([&] { file.apply(bad); });
    bad = {};
    bad.world.resources = {{Bank::PrimaryA,60U,583U,582U}}; // Valid first.
    bad.terrain = {{column,row,0x0400U,0U}};  // Stale second.
    rejects([&] { file.apply(bad); });
    bad = {};
    bad.terrain = {{column,row,0x040EU,0U}}; // Valid first.
    bad.cache = {{column,row,cache,cache}};  // Stale second.
    rejects([&] { file.apply(bad); });
    require(file.encode() == committed,
            "LEV025 rejected transaction partially modified state");

    rloft::Lev025Edits undo{};
    undo.world.resources = {{Bank::PrimaryA,60U,583U,582U}};
    undo.world.controls = {{Group::Primary,control_column,new_controls,controls}};
    undo.world.npcs = {{0U,changed_npc,npc}};
    undo.world.placed = {{0U,changed_placed,placed}};
    undo.world.loose = {{count,added,std::nullopt}};
    undo.world.notes = {{0U,annotation,std::nullopt}};
    undo.terrain = {{column,row,0x040EU,0U}};
    undo.cache = {{column,row,static_cast<std::uint8_t>(cache ^ 1U),cache}};
    undo.events = {{0U,changed_event,event}};
    file.apply(undo);
    require(file.encode() == original,
            "LEV025 inverse transaction did not restore each original byte");
    file.apply({});
    require(file.encode() == original,"LEV025 empty transaction changed source");
    rejects([&] { (void)file.event_slot(50U); });
    auto truncated = original;
    truncated.pop_back();
    rejects([&] { (void)rloft::Lev025File(truncated); });
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(),false);
    if (argc > 2) throw std::invalid_argument("usage: LEV025_tests [LEV025.DAT]");
    if (argc == 2) {
        std::ifstream stream(argv[1],std::ios::binary);
        if (!stream) throw std::runtime_error("unable to open original LEV025.DAT");
        const std::vector<std::uint8_t> source{
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
        exercise(source,true);
    }
}
