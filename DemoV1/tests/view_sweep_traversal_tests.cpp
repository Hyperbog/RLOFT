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

    // With zero slopes at (2.5,2.5), CODE.2's first left-edge flattened
    // expression is (3*6 + 2) >> 1 == 10.
    constexpr std::uint8_t code = 1;
    s.level.cell_words[10] =
        static_cast<std::uint16_t>(code | 0x0200U | 0x8000U);

    put16(s.level.header, 0x00A + code * 2U, 10);
    put16(s.level.header, 0x08A + code * 2U, 20);
    put16(s.level.header, 0x10A + code * 2U, 30);
    put16(s.level.header, 0x18A + code * 2U, 40);

    auto records =
        rloft::ViewSweepTraversalModel::build(s, t);
    assert(records.size() == 1);
    const auto& rec = records[0];
    assert(rec.cell_index == 10);
    assert(rec.resource_outer == 40);
    assert(rec.resource_inner == 30);
    assert(rec.prepass == 1);
    assert(rec.depth == 0x100);
    assert(rec.shade_decrement == 2);
    assert((s.level.cell_words[10] & 0x0440U) == 0x0440U);

    // The rebuild clears stale current-visibility while preserving explored.
    s.level.cell_words[0] = 0x0440U;
    records = rloft::ViewSweepTraversalModel::build(s, t);
    assert((s.level.cell_words[0] & 0x0040U) == 0);
    assert((s.level.cell_words[0] & 0x0400U) != 0);

    // True Seeing/FD08 treats hidden topology as traversable instead of a
    // terminating ordinary wall.
    s = session();
    s.level.cell_words[10] =
        static_cast<std::uint16_t>(code | 0x0800U);
    put16(s.level.header, 0x10A + code * 2U, 30);
    put16(s.level.header, 0x18A + code * 2U, 40);

    const auto hidden_records =
        rloft::ViewSweepTraversalModel::build(
            s, t, rloft::ViewSweepOptions{true});
    assert(hidden_records.size() == 1);
    assert(hidden_records[0].resource_outer == 0x93);
    assert(hidden_records[0].resource_inner == 0x93);
    assert((s.level.cell_words[10] & 0x0440U) == 0x0440U);
}
