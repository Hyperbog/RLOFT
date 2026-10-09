#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_ground_res4_audit.hpp"
#include "rloft/original_ground_texture_raster.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void le16(std::vector<std::uint8_t>& b,std::size_t p,
          std::uint16_t v) {
    b.at(p)=static_cast<std::uint8_t>(v);
    b.at(p+1)=static_cast<std::uint8_t>(v>>8U);
}
void le32(std::vector<std::uint8_t>& b,std::size_t p,
          std::uint32_t v) {
    le16(b,p,static_cast<std::uint16_t>(v));
    le16(b,p+2,static_cast<std::uint16_t>(v>>16U));
}
std::vector<std::uint8_t> original_type1_64_tile() {
    // Authentic EH-1 descriptor framing. 0x08-byte descriptor:
    // count 1, height 64, width_units 8, key FF, compression 0,
    // transform 0. 4096 palette-index texels follow.
    std::vector<std::uint8_t> r{
        'E','H',1U,8U,0U,
        1U,0U,64U,0U,8U,0xFFU,0U,0U
    };
    for(std::size_t i=0;i<4096U;++i)
        r.push_back(static_cast<std::uint8_t>(i&255U));
    return r;
}
rloft::ResArchive fixture() {
    std::vector<std::uint8_t> bytes(4U+4U*4U,0U);
    le32(bytes,0U,4U);
    const auto append=[&](int index,std::vector<std::uint8_t> part) {
        le32(bytes,4U+static_cast<std::size_t>(index)*4U,
             static_cast<std::uint32_t>(bytes.size()));
        bytes.insert(bytes.end(),part.begin(),part.end());
    };
    append(0,{'E','H',7U,0U,0U,0U,0U});
    auto valid=original_type1_64_tile();
    append(1,valid);
    // Type-1 but not a ground shape: one 8x1 raw indexed strip.
    append(2,{'E','H',1U,8U,0U,1U,0U,1U,0U,1U,0xFFU,
              0U,0U,10U,11U,12U,13U,14U,15U,16U,17U});
    auto invalid=valid;
    invalid.pop_back(); // malformed payload, must not be a candidate
    append(3,invalid);
    return rloft::ResArchive(std::move(bytes));
}
template<class Fn> bool throws(Fn&& fn) {
    try {fn();} catch(const std::exception&) {return true;}
    return false;
}
}
int main() {
    const auto res4=fixture();
    assert(res4.entry_count()==4);
    const auto report=rloft::OriginalGroundRes4Audit::inspect(res4);
    assert(report.total_entries==4);
    assert(report.type1_entries==3);
    assert(report.rejected_type1_entries==1);
    assert(report.tiles_64x64.size()==1U);
    const auto& tile=report.tiles_64x64[0];
    assert(tile.absolute_entry==1);
    assert(tile.compressed_entry_bytes==4109U);
    assert(tile.compression==0U);
    assert(tile.transform==0U);
    assert(tile.transparent_index==0xFFU);
    assert(tile.decoded_fnv1a32!=0U);

    // The original lookup is relative selector plus a runtime base.
    // A bare Level 22 relative 55 is not proof of absolute RES4[55].
    assert(rloft::OriginalGroundRes4Audit::absolute_index(
        55,-54,4)==1);
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::absolute_index(
            55,0,4);
    }));
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::absolute_index(
            55,-60,4);
    }));
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::absolute_index(
            55,-54,0);
    }));

    const auto source=
        rloft::OriginalGroundRes4Audit::verified_tile_source(res4,1);
    assert(source.size()==4098U);
    assert(source[0]==0 && source[1]==0);
    for(std::size_t i=0;i<4096U;++i) {
        assert(source[2U+i]==static_cast<std::uint8_t>(i&255U));
    }
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::verified_tile_source(
            res4,2);
    }));
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::verified_tile_source(
            res4,3);
    }));

    // Original texture consumes only payload pixels, never synthetic
    // host alignment bytes. Verify with actual 34A74 scanline writes.
    rloft::IndexedFramebuffer fb;
    fb.clear(0xEEU);
    std::array<rloft::GroundPerspectiveRow,128U> rows{};
    rows[0]={0x0400U,0U,0U,0U};
    rloft::GroundTextureShadeTable shading{};
    rloft::GroundTextureRasterSpec spec{};
    spec.row_count=1U;
    auto result=rloft::OriginalGroundTextureRaster::draw(
        fb,source,shading,rows,spec);
    assert(result.rows==1U);
    assert(result.source_reads==256U);
    const auto& pixels=fb.pixels();
    assert(pixels[259U*320U+32U]==0U);
    assert(pixels[259U*320U+33U]==1U);
    assert(pixels[259U*320U+95U]==63U);
    assert(pixels[259U*320U+96U]==0U);
}
