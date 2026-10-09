#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_ground_res4_audit.hpp"
#include "rloft/original_ground_texture_raster.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
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
    std::vector<std::uint8_t> bytes(4U+6U*4U,0U);
    le32(bytes,0U,6U);
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
    // A type-1 with original compression code 3: LZSS flags FF
    // describe eight literal bytes per control packet. The decoded
    // payload MUST match the uncompressed candidate byte for byte.
    std::vector<std::uint8_t> compressed(
        valid.begin(),valid.begin()+13U);
    compressed[11U]=3U;
    for(std::size_t i=0;i<4096U;i+=8U) {
        compressed.push_back(0xFFU);
        compressed.insert(
            compressed.end(),valid.begin()+13U+
                static_cast<std::ptrdiff_t>(i),
            valid.begin()+13U+
                static_cast<std::ptrdiff_t>(i+8U));
    }
    append(4,std::move(compressed));
    // Native type-1 parser implements original DC90 one-row
    // transposition: 64 images x 1 row become 1 image x 64 rows.
    // This is also a valid 64x64 tile and must not be filtered out
    // prematurely by the candidate descriptor scan.
    auto transformed=valid;
    transformed[5U]=64U; // original element count
    transformed[7U]=1U;  // original height
    transformed[12U]=1U; // original DC90 transform mode
    append(5,std::move(transformed));
    return rloft::ResArchive(std::move(bytes));
}
template<class Fn> bool throws(Fn&& fn) {
    try {fn();} catch(const std::exception&) {return true;}
    return false;
}
}
int main() {
    const auto res4=fixture();
    assert(res4.entry_count()==6);
    const auto report=rloft::OriginalGroundRes4Audit::inspect(res4);
    assert(report.total_entries==6);
    assert(report.type1_entries==5);
    assert(report.rejected_type1_entries==1);
    assert(report.tiles_64x64.size()==3U);
    const auto& tile=report.tiles_64x64[0];
    assert(tile.absolute_entry==1);
    assert(tile.compressed_entry_bytes==4109U);
    assert(tile.compression==0U);
    assert(tile.transform==0U);
    assert(tile.transparent_index==0xFFU);
    assert(tile.decoded_fnv1a32!=0U);
    const auto& packed=report.tiles_64x64[1];
    assert(packed.absolute_entry==4);
    assert(packed.compression==3U);
    assert(packed.decoded_fnv1a32==tile.decoded_fnv1a32);
    assert(packed.transparent_index==tile.transparent_index);
    const auto& transform=report.tiles_64x64[2];
    assert(transform.absolute_entry==5);
    assert(transform.transform==1U);
    assert(transform.decoded_fnv1a32==tile.decoded_fnv1a32);

    // The original lookup is relative selector plus a runtime base.
    // A bare Level 22 relative 55 is not proof of absolute RES4[55].
    assert(rloft::OriginalGroundRes4Audit::absolute_index(
        55,-54,6)==1);
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::absolute_index(
            55,0,6);
    }));
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::absolute_index(
            55,-60,6);
    }));
    assert(throws([&]{
        (void)rloft::OriginalGroundRes4Audit::absolute_index(
            55,-54,0);
    }));

    const auto source=
        rloft::OriginalGroundRes4Audit::verified_tile_source(res4,1);
    assert(source.size()==4098U);
    const auto compressed_source=
        rloft::OriginalGroundRes4Audit::verified_tile_source(
            res4,4);
    assert(compressed_source==source);
    assert(rloft::OriginalGroundRes4Audit::verified_tile_source(
        res4,5)==source);
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
