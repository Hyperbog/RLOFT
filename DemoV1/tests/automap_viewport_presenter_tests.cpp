#include "rloft/automap_viewport_presenter.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

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

    // Native map field is 48*8 by 47*5 inside the 320x400
    // framebuffer. The player facing line may approach the boundaries,
    // but MUST NOT paint over controls at x=239..319 or the border.
    framebuffer.clear(0x33U);
    rloft::AutomapViewportPresenter::draw_line(
        framebuffer,385,234,396,245,0xF0U,true);
    assert(at(385,234)==0xF0U);
    assert(at(389,238)==0xF0U);
    assert(at(390,239)==0x33U);
    assert(at(396,245)==0x33U);
    framebuffer.clear(0x33U);
    rloft::AutomapViewportPresenter::draw_line(
        framebuffer,385,234,396,245,0xF0U,false);
    assert(at(396,245)==0xF0U);
    assert(rloft::AutomapViewportPresenter::kMapMinY==6);
    assert(rloft::AutomapViewportPresenter::kMapMaxY==389);
    assert(rloft::AutomapViewportPresenter::kMapMinX==4);
    assert(rloft::AutomapViewportPresenter::kMapMaxX==238);

    // The arrow may terminate outside the map field; its visible
    // segment must survive instead of making the entire arrow vanish.
    framebuffer.clear(0x55U);
    rloft::AutomapViewportPresenter::draw_line(
        framebuffer,12,6,-10,6,0xF0U,true);
    assert(at(6,6)==0xF0U);
    assert(at(12,6)==0xF0U);
    assert(at(5,6)==0x55U);
    framebuffer.clear(0x55U);
    rloft::AutomapViewportPresenter::draw_line(
        framebuffer,8,8,8,-5,0xF0U,true);
    assert(at(8,4)==0xF0U);
    assert(at(8,8)==0xF0U);
    assert(at(8,3)==0x55U);

    // Native automap terrain tiles use the SAME clipping bounds as
    // the arrow; this must preserve chrome immediately outside map.
    rloft::Type1Raster tile{};
    tile.count=1;
    tile.width=3;
    tile.height=2;
    tile.transparent_index=254U;
    tile.pixels={0xB1U,0xB5U,254U,4U,5U,6U};
    std::array<std::uint8_t,256> map{};
    for(std::size_t i=0;i<map.size();++i)map[i]=
        static_cast<std::uint8_t>(i);
    map[0xB1]=0x21U;
    map[0xB5]=0x22U;
    framebuffer.clear(0x33U);
    auto drawn=rloft::AutomapViewportPresenter::draw_tile_clipped(
        framebuffer,tile,0,389,238,map);
    assert(drawn==1U);
    assert(at(389,238)==0x21U);
    assert(at(389,239)==0x33U);
    assert(at(390,238)==0x33U);

    framebuffer.clear(0x33U);
    drawn=rloft::AutomapViewportPresenter::draw_tile_clipped(
        framebuffer,tile,0,388,237,map);
    assert(drawn==4U);
    assert(at(388,237)==0x21U);
    assert(at(388,238)==0x22U);
    assert(at(389,237)==4U);
    assert(at(389,238)==5U);
    assert(at(388,239)==0x33U);
    assert(at(390,238)==0x33U);

    // Corrupt inputs must not partially overwrite the automap.
    tile.pixels.pop_back();
    bool rejected=false;
    try{
        (void)rloft::AutomapViewportPresenter::draw_tile_clipped(
            framebuffer,tile,0,388,237,map);
    }catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
    assert(at(388,237)==0x21U);
}
