#include "rloft_native/native_party_item_icon.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
template<class F> void rejects(F&& call) {
    bool failed=false;
    try{call();}catch(const std::exception&){failed=true;}
    assert(failed);
}
std::vector<std::uint8_t> raster(unsigned key,
                                  std::vector<std::uint8_t> indices) {
    assert(indices.size()==8U);
    std::vector<std::uint8_t> bytes{
        'E','H',1,8,0,1,0,1,0,1,
        static_cast<std::uint8_t>(key),0,0
    };
    bytes.insert(bytes.end(),indices.begin(),indices.end());
    return bytes;
}
NativeResArchive fixture(bool bad=false) {
    constexpr unsigned count=428U;
    std::vector<std::uint8_t> result(4U+4U*count,0);
    result[0]=static_cast<std::uint8_t>(count);
    result[1]=static_cast<std::uint8_t>(count>>8U);
    const auto red=raster(7U,{1,7,2,3,4,5,6,7});
    const auto blue=raster(255U,{2,2,2,2,2,2,2,2});
    for(unsigned id=0;id<count;++id) {
        const auto offset=static_cast<std::uint32_t>(result.size());
        for(unsigned n=0;n<4U;++n)
            result[4U+4U*id+n]=static_cast<std::uint8_t>(offset>>(n*8U));
        if(id==168U)result.insert(result.end(),red.begin(),red.end());
        if(id==169U)result.insert(
            result.end(),blue.begin(),blue.end());
    }
    if(bad) {
        // Change original type1 header of occupied slot 168.
        const auto start=4U+4U*count;
        result[start+2U]=3;
    }
    return NativeResArchive(std::move(result));
}
}
int main() {
    auto source=fixture();
    NativePalette6 palette{};
    palette.rgb[1U*3U]=63U;            // index1 is R
    palette.rgb[2U*3U+1U]=63U;         // index2 is G
    palette.rgb[3U*3U+2U]=63U;         // index3 is B
    const auto red=decode_native_party_item_icon(source,168U,palette);
    assert(red.original_res1_id==168U);
    assert(red.width==8U&&red.height==1U);
    assert(red.original_frame_count==1U);
    assert(red.original_transparent_index==7U);
    assert(!red.source_is_intentionally_empty);
    assert(!red.original_ui_frame_and_palette_parity_verified);
    assert(red.compressed_source_sha256.size()==64U);
    assert(red.rgba.size()==32U);
    assert(red.rgba[0]==255U && red.rgba[1]==0U &&
           red.rgba[2]==0U && red.rgba[3]==255U);
    assert(red.rgba[7]==0U); // index7 is original transparent key
    assert(red.rgba[4U*2U]==0U && red.rgba[4U*2U+1U]==255U);
    assert(red.rgba[4U*3U+2U]==255U);
    const auto blue=decode_native_party_item_icon(source,169U,palette);
    assert(blue.original_transparent_index==255U);
    assert(blue.rgba[3U]==255U); // FF means key disabled
    assert(blue.rgba[1U]==255U); // uses current source palette, not title bank
    const auto blank=decode_native_party_item_icon(source,170U,palette);
    assert(blank.source_is_intentionally_empty);
    assert(blank.rgba.empty()&&blank.compressed_source_sha256.empty());
    const auto corpus=audit_native_party_item_icon_corpus(source,palette);
    assert(corpus.examined==168U);
    assert(corpus.nonempty==2U&&corpus.zero_length==166U);
    assert(corpus.decoded_pixels==16U);
    assert(!corpus.original_ui_frame_palette_parity_verified);
    rejects([&]{static_cast<void>(
        decode_native_party_item_icon(source,167U,palette));});
    rejects([&]{static_cast<void>(
        decode_native_party_item_icon(source,336U,palette));});
    palette.rgb[3U]=64U;
    rejects([&]{static_cast<void>(
        decode_native_party_item_icon(source,168U,palette));});
    const auto corrupt=fixture(true);
    NativePalette6 sane{};
    rejects([&]{static_cast<void>(
        decode_native_party_item_icon(corrupt,168U,sane));});
    rejects([&]{static_cast<void>(
        audit_native_party_item_icon_corpus(corrupt,sane));});
    assert(decode_native_party_item_icon(source,168U,sane).rgba[0]==0U);
}
