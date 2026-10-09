#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_ground_texture_raster.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
std::uint8_t pixel(const rloft::IndexedFramebuffer& fb,int y,int x) {
    return fb.pixels().at(
        static_cast<std::size_t>(y)*320U+
        static_cast<std::size_t>(x));
}
template<class Callback>
bool rejected(Callback&& cb) {
    try { cb(); }
    catch(const std::exception&) {return true;}
    return false;
}
}

int main() {
    using namespace rloft;
    constexpr std::size_t kCode2Base=0x75F24U;
    constexpr std::size_t kShadeAt=0xFE15U;
    std::vector<std::uint8_t> code2(
        kCode2Base+kShadeAt+256U,0U);
    for(std::size_t i=0U;i<256U;++i) {
        code2[kCode2Base+kShadeAt+i]=
            static_cast<std::uint8_t>(i);
    }
    const auto shades=GroundTextureShadeTable::from_code2(code2);
    assert(shades.subtract[0]==0U);
    assert(shades.subtract[127]==127U);
    assert(shades.subtract[254]==254U);
    assert(shades.subtract[255]==255U);

    // Exact original 0x300-byte FF58 row layout.
    std::vector<std::uint8_t> ff58(128U*6U,0U);
    for(std::size_t row=0U;row<128U;++row) {
        ff58[row*6U+0U]=0x00U;
        ff58[row*6U+1U]=0x04U; // +0x0400 <<16 => next texel/px
        ff58[row*6U+2U]=0U;
        ff58[row*6U+3U]=0U;
        ff58[row*6U+4U]=0U;    // origin U0
        ff58[row*6U+5U]=2U;    // origin V2
    }
    const auto records=OriginalGroundTextureRaster::decode_ff58(ff58);
    assert(records[0].delta_u==0x400U);
    assert(records[127].origin_v==2U);
    assert(rejected([&] {
        (void)OriginalGroundTextureRaster::decode_ff58(
            std::vector<std::uint8_t>(127U*6U,0U));
    }));

    // Original 2F29C returns a resource with 2 header bytes followed
    // by a raw 64x64 indexed tile. The tile deliberately distinguishes
    // successive U samples and wraps every 64.
    std::vector<std::uint8_t> source(
        GroundTextureRasterSpec::kTextureBytes+
        GroundTextureRasterSpec::kTextureHeaderBytes,0U);
    source[0]=0xA5U;
    source[1]=0x5AU;
    for(std::size_t v=0U;v<64U;++v) {
        for(std::size_t u=0U;u<64U;++u) {
            source[2U+v*64U+u]=
                static_cast<std::uint8_t>(v*64U+u);
        }
    }
    IndexedFramebuffer fb;
    fb.clear(0xEE);
    GroundTextureRasterSpec spec{};
    const auto lower=OriginalGroundTextureRaster::draw(
        fb,source,shades,records,spec);
    assert(lower.rows==128U);
    assert(lower.individual_plane_rows==80U);
    assert(lower.paired_plane_rows==48U);
    assert(lower.source_reads==80U*256U+48U*128U);
    assert(lower.indexed_writes==128U*256U);

    // Lower 80 scanlines: four physical plane masks, four unique
    // texture samples per VGA byte. This spans screen y259..180.
    assert(pixel(fb,259,32)==128U);
    assert(pixel(fb,259,33)==129U);
    assert(pixel(fb,259,34)==130U);
    assert(pixel(fb,259,35)==131U);
    assert(pixel(fb,259,96)==128U); // original modulo-64 tile wrap
    assert(pixel(fb,180,32)==
           static_cast<std::uint8_t>(128U-79U));
    assert(pixel(fb,180,33)==
           static_cast<std::uint8_t>(129U-79U));

    // Remaining 48: mask 0x0C samples U2 for lanes 2+3 and mask
    // 0x03 samples U0 for lanes 0+1 (2-pixel texture grouping).
    assert(pixel(fb,179,32)==
           static_cast<std::uint8_t>(128U-80U));
    assert(pixel(fb,179,33)==
           static_cast<std::uint8_t>(128U-80U));
    assert(pixel(fb,179,34)==
           static_cast<std::uint8_t>(130U-80U));
    assert(pixel(fb,179,35)==
           static_cast<std::uint8_t>(130U-80U));
    assert(pixel(fb,132,32)==1U);
    assert(pixel(fb,132,33)==1U);
    assert(pixel(fb,131,32)==0xEEU);
    assert(pixel(fb,260,32)==0xEEU);
    assert(pixel(fb,259,31)==0xEEU);
    assert(pixel(fb,259,288)==0xEEU);

    // Original upper textured strip at 1F343/1F352 uses +80 and
    // +1 shade index steps rather than reversed gradient indexing.
    fb.clear(0xE9U);
    spec.first_planar_byte_offset=0x148U; // screen x32,y4
    spec.row_step_bytes=80;
    spec.shade_index_step=1;
    const auto upper=OriginalGroundTextureRaster::draw(
        fb,source,shades,records,spec);
    assert(upper.indexed_writes==128U*256U);
    assert(pixel(fb,4,32)==
           static_cast<std::uint8_t>(128U-254U));
    assert(pixel(fb,4,33)==
           static_cast<std::uint8_t>(129U-254U));
    assert(pixel(fb,84,32)==
           static_cast<std::uint8_t>(128U-174U));
    assert(pixel(fb,84,33)==
           static_cast<std::uint8_t>(128U-174U));
    assert(pixel(fb,131,32)==1U);
    assert(pixel(fb,3,32)==0xE9U);
    assert(pixel(fb,132,32)==0xE9U);

    // Runtime C048/C044 bytes patch origin AL/DL, preserving wrap.
    spec.row_count=1U;
    spec.phase_u=1U;
    spec.phase_v=1U;
    fb.clear(0xDDU);
    const auto phase=OriginalGroundTextureRaster::draw(
        fb,source,shades,records,spec);
    assert(phase.source_reads==256U);
    // y=3, x=1 => src(3*64+1)=193; shade254.
    assert(pixel(fb,4,32)==
           static_cast<std::uint8_t>(193U-254U));
    assert(pixel(fb,5,32)==0xDDU);

    // Reject malformed inputs transactionally: no intermediate writes.
    const auto before=fb.pixels();
    assert(rejected([&] {
        auto short_tile=source;
        short_tile.pop_back();
        (void)OriginalGroundTextureRaster::draw(
            fb,short_tile,shades,records,spec);
    }));
    assert(fb.pixels()==before);
    assert(rejected([&] {
        auto bad=spec;
        bad.first_planar_byte_offset=0x50F0U; // wrong x origin
        (void)OriginalGroundTextureRaster::draw(
            fb,source,shades,records,bad);
    }));
    assert(fb.pixels()==before);
    assert(rejected([&] {
        auto bad=spec;
        bad.row_count=129U;
        (void)OriginalGroundTextureRaster::draw(
            fb,source,shades,records,bad);
    }));
    assert(fb.pixels()==before);
    assert(rejected([&] {
        (void)GroundTextureShadeTable::from_code2(
            std::vector<std::uint8_t>(32U,0U));
    }));
}
