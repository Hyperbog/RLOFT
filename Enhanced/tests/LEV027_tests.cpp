#include "rloft/LEV027.hpp"

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
using Bank = rloft::Lev023ResourceBank;

void require(bool condition, const char* why) {
    if (!condition) throw std::runtime_error(why);
}
template<class Callable> void rejects(Callable operation) {
    bool rejected = false;
    try { operation(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "LEV027 accepted an invalid or stale operation");
}
void put16(std::vector<std::uint8_t>& out, std::size_t at, unsigned n) {
    out.at(at) = static_cast<std::uint8_t>(n);
    out.at(at + 1U) = static_cast<std::uint8_t>(n >> 8U);
}
void put32(std::vector<std::uint8_t>& out, std::size_t at, unsigned n) {
    for (unsigned i = 0U; i < 4U; ++i)
        out.at(at + i) = static_cast<std::uint8_t>(n >> (8U * i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr unsigned width = 4U, height = 3U;
    constexpr std::size_t header = 0x1DU, grid = header + 0x2A4U;
    constexpr std::size_t b1 = grid + 2U * width * height;
    constexpr std::size_t b2 = b1 + (2U*width + 20U) + 1U +
                              (2U*width + 10U);
    constexpr std::size_t b3 = b2 + 22U, b4 = b3 + 2U + 22U;
    constexpr std::size_t b5 = b4, b6 = b5 + 0x514U;
    constexpr std::array<std::size_t,7> bounds{{header,b1,b2,b3,b4,b5,b6}};
    std::vector<std::uint8_t> out(b6 + 4U + width*height + 4U, 0U);
    out[0] = 0xC3U; // Writer-unowned original marker survives all edits.
    for (std::size_t i = 0U; i < bounds.size(); ++i)
        put32(out, 1U + 4U*i, static_cast<unsigned>(bounds[i]));
    put16(out, header + 1U, width);
    put16(out, header + 3U, height);
    put16(out, header + 0x28AU, 1U); // One NPC.
    put16(out, header + 0x28EU, 1U); // One placed item.
    put16(out, header + 0x29AU, 1U); // Saved rendering threshold.
    out[header + 0x29CU] = 37U;
    put16(out, header + 0x02AU, 171U);
    put16(out, header + 0x030U, 170U);
    put16(out, header + 0x12CU, 171U);
    put16(out, header + 0x12EU, 170U);
    put16(out, grid, 0x0406U);

    std::size_t cursor = b1;
    for (unsigned col = 0U; col < width; ++col) {
        put16(out, cursor, col < 2U ? 1U : 0U); cursor += 2U;
        if (col == 0U || col == 1U) {
            out[cursor] = static_cast<std::uint8_t>(col == 0U ? 8U : 3U);
            out[cursor + 1U] = static_cast<std::uint8_t>(col == 1U ? 4U : 0U);
            out[cursor + 4U] = 2U;
            out[cursor + 5U] = static_cast<std::uint8_t>(col);
            cursor += 10U;
        }
    }
    out[cursor++] = 0U; // Animation byte between control groups.
    for (unsigned col = 0U; col < width; ++col) {
        put16(out, cursor, col == 1U ? 1U : 0U); cursor += 2U;
        if (col == 1U) {
            out[cursor] = 1U; out[cursor + 1U] = 5U;
            out[cursor + 4U] = 1U; out[cursor + 5U] = 1U;
            out[cursor + 6U] = 0U;
            cursor += 10U;
        }
    }
    require(cursor == b2, "synthetic LEV027 group boundaries incorrect");
    put32(out, b2, 1U << 16U); put32(out, b2 + 4U, 2U << 16U);
    put16(out, b2 + 12U, width + 2U); put16(out, b2 + 14U, 64U);
    put16(out, b3, 1U);
    put32(out, b3 + 2U, 1U << 16U); put32(out, b3 + 6U, 1U << 16U);
    put16(out, b3 + 14U, width + 1U); put16(out, b3 + 16U, 72U);
    out[b5] = 1U; out[b5 + 1U] = 12U; out[b5 + 5U] = 0x65U;
    put16(out, b6, width); put16(out, b6 + 2U, height);
    std::fill(out.begin() + static_cast<std::ptrdiff_t>(b6 + 4U),
              out.begin() + static_cast<std::ptrdiff_t>(b6 + 4U + width*height),
              0xFFU);
    put32(out, b6 + 4U + width*height, 0U);
    return out;
}

void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev027File level(original);
    const auto shape = level.summary();
    require(level.encode() == original && shape.group1_controls == 2U &&
            shape.group2_controls == 1U,
            "LEV027 input failed direct-save identity");
    if (retail) {
        require(original.size() == 2834U && shape.width == 12 &&
                shape.height == 12 && shape.world_counts ==
                   std::array<std::size_t,3>{{1U,13U,0U}} &&
                shape.active_event_slots == 1U &&
                shape.active_event_commands == 1U &&
                shape.annotation_bytes == 0U && shape.music_sequence == 37U &&
                shape.active_animation_count == 0U &&
                level.serialized_render_floor() == 1,
                "uploaded LEV027 format and counts differ");
        const auto world = level.decode();
        require(std::count_if(world.cell_words.begin(), world.cell_words.end(),
                    [](std::uint16_t w){return (w & 0x0400U) != 0U;}) == 55 &&
                std::count_if(world.cell_words.begin(), world.cell_words.end(),
                    [](std::uint16_t w){return (w & 0x0040U) != 0U;}) == 4,
                "uploaded LEV027 exploration flags differ");
    } else {
        require(shape.width == 4 && shape.height == 3 &&
                shape.world_counts == std::array<std::size_t,3>{{1U,1U,0U}},
                "synthetic LEV027 format and counts differ");
    }
    require(level.npc(0U).npc_type_id() == 64U &&
            level.placed_object(0U).object_type_id() == 72U &&
            level.event_slot(0U).active_count() == 1U &&
            level.event_slot(0U).raw[1U] == 12U &&
            level.resource_word(Bank::PrimaryA,16U) == 171U &&
            level.resource_word(Bank::PrimaryA,19U) == 170U &&
            level.resource_word(Bank::PrimaryB,17U) == 171U &&
            level.resource_word(Bank::PrimaryB,18U) == 170U,
            "LEV027 original NPC, item, script or resource bytes differ");

    const auto primary = level.controls(Group::Primary,1);
    const auto secondary = level.controls(Group::Secondary,1);
    require(primary.size() == 1U && secondary.size() == 1U &&
            primary[0U].raw[0] == 3U && primary[0U].raw[1] == 4U &&
            secondary[0U].raw[0] == 1U && secondary[0U].raw[6] == 0U &&
            secondary[0U].column() == 1U &&
            secondary[0U].row() == (retail ? 6U : 1U),
            "LEV027 primary/secondary group split is not authentic");
    const auto saved_control = secondary[0U];
    auto archive_control = saved_control;
    archive_control.raw[6] = 8U;
    const auto npc = level.npc(0U);
    auto changed_npc = npc;
    changed_npc.set_current_hp(static_cast<std::int16_t>(npc.current_hp() - 1));
    const auto placed = level.placed_object(0U);
    auto changed_placed = placed;
    changed_placed.set_packed_runtime_state(
        static_cast<std::uint16_t>(placed.packed_runtime_state() ^ 0x40U));
    const auto event = level.event_slot(0U);
    auto changed_event = event; changed_event.raw[5U] ^= 1U;
    const auto cache = level.automap_cell(0,0);

    rloft::Lev027Edits update{};
    update.state.level.world.resources = {{Bank::PrimaryA,16U,171U,170U}};
    update.state.level.world.npcs = {{0U,npc,changed_npc}};
    update.state.level.world.placed = {{0U,placed,changed_placed}};
    update.state.level.cache = {{0,0,cache,static_cast<std::uint8_t>(cache^1U)}};
    update.state.level.events = {{0U,event,changed_event}};
    update.transfer = rloft::Lev027ControlTransfer{
        Group::Secondary,Group::Primary,1,0U,1U,saved_control,archive_control};
    level.apply(update);
    require(level.controls(Group::Primary,1).size() == 2U &&
            level.controls(Group::Secondary,1).empty() &&
            level.controls(Group::Primary,1)[1U].raw == archive_control.raw &&
            level.resource_word(Bank::PrimaryA,16U) == 170U &&
            level.npc(0U).raw == changed_npc.raw &&
            level.placed_object(0U).raw == changed_placed.raw &&
            level.event_slot(0U).raw == changed_event.raw &&
            level.automap_cell(0,0) == (cache^1U) &&
            level.summary().boundaries == shape.boundaries &&
            level.encode().size() == original.size() &&
            level.bytes()[0U] == original[0U],
            "LEV027 simultaneous control migration and state edit failed");
    const auto checkpoint = level.encode();
    rejects([&]{level.apply(update);}); // Stale source and resource.
    auto bad = rloft::Lev027Edits{};
    bad.transfer = rloft::Lev027ControlTransfer{
        Group::Primary,Group::Primary,1,1U,0U,archive_control,saved_control};
    rejects([&]{level.apply(bad);});
    bad.transfer->destination = Group::Secondary;
    bad.transfer->replacement.raw[5] = 2U; // Cross-column move forbidden.
    rejects([&]{level.apply(bad);});
    bad.transfer->replacement.raw[5] = 1U;
    bad.transfer->expected.raw[6] = 7U; // Stale source instance.
    rejects([&]{level.apply(bad);});
    bad.transfer->expected.raw[6] = 8U;
    bad.transfer->destination_index = 99U;
    rejects([&]{level.apply(bad);});
    bad.transfer->destination_index = 0U;
    bad.state.level.world.resources = {{Bank::PrimaryA,16U,171U,170U}};
    rejects([&]{level.apply(bad);}); // Transfer valid but resource stale.
    bad = {};
    bad.state.level.world.npcs = {{0U,npc,changed_npc}};
    bad.transfer = rloft::Lev027ControlTransfer{
        Group::Primary,Group::Secondary,1,1U,0U,archive_control,saved_control};
    rejects([&]{level.apply(bad);}); // World edit stale; transfer must not commit.
    require(level.encode() == checkpoint, "LEV027 rejected edit partially committed");

    rloft::Lev027Edits undo{};
    undo.state.level.world.resources = {{Bank::PrimaryA,16U,170U,171U}};
    undo.state.level.world.npcs = {{0U,changed_npc,npc}};
    undo.state.level.world.placed = {{0U,changed_placed,placed}};
    undo.state.level.cache = {{0,0,static_cast<std::uint8_t>(cache^1U),cache}};
    undo.state.level.events = {{0U,changed_event,event}};
    undo.transfer = rloft::Lev027ControlTransfer{
        Group::Primary,Group::Secondary,1,1U,0U,archive_control,saved_control};
    level.apply(undo);
    require(level.encode() == original, "LEV027 inverse is not byte-exact");
    level.apply({});
    require(level.encode() == original, "LEV027 empty edit mutated original");
    rejects([&]{(void)level.controls(Group::Primary,shape.width);});
    auto truncated = original; truncated.pop_back();
    rejects([&]{(void)rloft::Lev027File(truncated);});
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(),false);
    if (argc > 2) throw std::invalid_argument("usage: LEV027_tests [LEV027.DAT]");
    if (argc == 2) {
        std::ifstream input(argv[1],std::ios::binary);
        if (!input) throw std::runtime_error("cannot open original LEV027.DAT");
        const std::vector<std::uint8_t> source{
            std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>()};
        exercise(source,true);
    }
}
