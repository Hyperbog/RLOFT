#include "rloft_native/native_code2_backdrop.hpp"
#include "rloft_native/native_sha256.hpp"
#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace rloft_native {
namespace {
constexpr std::size_t kTable=0x75f24U+0xfe94U;
constexpr unsigned kViewWidth=256U;
constexpr unsigned kViewHeight=256U;
}
NativeCode2Backdrop::NativeCode2Backdrop(
    const std::vector<std::uint8_t>& original_code2) {
    if(original_code2.size()<kTable+fe94_.size())
        throw std::runtime_error("original CODE.2 FE94 backdrop shading table absent");
    const std::vector<std::uint8_t> table(
        original_code2.begin()+kTable,
        original_code2.begin()+kTable+fe94_.size());
    // Exactly the owner-supplied CODE.2 original FE94 shading table,
    // not arbitrary image bytes interpreted as executable VGA data.
    if(native_sha256_hex(table)!=
       "dcd62d3a40b5cbd2e60aa49900c90e097090efc583b4aa7eb5eb1c2f385bfe84")
        throw std::runtime_error("original CODE.2 FE94 backdrop table identity mismatch");
    std::copy(table.begin(),table.end(),fe94_.begin());
}
std::uint8_t NativeCode2Backdrop::source_index(
    const NativeLevelPayload& level,unsigned viewport_row,int pitch_pixels) const {
    if(viewport_row>=kViewHeight || pitch_pixels< -96 || pitch_pixels>96)
        throw std::invalid_argument("backdrop viewport row/pitch invalid");
    // 33E28 writes top-down 128 rows, starting at VGA byte 0x148,
    // with EBX decreasing 128..1; 33DE0 writes bottom-up 128 rows,
    // starting at 0x50F8, with the same descending FE94 source offset.
    // No host per-pixel procedural color guesses.
    const int shifted=std::clamp(int(viewport_row)-pitch_pixels,0,255);
    const unsigned index=shifted<128?unsigned(128-shifted):
                                      unsigned(shifted-127);
    const unsigned source_base=level.original_header[
        shifted<128?0x298U:0x299U];
    return static_cast<std::uint8_t>(
        (source_base-unsigned(fe94_[index]))&0xffU);
}
void NativeCode2Backdrop::fill(
    const NativeLevelPayload& level,
    NativeIndexedVgaPlanes& planes,int pitch_pixels) const {
    for(unsigned row=0;row<kViewHeight;++row) {
        const auto value=source_index(level,row,pitch_pixels);
        const std::size_t offset=std::size_t(row+4U)*80U+8U;
        for(unsigned x=0;x<64;++x)
            planes.masked_write(offset+x,0x0fU,value);
    }
}
} // namespace rloft_native
