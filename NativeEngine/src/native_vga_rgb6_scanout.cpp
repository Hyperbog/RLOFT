#include "rloft_native/native_vga_rgb6_scanout.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_sha256.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_native {
namespace {
constexpr std::size_t kOriginalObject2=0x75F24U;
constexpr std::size_t kOriginalLevelDisplayTriples=0x81ACU;
void require(bool okay,const char* reason) {
    if(!okay)throw std::runtime_error(reason);
}
}

NativeOriginalLevelPaletteTable decode_native_original_level_palettes(
    const std::vector<std::uint8_t>& code) {
    constexpr auto start=kOriginalObject2+kOriginalLevelDisplayTriples;
    constexpr auto bytes=32U*3U;
    require(code.size()>=start+bytes,
            "original CODE.2 level display tuple table truncated");
    NativeOriginalLevelPaletteTable result;
    std::vector<std::uint8_t> original(code.begin()+start,
                                      code.begin()+start+bytes);
    result.original_triplets_sha256=native_sha256_hex(original);
    result.original_code2_sha256=native_sha256_hex(code);
    for(unsigned level=0;level<32;++level) {
        auto& tuple=result.original_display_triples[level];
        for(unsigned component=0;component<3;++component)
            tuple[component]=original[level*3U+component];
        require(tuple[0]<15U,
                "original CODE.2 world display palette bank outside COLORS");
        require(tuple[0]==native_world_palette_bank(level),
                "original CODE.2 display bank disagrees with mapped palette context");
    }
    return result;
}

NativeRgb6Scanout scanout_native_vga_rgb6(
    const NativeIndexedVgaPlanes& video,const NativePalette6& palette,
    unsigned bank,unsigned page,unsigned width,unsigned height) {
    require(bank<15 && page<=1 && width>0 && width<=320 &&
            (height==200 || height==400),
            "original RGB6 scanout width/height/page/bank invalid");
    NativeRgb6Scanout output;
    output.width=width;
    output.height=height;
    output.page=page;
    output.bank=bank;
    output.rgb6.reserve(std::size_t(width)*height*3U);
    // Preserve original indexed-pixel plane selection as 80 bytes/line,
    // x&3 gives plane, page1 starts at 0x8000.
    for(unsigned y=0;y<height;++y) {
        for(unsigned x=0;x<width;++x) {
            const auto pixel=video.indexed_pixel(x,y,page);
            const auto at=std::size_t(pixel)*3U;
            for(unsigned channel=0;channel<3;++channel) {
                const auto value=palette.rgb[at+channel];
                require(value<=63U,"original DAC6 value exceeds 6-bit channel");
                output.rgb6.push_back(value);
            }
        }
    }
    output.rgb6_sha256=native_sha256_hex(output.rgb6);
    return output;
}

NativeRgb6Scanout scanout_native_world_vga_rgb6(
    const NativeIndexedVgaPlanes& video,
    const NativeOriginalLevelPaletteTable& table,
    const std::vector<std::uint8_t>& colors,
    unsigned level,unsigned page,unsigned width,unsigned height) {
    require(level<32 && table.original_triplets_sha256.size()==64 &&
            table.original_code2_sha256.size()==64,
            "original CODE.2 source level display table missing");
    const auto bank=unsigned(table.original_display_triples[level][0]);
    require(bank==native_world_palette_bank(level),
            "original world level palette selector no longer matches source");
    return scanout_native_vga_rgb6(
        video,decode_native_palette(colors,bank),bank,page,width,height);
}
} // namespace rloft_native
