#include "rloft/LEV031.hpp"

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

void require(bool value, const char* why) {
    if (!value) throw std::runtime_error(why);
}
template<class Action> void rejects(Action action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "LEV031 invalid edit was not rejected");
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
    require(cursor == b2, "synthetic LEV031 group boundaries incorrect");
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
    // Four independently serialized three-entry resource mapping cycles.
    constexpr std::array<std::size_t,4> banks{{0x00AU,0x08AU,0x10AU,0x18AU}};
    for (std::size_t bank = 0U; bank < banks.size(); ++bank) {
        const unsigned first = bank % 2U == 0U ? 581U : 587U;
        put16(out, header + banks[bank] + 2U*60U, first + 2U);
        put16(out, header + banks[bank] + 2U*61U, first);
        put16(out, header + banks[bank] + 2U*62U, first + 1U);
    }
    return out;
}

void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    rloft::Lev031File file(original);
    const auto shape = file.summary();
    require(file.encode() == original &&
            shape.group2_controls == 1U && shape.annotation_bytes == 0U,
            "LEV031 original binary changed during parse/encode");
    const int col = retail ? 2 : 1;
    if (retail) {
        require(original.size() == 14322U && shape.width == 64 &&
                shape.height == 32 && shape.group1_controls == 11U &&
                shape.world_counts == std::array<std::size_t,3>{{54U,197U,12U}} &&
                shape.active_event_slots == 2U &&
                shape.active_event_commands == 2U &&
                shape.music_sequence == 38U &&
                shape.active_animation_count == 0U &&
                file.npc(0U).npc_type_id() == 5U &&
                file.placed_object(0U).object_type_id() == 113U &&
                file.loose_item(0U).object_type_id() == 164U &&
                file.event_slot(0U).active_count() == 1U &&
                file.event_slot(1U).active_count() == 1U,
                "uploaded LEV031 original identity/structure changed");
        const auto decoded = file.decode();
        require(std::count_if(decoded.cell_words.begin(), decoded.cell_words.end(),
                    [](std::uint16_t v){return (v & 0x0400U) != 0U;}) == 182 &&
                std::count_if(decoded.cell_words.begin(), decoded.cell_words.end(),
                    [](std::uint16_t v){return (v & 0x0040U) != 0U;}) == 5,
                "uploaded LEV031 exploration flags changed");
    } else {
        require(shape.width == 4 && shape.height == 3 &&
                shape.group1_controls == 2U &&
                shape.world_counts == std::array<std::size_t,3>{{1U,1U,0U}},
                "synthetic LEV031 record geometry invalid");
    }
    constexpr std::array<std::uint8_t,3> codes{{60U,61U,62U}};
    const std::array<Bank,4> banks{{Bank::PrimaryA,Bank::AlternateA,
                                   Bank::PrimaryB,Bank::AlternateB}};
    rloft::Lev031Edits apply{};
    for (std::size_t i=0U; i<banks.size(); ++i) {
        const std::uint16_t begin = i%2U == 0U ? 581U : 587U;
        const std::array<std::uint16_t,3> saved{{
            static_cast<std::uint16_t>(begin+2U),begin,
            static_cast<std::uint16_t>(begin+1U)}};
        require(file.resource_triplet(banks[i],codes) == saved,
                "original resource-bank rotation does not match source");
        apply.resource_cycles.push_back({banks[i],codes,saved,false});
    }
    const auto primary = file.controls(Group::Primary,col);
    const auto secondary = file.controls(Group::Secondary,col);
    require(primary.size()==1U && secondary.size()==1U &&
            secondary[0U].raw[0] == 1U &&
            secondary[0U].row() == (retail ? 15U : 1U) &&
            secondary[0U].column() == col &&
            secondary[0U].raw[6U] == (retail ? 8U : 0U),
            "original Level 31 source control transfer not found");
    const auto before_npc = file.npc(0U);
    auto revised_npc = before_npc;
    revised_npc.set_current_hp(static_cast<std::int16_t>(before_npc.current_hp()-1));
    const auto before_placed = file.placed_object(0U);
    auto revised_placed = before_placed;
    revised_placed.set_packed_runtime_state(static_cast<std::uint16_t>(
        before_placed.packed_runtime_state() ^ 0x0008U));
    const auto old_cache = file.automap_cell(0,0);
    const auto old_event = file.event_slot(0U);
    auto revised_event = old_event; revised_event.raw[5U] ^= 1U;
    apply.state.state.level.world.npcs = {{0U,before_npc,revised_npc}};
    apply.state.state.level.world.placed = {{0U,before_placed,revised_placed}};
    apply.state.state.level.cache = {{0,0,old_cache,
                                      static_cast<std::uint8_t>(old_cache^1U)}};
    apply.state.state.level.events = {{0U,old_event,revised_event}};
    apply.state.transfer = rloft::Lev027ControlTransfer{
        Group::Secondary,Group::Primary,col,0U,primary.size(),
        secondary[0U],secondary[0U]};
    file.apply(apply);
    for (std::size_t i=0U; i<banks.size(); ++i) {
        const std::uint16_t begin = i%2U == 0U ? 581U : 587U;
        const std::array<std::uint16_t,3> normalized{{begin,
            static_cast<std::uint16_t>(begin+1U),
            static_cast<std::uint16_t>(begin+2U)}};
        require(file.resource_triplet(banks[i],codes) == normalized,
                "LEV031 resource rotation did not produce archived order");
    }
    require(file.controls(Group::Primary,col).size() == 2U &&
            file.controls(Group::Secondary,col).empty() &&
            file.npc(0U).raw == revised_npc.raw &&
            file.placed_object(0U).raw == revised_placed.raw &&
            file.event_slot(0U).raw == revised_event.raw &&
            file.automap_cell(0,0) == (old_cache^1U) &&
            file.encode().size() == original.size() &&
            file.summary().boundaries == shape.boundaries &&
            file.bytes()[0] == original[0],
            "combined LEV031 resource/control/world edit failed");
    const auto checkpoint = file.encode();
    rejects([&]{file.apply(apply);}); // Expected resource values now stale.
    rloft::Lev031Edits bad{};
    bad.resource_cycles={{Bank::PrimaryA,{{60U,60U,62U}},{{581U,582U,583U}},true}};
    rejects([&]{file.apply(bad);}); // Duplicate code within a cycle.
    bad.resource_cycles={{Bank::PrimaryA,{{60U,61U,64U}},{{581U,582U,583U}},true}};
    rejects([&]{file.apply(bad);}); // Invalid six-bit cell code.
    bad.resource_cycles={{static_cast<Bank>(5),codes,{{581U,582U,583U}},true}};
    rejects([&]{file.apply(bad);}); // Invalid resource bank.
    bad.resource_cycles={{Bank::PrimaryA,codes,{{581U,582U,582U}},true}};
    rejects([&]{file.apply(bad);}); // Stale resource word.
    bad.resource_cycles={{Bank::PrimaryA,codes,{{581U,582U,583U}},true},
                         {Bank::PrimaryA,codes,{{581U,582U,583U}},true}};
    rejects([&]{file.apply(bad);}); // Entry shared between two cycles.
    bad.resource_cycles={{Bank::PrimaryA,codes,{{581U,582U,583U}},true}};
    bad.state.state.level.world.resources = {{Bank::PrimaryA,60U,581U,582U}};
    rejects([&]{file.apply(bad);}); // Collision with ordinary resource edit.
    bad.state.state.level.world.resources.clear();
    bad.state.state.level.world.npcs = {{0U,before_npc,revised_npc}};
    rejects([&]{file.apply(bad);}); // Valid resource cycle, stale NPC.
    require(file.encode() == checkpoint,"rejected LEV031 edit mutated source");

    rloft::Lev031Edits undo{};
    for (std::size_t i=0U; i<banks.size(); ++i) {
        const std::uint16_t begin = i%2U == 0U ? 581U : 587U;
        undo.resource_cycles.push_back({banks[i],codes,{{begin,
            static_cast<std::uint16_t>(begin+1U),
            static_cast<std::uint16_t>(begin+2U)}},true});
    }
    undo.state.state.level.world.npcs = {{0U,revised_npc,before_npc}};
    undo.state.state.level.world.placed = {{0U,revised_placed,before_placed}};
    undo.state.state.level.cache = {{0,0,
        static_cast<std::uint8_t>(old_cache^1U),old_cache}};
    undo.state.state.level.events = {{0U,revised_event,old_event}};
    undo.state.transfer = rloft::Lev027ControlTransfer{
        Group::Primary,Group::Secondary,col,primary.size(),0U,
        secondary[0U],secondary[0U]};
    file.apply(undo);
    require(file.encode()==original,"LEV031 inverse did not restore all source bytes");
    file.apply({});
    require(file.encode()==original,"empty LEV031 edit mutated original");
    auto truncated=original;truncated.pop_back();
    rejects([&]{(void)rloft::Lev031File(truncated);});
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(),false);
    if (argc>2) throw std::invalid_argument("usage: LEV031_tests [LEV031.DAT]");
    if (argc==2) {
        std::ifstream in(argv[1],std::ios::binary);
        if (!in) throw std::runtime_error("cannot open supplied LEV031.DAT");
        const std::vector<std::uint8_t> source{
            std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
        exercise(source,true);
    }
}
