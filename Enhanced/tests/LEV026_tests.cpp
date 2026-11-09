#include "rloft/LEV026.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
using Group = rloft::Lev019ControlGroup;

void require(bool okay, const char* why) {
    if (!okay) throw std::runtime_error(why);
}
template<class Operation> void rejects(Operation operation) {
    bool failed = false;
    try { operation(); } catch (const std::exception&) { failed = true; }
    require(failed, "invalid LEV026 operation was accepted");
}
void write16(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    b.at(at) = static_cast<std::uint8_t>(value);
    b.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
void write32(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    for (unsigned i = 0U; i < 4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(value >> (8U * i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t header = 0x1DU, grid = header + 0x2A4U;
    constexpr unsigned width = 4U, height = 3U;
    constexpr std::size_t b1 = grid + 2U * width * height;
    constexpr std::size_t b2 = b1 + 2U * width + 3U * 10U + 1U + 2U * width;
    constexpr std::size_t b3 = b2 + 22U, b4 = b3 + 2U + 22U;
    constexpr std::size_t b5 = b4 + 22U, b6 = b5 + 0x514U;
    constexpr std::array<std::size_t, 7> bounds{{header,b1,b2,b3,b4,b5,b6}};
    std::vector<std::uint8_t> b(b6 + 4U + width*height + 4U, 0U);
    b[0] = 0xD2U;
    for (std::size_t i = 0; i < bounds.size(); ++i)
        write32(b, 1U + i * 4U, static_cast<unsigned>(bounds[i]));
    write16(b, header + 1U, width); write16(b, header + 3U, height);
    write16(b, header + 0x28AU, 1U); // One NPC.
    write16(b, header + 0x28CU, 1U); // One loose item.
    write16(b, header + 0x28EU, 1U); // One placed object.
    write16(b, header + 0x29AU, 1U); // Serialized renderer floor.
    b[header + 0x29CU] = 2U;
    write16(b, grid + 2U * (width + 1U), 0x0401U);
    std::size_t at = b1;
    for (unsigned col = 0U; col < width; ++col) {
        write16(b, at, col == 2U ? 3U : 0U); at += 2U;
        if (col == 2U) {
            for (unsigned row = 0U; row < 3U; ++row) {
                b[at] = 3U; b[at + 1U] = 0U;
                b[at + 4U] = static_cast<std::uint8_t>(row);
                b[at + 5U] = 2U;
                at += 10U;
            }
        }
    }
    b[at++] = 0U; // Animation state.
    for (unsigned col = 0U; col < width; ++col) {
        write16(b, at, 0U); at += 2U;
    }
    require(at == b2, "synthetic LEV026 control geometry invalid");
    write32(b, b2, 1U<<16U); write32(b, b2+4U, 1U<<16U);
    write16(b, b2+12U, width+1U); write16(b, b2+14U, 6U);
    write16(b, b2+18U, 4U);
    write16(b, b3, 1U);
    write32(b, b3+2U, 1U<<16U); write32(b, b3+6U, 2U<<16U);
    write16(b, b3+14U, width+2U); write16(b, b3+16U, 113U);
    write32(b, b4, 1U<<16U); write32(b, b4+4U, 3U<<16U);
    write16(b, b4+12U, width+3U); write16(b, b4+14U, 164U);
    b[b5] = 1U; b[b5+1U] = 1U; b[b5+2U] = 4U;
    write16(b, b6, width); write16(b, b6+2U, height);
    std::fill(b.begin() + static_cast<std::ptrdiff_t>(b6+4U),
              b.begin() + static_cast<std::ptrdiff_t>(b6+4U+width*height), 0xFFU);
    write32(b, b6+4U+width*height, 0U);
    return b;
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev026File level(original);
    const auto shape = level.summary();
    require(level.encode() == original &&
            level.serialized_render_floor() == 1 &&
            level.effective_render_floor() == 1,
            "LEV026 original source or render threshold mismatch");
    if (retail) {
        require(original.size() == 20438U && shape.width == 128 &&
                shape.height == 32 && shape.world_counts ==
                    std::array<std::size_t,3>{{47U,183U,11U}} &&
                shape.group1_controls == 32U && shape.group2_controls == 0U &&
                shape.active_event_slots == 2U &&
                shape.active_event_commands == 5U &&
                shape.annotation_bytes == 0U && shape.music_sequence == 2U,
                "uploaded LEV026 structure differs");
        require(level.npc(0U).npc_type_id() == 6U &&
                level.npc(0U).current_hp() == 4 &&
                level.placed_object(0U).object_type_id() == 113U &&
                level.loose_item(0U).object_type_id() == 164U &&
                level.event_slot(0U).active_count() == 1U &&
                level.event_slot(1U).active_count() == 4U,
                "uploaded LEV026 actor, item or event source differs");
        const auto world = level.decode();
        const auto explored = std::count_if(world.cell_words.begin(),
            world.cell_words.end(), [](std::uint16_t w) {return (w&0x0400U)!=0U;});
        const auto visible = std::count_if(world.cell_words.begin(),
            world.cell_words.end(), [](std::uint16_t w) {return (w&0x0040U)!=0U;});
        require(explored == 2006 && visible == 67,
                "uploaded LEV026 explored/visible bits differ");
    }

    const int column = retail ? 62 : 2;
    const auto controls = level.controls(Group::Primary, column);
    require(controls.size() == 3U, "expected three original kind-3 controls");
    for (std::size_t i = 0U; i < controls.size(); ++i) {
        require(controls[i].raw[0] == 3U && controls[i].raw[1] == 0U &&
                controls[i].raw[4] == i + (retail ? 17U : 0U) &&
                controls[i].raw[5] == column,
                "LEV026 original kind-3 control bytes differ");
    }
    auto revised_controls = controls;
    for (auto& record : revised_controls) record.raw[1] = 4U;
    const auto npc = level.npc(0U);
    auto changed_npc = npc;
    changed_npc.set_current_hp(static_cast<std::int16_t>(npc.current_hp() - 1));
    const auto event = level.event_slot(0U);
    auto changed_event = event;
    changed_event.raw[2] ^= 1U; // Keep active opcode and unused commands.
    const auto cell = level.cell_word(1,1);
    const auto next_code = static_cast<std::uint8_t>((cell & 0x3FU) ^ 1U);
    const auto target_cell = static_cast<std::uint16_t>((cell & 0xFFC0U) | next_code);
    const auto cache = level.automap_cell(1,1);

    rloft::Lev026Edits edits{};
    edits.render_floor = rloft::Lev026RenderFloorPatch{1,0};
    edits.level.world.controls = {{Group::Primary,column,controls,revised_controls}};
    edits.level.world.npcs = {{0U,npc,changed_npc}};
    edits.level.terrain = {{1,1,cell,next_code}};
    edits.level.cache = {{1,1,cache,static_cast<std::uint8_t>(cache^1U)}};
    edits.level.events = {{0U,event,changed_event}};
    level.apply(edits);
    require(level.serialized_render_floor() == 0 &&
            level.effective_render_floor() == 1 &&
            level.cell_word(1,1) == target_cell &&
            level.automap_cell(1,1) == (cache^1U) &&
            level.event_slot(0U).raw == changed_event.raw &&
            level.npc(0U).raw == changed_npc.raw &&
            level.controls(Group::Primary,column)[1].raw == revised_controls[1].raw &&
            level.bytes()[0] == original[0] &&
            level.encode().size() == original.size() &&
            level.summary().boundaries == shape.boundaries,
            "LEV026 multi-domain checkpoint failed");
    const auto checkpoint = level.encode();
    rejects([&]{level.apply(edits);}); // Original expected floor now stale.
    auto invalid = rloft::Lev026Edits{};
    invalid.render_floor = rloft::Lev026RenderFloorPatch{0,-2};
    invalid.level.world.controls = {{Group::Primary,column,controls,revised_controls}};
    rejects([&]{level.apply(invalid);}); // Valid threshold, stale controls.
    invalid = {};
    invalid.render_floor = rloft::Lev026RenderFloorPatch{0,-2};
    invalid.level.terrain = {{1,1,cell,0}}; // Stale map word.
    rejects([&]{level.apply(invalid);});
    invalid = {};
    invalid.render_floor = rloft::Lev026RenderFloorPatch{0,-2};
    invalid.level.events = {{0U,changed_event,event},{0U,changed_event,event}};
    rejects([&]{level.apply(invalid);});
    require(level.encode() == checkpoint, "failed LEV026 patch mutated source");

    level.apply({rloft::Lev026RenderFloorPatch{0,-2},{}});
    require(level.serialized_render_floor() == -2 &&
            level.effective_render_floor() == -2,
            "signed renderer threshold was not preserved");
    level.apply({rloft::Lev026RenderFloorPatch{-2,0},{}});
    require(level.encode() == checkpoint, "negative renderer edit did not undo");

    rloft::Lev026Edits undo{};
    undo.render_floor = rloft::Lev026RenderFloorPatch{0,1};
    undo.level.world.controls = {{Group::Primary,column,revised_controls,controls}};
    undo.level.world.npcs = {{0U,changed_npc,npc}};
    undo.level.terrain = {{1,1,target_cell,static_cast<std::uint8_t>(cell&0x3FU)}};
    undo.level.cache = {{1,1,static_cast<std::uint8_t>(cache^1U),cache}};
    undo.level.events = {{0U,changed_event,event}};
    level.apply(undo);
    require(level.encode() == original, "LEV026 inverse failed byte-exact restore");
    level.apply({});
    require(level.encode() == original, "empty LEV026 edit mutated input");
    rejects([&] { (void)level.controls(Group::Primary, shape.width); });
    auto broken = original; broken.pop_back();
    rejects([&] { (void)rloft::Lev026File(broken); });
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(), false);
    if (argc > 2) throw std::invalid_argument("usage: LEV026_tests [LEV026.DAT]");
    if (argc == 2) {
        std::ifstream input(argv[1], std::ios::binary);
        if (!input) throw std::runtime_error("cannot open supplied LEV026.DAT");
        const std::vector<std::uint8_t> source{
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>()};
        exercise(source, true);
    }
}
