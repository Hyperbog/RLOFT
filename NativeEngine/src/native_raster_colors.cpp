#include "rloft_native/native_raster_colors.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft_native {
namespace {
constexpr std::array<std::pair<std::uint8_t,std::uint8_t>,19> kTitleCursorMap{{
    {0x08,0xEB},{0x1B,0xFF},{0x16,0xF9},
    {0x09,0xEC},{0x1A,0xFE},{0x0B,0xEF},
    {0x12,0xF5},{0x10,0xF3},{0x0C,0xEF},
    {0x0D,0xF0},{0x0A,0xED},{0x5C,0xAC},
    {0x58,0x53},{0x55,0x44},{0x53,0x0A},
    {0x56,0x45},{0x57,0x45},{0x54,0x42},
    {0x59,0x53}
}};

std::size_t checked_stride(const NativeType1Raster& raster) {
    if (!raster.width || !raster.height || !raster.frames)
        throw std::invalid_argument("original raster has empty dimensions");
    const std::size_t stride =
        std::size_t(raster.width) * std::size_t(raster.height);
    if (raster.pixels.size() != stride * raster.frames)
        throw std::invalid_argument("original raster data/geometry mismatch");
    return stride;
}

std::uint8_t expand6(std::uint8_t channel) {
    if (channel > 63) throw std::invalid_argument("original DAC6 value exceeds 63");
    return static_cast<std::uint8_t>(
        (static_cast<unsigned>(channel) * 255U + 31U) / 63U);
}
} // namespace

NativeType1Raster remap_native_code2_title_cursor(
    const NativeType1Raster& source) {
    const auto stride=checked_stride(source);
    NativeType1Raster output=source;
    // Original CODE.2 lookup table defaults to zero for every index
    // except these nineteen explicit cursor patches.
    std::array<std::uint8_t,256> mapping{};
    for(const auto& entry:kTitleCursorMap) mapping[entry.first]=entry.second;
    for(std::size_t i=0;i<stride;++i) {
        const auto idx=output.pixels[i];
        if(output.transparent_index!=0xFF && idx==output.transparent_index)
            continue;
        output.pixels[i]=mapping[idx];
    }
    return output;
}

NativeBgraRaster colorize_native_raster(
    const NativeType1Raster& source, const NativePalette6& palette,
    std::size_t frame, NativeRasterColorMode mode) {
    const auto stride=checked_stride(source);
    if(frame>=source.frames)
        throw std::out_of_range("original image frame outside raster");
    // CODE.2 remaps ONLY frame zero; it must never recolor later frames.
    if(mode==NativeRasterColorMode::Code2TitleCursor && frame!=0)
        throw std::invalid_argument("CODE.2 cursor remap applies only to first frame");
    if(stride>4U*1024U*1024U)
        throw std::runtime_error("native preview frame exceeds safe display bound");
    const auto transformed=
        mode==NativeRasterColorMode::Code2TitleCursor ?
            remap_native_code2_title_cursor(source) : source;

    NativeBgraRaster out;
    out.width=source.width;
    out.height=source.height;
    out.bgra.resize(stride*4U);
    const auto start=frame*stride;
    for(std::size_t i=0;i<stride;++i) {
        const auto index=transformed.pixels[start+i];
        const auto offset=3U*std::size_t(index);
        const auto target=4U*i;
        if(source.transparent_index!=0xFF &&
           index==source.transparent_index) {
            // Deterministic dark inspector backdrop, not palette inversion.
            out.bgra[target]=35;
            out.bgra[target+1]=27;
            out.bgra[target+2]=22;
        } else {
            out.bgra[target]=expand6(palette.rgb[offset+2]);  // B
            out.bgra[target+1]=expand6(palette.rgb[offset+1]); // G
            out.bgra[target+2]=expand6(palette.rgb[offset]);   // R
        }
        out.bgra[target+3]=255;
    }
    return out;
}

} // namespace rloft_native
