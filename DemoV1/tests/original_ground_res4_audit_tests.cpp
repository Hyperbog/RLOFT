#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_ground_res4_audit.hpp"
#include "rloft/original_ground_res4_probe_snapshot.hpp"
#include "rloft/original_ground_texture_raster.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <string>
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
// This is the actually SHIPPED RES4 format: EH6 on disk, decoded by
// CODE.2 37590/37700 into a runtime type-1 cache block at 2EF38.
// The compressed stream below uses 512 original LZSS control byte FF
// groups of eight literal 64x64 indexed-pixel bytes.
rloft::ResArchive eh6_fixture() {
    std::vector<std::uint8_t> archive(4U+5U*4U,0U);
    le32(archive,0U,5U);
    const auto add=[&](std::size_t id,const std::vector<std::uint8_t>& bytes) {
        le32(archive,4U+id*4U,
             static_cast<std::uint32_t>(archive.size()));
        archive.insert(archive.end(),bytes.begin(),bytes.end());
    };
    const auto encoded=[&](bool extended) {
        std::vector<std::uint8_t> bytes{
            'E','H',6U,static_cast<std::uint8_t>(extended?2U:0U),0U};
        if(extended) {
            bytes.push_back(0xA5U); bytes.push_back(0x5AU);
        }
        bytes.push_back(0U);bytes.push_back(0x10U);
        for(std::size_t i=0;i<4096U;i+=8U) {
            bytes.push_back(0xFFU);
            for(std::size_t j=0;j<8U;++j)
                bytes.push_back(static_cast<std::uint8_t>((i+j)&255U));
        }
        return bytes;
    };
    add(0U,{'E','H',7U,0U,0U,0U,0U});
    add(1U,encoded(false));
    auto broken=encoded(false);
    broken.resize(8U);  // control FF without eight literals
    add(2U,broken);
    add(3U,{'E','H',6U,0U,0U,0U,0x20U}); // 8192, not 64x64
    add(4U,encoded(true));
    return rloft::ResArchive(std::move(archive));
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

    const auto real_eh6=eh6_fixture();
    const auto eh6_report=rloft::OriginalGroundRes4Audit::inspect(real_eh6);
    assert(eh6_report.total_entries==5);
    assert(eh6_report.type1_entries==0);
    assert(eh6_report.type6_entries==4);
    assert(eh6_report.rejected_type6_entries==1);
    assert(eh6_report.tiles_64x64.size()==2U);
    assert(eh6_report.tiles_64x64[0].absolute_entry==1);
    assert(eh6_report.tiles_64x64[0].original_eh_type==6U);
    assert(eh6_report.tiles_64x64[1].absolute_entry==4);
    assert(eh6_report.tiles_64x64[1].original_eh_type==6U);
    const auto source_eh6=real_eh6.decode_type6_block(1);
    assert(source_eh6.size()==4098U);
    assert(source_eh6[0]==0U && source_eh6[1]==0x10U);
    assert(real_eh6.decode_type6_block(4)==source_eh6);
    for(std::size_t i=0;i<4096U;++i)
        assert(source_eh6[i+2U]==static_cast<std::uint8_t>(i&255U));
    assert(throws([&] {(void)real_eh6.decode_type6_block(0);}));
    assert(throws([&] {(void)real_eh6.decode_type6_block(2);}));
    assert(rloft::OriginalGroundRes4Audit::verified_tile_source(
        real_eh6,1)==source_eh6);
    // CODE.2 34A74 skips precisely two ORIGINAL EH6 size bytes.
    rloft::LevelState eh6_level{};
    eh6_level.header[7U]=55U;
    std::vector<std::uint8_t> eh6_init(0x10U,0U);
    eh6_init[0x0EU]=0xCAU;eh6_init[0x0FU]=0xFFU;
    // Source base is unsigned. Use a signed selector -54 to resolve 1.
    eh6_level.header[7U]=0xCAU;eh6_level.header[8U]=0xFFU;
    eh6_init[0x0EU]=55U;eh6_init[0x0FU]=0U;
    const auto resolved_eh6=rloft::OriginalGroundRes4Audit::resolve(
        eh6_init,eh6_level,real_eh6);
    assert(resolved_eh6.absolute_archive_entry==1);
    assert(resolved_eh6.raster_source==source_eh6);
    const auto probe_eh6=rloft::OriginalGroundRes4ProbeSnapshot::capture(
        eh6_init,eh6_level,real_eh6);
    assert(probe_eh6.verified && probe_eh6.source_entry==1);
    assert(probe_eh6.verified_pixel_bytes==4096U);
    std::ostringstream native_report;
    probe_eh6.print(native_report);
    assert(native_report.str().find("type6_count=4")!=std::string::npos);
    assert(native_report.str().find("source_EH_type=6")!=std::string::npos);
    assert(native_report.str().find("source_selected=1")!=std::string::npos);

    // Source proof: CODE.2 reads the original INIT word at +0x0E
    // to DGROUP:258F8 before adding the Level 22 relative selector.
    std::vector<std::uint8_t> init(0x10U,0U);
    init[0x0EU]=0x34U;
    init[0x0FU]=0x12U;
    const auto original_base=
        rloft::OriginalGroundRes4Audit::base_from_init(init);
    assert(original_base==0x1234);
    assert(rloft::OriginalGroundRes4Audit::absolute_index(
        55,original_base,0x2000)==0x126B);

    // The production selector is original level.header[7..8],
    // *not* an assumed 55. A signed -54 with INIT base 55
    // selects the exact original-layout resource at archive[1].
    rloft::LevelState level{};
    level.header[7U]=0xCAU; // -54 low byte
    level.header[8U]=0xFFU;
    init[0x0EU]=55U;
    init[0x0FU]=0U;
    const auto resolved=
        rloft::OriginalGroundRes4Audit::resolve(
            init,level,res4);
    assert(resolved.source_init_base==55);
    assert(resolved.signed_level_selector==-54);
    assert(resolved.absolute_archive_entry==1);
    assert(resolved.raster_source==
        rloft::OriginalGroundRes4Audit::verified_tile_source(res4,1));
    // The source selector is level HEADER data rather than a
    // hardcoded Level22 identity. A later Level23-like level
    // with a different signed header selector MUST bind its
    // own authentic RES4 entry, including LZSS-compressed
    // type-1 ground. It must not keep the previous level's
    // indexed resource, palette contents or host test fallback.
    auto destination_level=level;
    destination_level.header[7U]=0xCDU; // signed -51
    destination_level.header[8U]=0xFFU;
    const auto destination_ground=
        rloft::OriginalGroundRes4Audit::resolve(
            init,destination_level,res4);
    assert(destination_ground.source_init_base==55);
    assert(destination_ground.signed_level_selector==-51);
    assert(destination_ground.absolute_archive_entry==4);
    assert(destination_ground.absolute_archive_entry!=
           resolved.absolute_archive_entry);
    assert(destination_ground.raster_source==resolved.raster_source);
    assert(destination_level.header[7U]==0xCDU);
    assert(level.header[7U]==0xCAU);
    // A Level22-like positive +55 field with the same small
    // synthetic archive must fail rather than try an arbitrary tile.
    level.header[7U]=55U;
    level.header[8U]=0U;
    assert(throws([&] {
        (void)rloft::OriginalGroundRes4Audit::resolve(
            init,level,res4);
    }));
    assert(throws([&] {
        (void)rloft::OriginalGroundRes4Audit::base_from_init(
            std::vector<std::uint8_t>(0x0FU,0U));
    }));

    // A bare Level 22 relative 55 is not proof of absolute RES4[55];
    // the production source provides the parsed INIT word. Test the
    // separate signed arithmetic on a small synthetic fixture.
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

    // Diagnostic regression from Windows Full #654: selected entry
    // decode fails with "not an EH type-1 raster". The probe MUST
    // still retain the original archive candidate list and raw bytes.
    using Probe=rloft::OriginalGroundRes4ProbeSnapshot;
    rloft::LevelState probe_level{};
    probe_level.header[7U]=0U;
    probe_level.header[8U]=0U;
    const std::vector<std::uint8_t> zero_base_init(0x10U,0U);
    const auto missing=Probe::capture(zero_base_init,probe_level,res4);
    assert(!missing.verified);
    assert(missing.source_base==0 && missing.source_entry==0);
    assert(missing.selected_raw_bytes==7U);
    assert(missing.selected_prefix_hex=="45 48 07 00 00 00 00");
    assert(missing.error=="RES entry is not an EH type-1 raster");
    assert(missing.archive.total_entries==6);
    assert(missing.archive.tiles_64x64.size()==3U);
    std::ostringstream failed_out;
    missing.print(failed_out);
    assert(failed_out.str().find("selection_error=RES entry is not an EH type-1 raster")!=
           std::string::npos);
    assert(failed_out.str().find("eligible_64x64_count=3")!=std::string::npos);
    assert(failed_out.str().find("candidate\t1\t")!=std::string::npos);
    assert(failed_out.str().find("source_selected=1")==std::string::npos);
    assert(failed_out.str().find("selected_entry_first_13_bytes_hex=45 48 07")!=
           std::string::npos);

    auto probe_init=zero_base_init;
    probe_init[0x0EU]=1U;
    const auto valid_probe=Probe::capture(probe_init,probe_level,res4);
    assert(valid_probe.verified);
    assert(valid_probe.source_entry==1);
    assert(valid_probe.verified_pixel_bytes==4096U);
    assert(valid_probe.error.empty());
    std::ostringstream valid_out;
    valid_probe.print(valid_out);
    assert(valid_out.str().find("candidate\t1\t")!=std::string::npos);
    assert(valid_out.str().find("source_selected=1")!=std::string::npos);

    probe_init[0x0EU]=250U;
    const auto out_of_range_probe=Probe::capture(probe_init,probe_level,res4);
    assert(!out_of_range_probe.verified);
    assert(out_of_range_probe.source_base==250 &&
           out_of_range_probe.source_entry==-1);
    assert(out_of_range_probe.archive.tiles_64x64.size()==3U);
    assert(out_of_range_probe.error.find("out of range")!=std::string::npos);
    const auto short_init_probe=Probe::capture(
        std::vector<std::uint8_t>(0x0FU,0U),probe_level,res4);
    assert(!short_init_probe.verified);
    assert(short_init_probe.source_base==-1);
    assert(short_init_probe.archive.tiles_64x64.size()==3U);
    assert(short_init_probe.error.find("truncated")!=std::string::npos);

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
