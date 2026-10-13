#include "rloft_native/native_party_item_icon.hpp"
#include "rloft_native/native_sha256.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_native {
namespace {
void require(bool ok,const char* why) {
    if(!ok)throw std::runtime_error(why);
}
std::uint8_t rgb8(std::uint8_t value) {
    require(value<=63U,
            "original CODE.2 inventory preview palette is not VGA DAC6");
    return static_cast<std::uint8_t>(
        (static_cast<unsigned>(value)*255U+31U)/63U);
}
} // namespace

NativePartyItemIcon decode_native_party_item_icon(
    const NativeResArchive& res1,unsigned id,const NativePalette6& palette) {
    require(res1.entry_count()==428U,
            "original RES1 item resource archive count changed");
    require(id>=168U && id<=335U,
            "item UI resource ID lies outside original INIT base 168 namespace");
    NativePartyItemIcon out;
    out.original_res1_id=id;
    const auto bytes=res1.entry(id);
    if(bytes.empty()) {
        out.source_is_intentionally_empty=true;
        return out;
    }
    require(bytes.size()>=13U && bytes[0]=='E'&&bytes[1]=='H'&&
            bytes[2]==1U,
            "nonempty original inventory resource is not EH type1");
    const auto raster=decode_native_type1_raster(bytes);
    require(raster.frames>0U && raster.frames<=256U &&
            raster.width>0U && raster.width<=512U &&
            raster.height>0U && raster.height<=512U &&
            std::size_t(raster.width)*raster.height<=65536U,
            "original item UI resource frame outside safe native texture bounds");
    const auto pixels=std::size_t(raster.width)*raster.height;
    require(raster.pixels.size()==pixels*raster.frames,
            "original inventory EH1 frame data size mismatch");
    out.compressed_source_sha256=native_sha256_hex(bytes);
    out.width=raster.width;
    out.height=raster.height;
    out.original_frame_count=raster.frames;
    out.original_transparent_index=raster.transparent_index;
    out.rgba.reserve(pixels*4U);
    // This is a direct first-frame preview. No surrogate texture,
    // remapped title cursor, inferred animation or invented palette.
    for(std::size_t i=0;i<pixels;++i) {
        const auto index=raster.pixels[i];
        const auto offset=std::size_t(index)*3U;
        const auto alpha=raster.transparent_index!=0xFFU &&
                         index==raster.transparent_index?0U:255U;
        out.rgba.push_back(rgb8(palette.rgb[offset]));
        out.rgba.push_back(rgb8(palette.rgb[offset+1U]));
        out.rgba.push_back(rgb8(palette.rgb[offset+2U]));
        out.rgba.push_back(static_cast<std::uint8_t>(alpha));
    }
    return out;
}

NativePartyItemIconCorpus audit_native_party_item_icon_corpus(
    const NativeResArchive& archive,const NativePalette6& palette) {
    require(archive.entry_count()==428U,
            "original RES1 item icon audit count changed");
    NativePartyItemIconCorpus evidence;
    for(unsigned id=168U;id<=335U;++id) {
        const auto item=decode_native_party_item_icon(archive,id,palette);
        ++evidence.examined;
        if(item.source_is_intentionally_empty) {
            ++evidence.zero_length;
            require(item.rgba.empty()&&item.compressed_source_sha256.empty(),
                    "original empty RES1 entry incorrectly materialized");
        } else {
            ++evidence.nonempty;
            require(item.original_res1_id==id &&
                    item.compressed_source_sha256.size()==64U &&
                    item.rgba.size()==std::size_t(item.width)*item.height*4U,
                    "original RES1 EH1 item preview provenance invalid");
            evidence.decoded_pixels+=std::size_t(item.width)*item.height;
        }
    }
    require(evidence.examined==168U &&
            evidence.nonempty+evidence.zero_length==168U,
            "original RES1 168-slot UI family incomplete");
    return evidence;
}
} // namespace rloft_native
