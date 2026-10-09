#include "rloft/view_sweep_traversal.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

void put16(
    std::array<std::uint8_t, 0x2A4>& header,
    std::size_t offset,
    std::uint16_t value) {
    header[offset] = static_cast<std::uint8_t>(value);
    header[offset + 1U] =
        static_cast<std::uint8_t>(value >> 8U);
}

rloft::RendererTables tables() {
    rloft::RendererTables t{};
    t.constants[0] = 1;       // one ray
    t.constants[5] = 0;       // no camera-origin displacement
    t.constants[7] = 0x1600;  // clamp-depth quotient = 0x100
    t.view_angle_tenths[0] = 0;
    t.tangent_q16.fill(0);
    t.sine_q16_signmag.fill(0);
    return t;
}

rloft::ExplorationSessionState session() {
    rloft::ExplorationSessionState s{};
    s.coordinate_a = 0x00028000U;
    s.coordinate_b = 0x00028000U;
    s.facing = 0;
    s.level.width = 6;
    s.level.height = 4;
    s.level.cell_words.assign(24, 0);
    return s;
}

}  // namespace

int main() {
    auto t = tables();
    auto s = session();

    // CODE.2 computes the cell index (3*6+2)=20. The SHL
    // before indexing a 16-bit word and later SHR cancel exactly.
    constexpr std::uint8_t code = 1;
    s.level.cell_words[20] =
        static_cast<std::uint16_t>(code | 0x0200U | 0x8000U);

    put16(s.level.header, 0x00A + code * 2U, 10);
    put16(s.level.header, 0x08A + code * 2U, 20);
    put16(s.level.header, 0x10A + code * 2U, 30);
    put16(s.level.header, 0x18A + code * 2U, 40);

    auto records =
        rloft::ViewSweepTraversalModel::build(s, t);
    assert(records.size() == 1);
    const auto& rec = records[0];
    assert(rec.cell_index == 20);
    assert(rec.resource_outer == 40);
    assert(rec.resource_inner == 30);
    assert(rec.prepass == 1);
    assert(rec.depth == 0x100);
    assert(rec.shade_decrement == 2);
    assert((s.level.cell_words[20] & 0x0440U) == 0x0440U);

    // The rebuild clears stale current-visibility while preserving explored.
    s.level.cell_words[0] = 0x0440U;
    records = rloft::ViewSweepTraversalModel::build(s, t);
    assert((s.level.cell_words[0] & 0x0040U) == 0);
    assert((s.level.cell_words[0] & 0x0400U) != 0);

    // True Seeing/FD08 treats hidden topology as traversable instead of a
    // terminating ordinary wall.
    s = session();
    s.level.cell_words[20] =
        static_cast<std::uint16_t>(code | 0x0800U);
    put16(s.level.header, 0x10A + code * 2U, 30);
    put16(s.level.header, 0x18A + code * 2U, 40);

    const auto hidden_records =
        rloft::ViewSweepTraversalModel::build(
            s, t, rloft::ViewSweepOptions{true});
    assert(hidden_records.size() == 1);
    assert(hidden_records[0].resource_outer == 0x93);
    assert(hidden_records[0].resource_inner == 0x93);
    assert((s.level.cell_words[20] & 0x0440U) == 0x0440U);

    // Asset-free reproduction of the near-cardinal failure after the
    // user's 16th Level 22 capture. Fixture numbers below are copied
    // from the immutable original SINTAB's measured ray tables.
    // The exact 30x15 perimeter is source-authenticated LEV022.DAT.
    s=session();
    s.level.width=30;
    s.level.height=15;
    s.level.cell_words.assign(450U,0U);
    for(int y=0;y<15;++y) {
        for(int x=0;x<30;++x) {
            if(x==0 || x==29 || y==0 || y==14) {
                s.level.cell_words[static_cast<std::size_t>(
                    y*30+x)]=1U;
            }
        }
    }
    for(const std::size_t at:{0x00CU,0x08CU,0x10CU,0x18CU})
        put16(s.level.header,at,0x0093U);
    s.coordinate_a=759417U;
    s.coordinate_b=468295U;
    s.facing=104U;

    t=tables();
    t.constants[5]=22272U;
    t.sine_q16_signmag[104U]=0x0000F865U;
    t.sine_q16_signmag[194U]=0x80003DEEU;
    t.tangent_q16[18U]=0x0000087EU;
    t.tangent_q16[881U]=0x001E2505U;
    t.view_angle_tenths[0]=-159;
    const auto near_cardinal=
        rloft::ViewSweepTraversalModel::build(s,t);
    assert(near_cardinal.size()==1U);
    assert(near_cardinal[0].cell_index==210U); // (0,7)
    assert(near_cardinal[0].resource_outer==0x0093U);
    assert(near_cardinal[0].resource_inner==0x0093U);
    assert((s.level.cell_words[210U]&0x0440U)==0x0440U);

    // The same original Level22 camera also exercises the other
    // CODE.2 sweep branch; it should reach (24,14) at facing 298.
    s.facing=298U;
    t.sine_q16_signmag[298U]=0x8000E208U;
    t.sine_q16_signmag[388U]=0x0000782FU;
    t.tangent_q16[280U]=0x000088B0U;
    t.tangent_q16[619U]=0x0001DF72U;
    t.view_angle_tenths[0]=0;
    const auto south_edge=
        rloft::ViewSweepTraversalModel::build(s,t);
    assert(south_edge.size()==1U);
    assert(south_edge[0].cell_index==444U); // (24,14)
    assert(south_edge[0].resource_inner==0x0093U);
    assert((s.level.cell_words[444U]&0x0440U)==0x0440U);
}
