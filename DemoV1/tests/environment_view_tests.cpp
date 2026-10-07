#include "rloft/environment_view.hpp"
#include "rloft/resource_assets.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] = static_cast<std::uint8_t>(v >> 8U);
}

void put16(std::array<std::uint8_t, 0x2A4>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] = static_cast<std::uint8_t>(v >> 8U);
}

}  // namespace

int main() {
    // Synthetic INIT containing one profile with one active placement and
    // the mandatory all-zero terminator.
    std::vector<std::uint8_t> init(0x0547U + 4U + 16U, 0);
    put16(init, 0x0545U, 1);
    std::size_t p = 0x0547U;
    put16(init, p + 0U, 8);
    put16(init, p + 2U, 2);
    p += 4U;
    put16(init, p + 0U, 24);
    put16(init, p + 2U, 355);
    put16(init, p + 4U, 128);
    put16(init, p + 6U, 4);
    p += 8U;
    // Final eight bytes remain zero.

    const auto profiles = rloft::decode_environment_profiles(init);
    assert(profiles.size() == 1);
    assert(profiles[0].res8_base == 8);
    assert(profiles[0].stored_record_count == 2);
    assert(profiles[0].placements.size() == 2);
    assert(profiles[0].placements[0].resource_selector == 24);
    assert(profiles[0].placements[0].azimuth_degrees == 355);
    assert(profiles[0].placements[0].draw_width_pixels == 128);
    assert(profiles[0].placements[0].screen_y == 4);
    assert(profiles[0].placements[1].terminator());
    assert(rloft::resolve_environment_res8_id(
               profiles[0], profiles[0].placements[0]) == 32);

    bool rejected = false;
    try {
        auto bad = init;
        bad.back() = 1;
        (void)rloft::decode_environment_profiles(bad);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);

    rloft::LevelState level{};
    level.width = 2;
    level.height = 2;
    level.cell_words.assign(4, 0);

    constexpr std::uint8_t code = 3;
    put16(level.header, 0x00A + code * 2U, 100);
    put16(level.header, 0x08A + code * 2U, 200);
    put16(level.header, 0x10A + code * 2U, 300);
    put16(level.header, 0x18A + code * 2U, 400);
    put16(level.header, 0x20A + code * 2U, 0xB5B1);

    level.cell_words[0] =
        static_cast<std::uint16_t>(
            code | 0x0200U | 0x0800U | 0x1000U | 0x8000U);

    rloft::CellControlRecord record{};
    record.raw[1] = 0x05;
    record.raw[4] = 0;
    record.raw[5] = 0;
    level.group1.push_back(record);

    const auto banks = rloft::EnvironmentViewModel::resource_banks(level);
    assert(banks.primary_a[code] == 100);
    assert(banks.alternate_a[code] == 200);
    assert(banks.primary_b[code] == 300);
    assert(banks.alternate_b[code] == 400);
    assert(banks.automap_style[code] == 0xB5B1);

    const auto visual =
        rloft::EnvironmentViewModel::cell_visual_inputs(level, {0, 0});
    assert(visual.cell_code == code);
    assert(visual.primary_a == 100);
    assert(visual.alternate_a == 200);
    assert(visual.primary_b == 300);
    assert(visual.alternate_b == 400);
    assert(visual.automap_style == 0xB5B1);
    assert(!visual.special_geometry_center);
    assert(!visual.special_geometry_frame);
    assert(visual.extra_render_prepass);
    assert(visual.hidden_passage_topology);
    assert(visual.record_state_variant_enabled);
    assert(visual.record_state_variant.has_value());
    assert(*visual.record_state_variant == 1);
    assert(visual.shade_decrement == 2);

    level.group1.clear();
    const auto no_record =
        rloft::EnvironmentViewModel::cell_visual_inputs(level, {0, 0});
    assert(no_record.record_state_variant_enabled);
    assert(!no_record.record_state_variant.has_value());
}
