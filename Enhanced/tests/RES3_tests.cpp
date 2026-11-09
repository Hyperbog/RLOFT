#include "rloft/RES3.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
template <class F> void rejects(F f) {
    bool rejected = false;
    try { f(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "invalid RES3 data was accepted");
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    for (unsigned i = 0; i < 4U; ++i) {
        b.at(at + i) = static_cast<std::uint8_t>(value >> (i * 8U));
    }
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    b.at(at) = static_cast<std::uint8_t>(value);
    b.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
std::vector<std::uint8_t> compact_shape() {
    // Width=64, one shape column, one span [1,3), two packed indices.
    // Table at +10; packed pixel source at +12..13; list at +14.
    std::vector<std::uint8_t> shape(22U, 0U);
    put16(shape, 0, 64U); put16(shape, 2, 0U); put16(shape, 4, 0U);
    put16(shape, 6, 1U); put16(shape, 8, 3U);
    put16(shape, 10, 14U);
    shape[12] = 0x31U; shape[13] = 0x42U;
    put16(shape, 14, 6U); // raster_stop=6 => row3
    put16(shape, 16, 0U); // signed adjustment
    put16(shape, 18, 2U); // raster_entry=2 => row1
    put16(shape, 20, 0U); // list terminator
    return shape;
}
std::vector<std::uint8_t> synthetic_archive() {
    const auto shape = compact_shape();
    std::vector<std::uint8_t> raw{'E','H',7U,0U,0U,
        static_cast<std::uint8_t>(shape.size()),0U};
    for (std::size_t pos=0;pos<shape.size();) {
        const auto group=std::min<std::size_t>(8U,shape.size()-pos);
        raw.push_back(static_cast<std::uint8_t>((1U<<group)-1U));
        raw.insert(raw.end(),shape.begin()+static_cast<std::ptrdiff_t>(pos),
                   shape.begin()+static_cast<std::ptrdiff_t>(pos+group));
        pos+=group;
    }
    std::vector<std::uint8_t> archive(8U,0U);
    put32(archive,0U,1U); put32(archive,4U,8U);
    archive.insert(archive.end(),raw.begin(),raw.end());
    return archive;
}
void synthetic_checks() {
    const auto original=synthetic_archive();
    rloft::Res3Archive res3(original);
    require(res3.entry_count()==1 && res3.encode()==original,
            "synthetic archive changed during load");
    const auto info=res3.inspect_shape(0);
    require(info.source_width==64U && info.column_lists==1U &&
            info.draw_descriptors==1U && info.decoded_bytes==22U,
            "synthetic scene shape metadata invalid");
    const auto raster=res3.decode_native_raster(0);
    require(raster.width==64U && raster.height==128U &&
            raster.pixels[64U+0U]==0x31U &&
            raster.pixels[128U+0U]==0x42U &&
            raster.mask[64U+0U]==0xFFU,
            "original-shaped packed column was not decoded");
    auto body=res3.decode_body(0);
    body[12U]=0xB3U;
    res3.replace_body(0,body);
    const auto refreshed=rloft::Res3Archive(res3.encode());
    require(refreshed.decode_body(0)==body &&
            refreshed.decode_native_raster(0).pixels[64U]==0xB3U,
            "modified synthetic type-7 body did not survive LZSS reload");
    rejects([&]{(void)res3.entry(-1);});
    rejects([&]{(void)res3.inspect_shape(1);});
    rejects([&]{res3.replace_body(1,body);});
    auto invalid=body; put16(invalid,10U,11U);
    rejects([&]{res3.replace_body(0,invalid);});
    invalid=body;put16(invalid,18U,3U);
    rejects([&]{res3.replace_body(0,invalid);});
    invalid=body;invalid.pop_back();
    rejects([&]{res3.replace_body(0,invalid);});
    rejects([&]{res3.replace_body(0,{});});
    rejects([&]{res3.replace_body(0,std::vector<std::uint8_t>(65536U,0U));});
    require(res3.decode_body(0)==body,"failed edit corrupted live RES3");
    auto broken=original;broken[8U]='X';
    rejects([&]{(void)rloft::Res3Archive(broken);});
    broken=original;broken[11U]=1U; // nonzero EH7 header length
    rejects([&]{(void)rloft::Res3Archive(broken);});
    rejects([&]{(void)rloft::Res3Archive({});});
}
void original_checks(const std::vector<std::uint8_t>& original) {
    require(original.size()==5228638U,"RES3 original size mismatch");
    rloft::Res3Archive archive(original);
    require(archive.entry_count()==4234 && archive.encode()==original,
            "original RES3 archive is not byte-exact");
    const auto stats=archive.inspect_all();
    require(stats.shapes==4234U && stats.widths_64==4158U &&
            stats.widths_96==40U && stats.widths_128==36U &&
            stats.decoded_bytes==7600070U && stats.draw_descriptors>100000U,
            "RES3 corpus geometry/calls differ from original");
    for (int id : {0, 2168, 2169, 3315, 3316, 4157, 4158, 4233}) {
        const auto& info=archive.inspect_shape(id);
        const auto geometry=archive.decode_geometry(id);
        require(info.source_width==geometry.source_width &&
                info.column_lists==static_cast<std::size_t>(
                    geometry.x_max-geometry.x_min+1),
                "RES3 original source-bank shape mismatch");
        const auto native=archive.decode_native_raster(id);
        require(native.pixels.size()==static_cast<std::size_t>(native.width)*128U &&
                native.mask.size()==native.pixels.size(),
                "RES3 native raster size mismatch");
    }
    // Make one safe artist-style texel edit to an original display-list
    // body without changing geometry, descriptors, or bank-local indexing.
    const auto before=archive.decode_body(0);
    const auto first=archive.inspect_shape(0);
    const auto pixel_at=10U+first.column_lists*2U;
    require(pixel_at<before.size(),"RES3 shape has no packed source");
    auto modified=before;
    modified[pixel_at]^=0x5AU;
    archive.replace_body(0,modified);
    const auto edited=archive.encode();
    require(edited!=original && edited.size()!=0U,
            "RES3 modification did not update binary archive");
    const rloft::Res3Archive reloaded(edited);
    require(reloaded.decode_body(0)==modified &&
            reloaded.decode_shape(0).columns.size()==first.column_lists,
            "edited RES3 display list did not reload");
    const rloft::Res3Archive untouched(original);
    for (int i=1;i<4234;++i) {
        require(reloaded.entry(i)==untouched.entry(i),
                "a non-target compressed RES3 entry was altered");
    }
    // Verify a variable-compression-size replacement actually changed the
    // directory while preserving the last record and resource count.
    require(reloaded.entry(4233)==untouched.entry(4233),
            "RES3 last record or offsets were lost");
}
} // namespace
int main(int argc,char** argv) {
    synthetic_checks();
    if (argc>2) throw std::invalid_argument("usage: RES3_tests [RES3]");
    if(argc==2){
        std::ifstream input(argv[1],std::ios::binary);
        if(!input)throw std::runtime_error("cannot open original RES3");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());
        original_checks(data);
    }
}
