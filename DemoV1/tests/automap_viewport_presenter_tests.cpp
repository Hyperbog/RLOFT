#include "rloft/automap_viewport_presenter.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {

void put16(
    std::array<std::uint8_t, 0x2A4>& header,
    std::size_t offset,
    std::uint16_t value) {
    header[offset] =
        static_cast<std::uint8_t>(value);
    header[offset + 1U] =
        static_cast<std::uint8_t>(
            value >> 8U);
}

}  // namespace

int main() {
    rloft::ExplorationSessionState session{};
    session.coordinate_a = 0x00048000U;
    session.coordinate_b = 0x00048000U;
    session.level.width = 30;
    session.level.height = 15;
    session.level.cell_words.assign(450, 0);
    session.level.automap_cache.assign(
        450, 0xFF);

    const auto origin =
        rloft::AutomapViewportPresenter::
            initial_origin(session);
    assert(origin.column == -20);
    assert(origin.row == -20);

    constexpr std::uint8_t code = 3;
    const rloft::CellPosition cell{4, 4};
    const auto index =
        rloft::ExplorationWorldModel::cell_index(
            session.level, cell);
    session.level.cell_words[index] =
        static_cast<std::uint16_t>(
            0x0400U | code);
    session.level.automap_cache[index] =
        0x2EU;

    put16(
        session.level.header,
        0x20AU +
            static_cast<std::size_t>(code) *
                2U,
        0x3412U);

    const auto draw =
        rloft::AutomapViewportPresenter::
            tile_draw(
                session.level,
                cell,
                origin);

    // 0139C coordinates: column axis advances screen Y by 8; row axis
    // advances screen X by 5.
    assert(draw.screen_y ==
           6 + (4 - (-20)) * 8);
    assert(draw.screen_x ==
           4 + (4 - (-20)) * 5);
    assert(draw.frame == 0x2E);
    assert(draw.remap_b1 == 0x12);
    assert(draw.remap_b5 == 0x34);

    assert(
        rloft::AutomapViewportPresenter::
            tile_resource_id(15) == 67);

    // Exact 29CEC tie behavior: shallow diagonal advances the secondary
    // axis only when the signed error is nonnegative.
    rloft::IndexedFramebuffer framebuffer;
    framebuffer.clear(0);
    rloft::AutomapViewportPresenter::draw_line(
        framebuffer,
        10, 10,
        12, 14,
        7);
    const auto& pixels = framebuffer.pixels();
    auto at = [&](int y, int x) {
        return pixels[
            static_cast<std::size_t>(y) * 320U +
            static_cast<std::size_t>(x)];
    };
    assert(at(10,10) == 7);
    assert(at(11,11) == 7);
    assert(at(11,12) == 7);
    assert(at(12,13) == 7);
    assert(at(12,14) == 7);
}
