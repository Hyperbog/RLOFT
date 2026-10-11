#include "rloft_native/native_raster_colors.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
template <typename F> void must_fail(F f) {
    bool failed=false;
    try{f();}catch(const std::exception&){failed=true;}
    assert(failed);
}
void run() {
    NativePalette6 bank8{},bank12{};
    // Known non-grey primaries catch RGB/BGR swaps that a greyscale image
    // cannot reveal. Indices are deliberately nonzero and non-symmetric.
    bank8.rgb[3*8]=63;             // red
    bank8.rgb[3*9+1]=63;           // green
    bank8.rgb[3*10+2]=63;          // blue
    bank12.rgb[3*8+1]=63;          // same pixel, DIFFERENT scene palette
    bank12.rgb[3*0xEB+2]=63;       // translated CODE.2 index 0x08 -> 0xEB
    NativeType1Raster raster;
    raster.frames=1;
    raster.width=3;
    raster.height=1;
    raster.transparent_index=255; // FF means no transparent key
    raster.pixels={8,9,10};

    const auto raw=colorize_native_raster(
        raster,bank8,0,NativeRasterColorMode::RawIndices);
    assert(raw.width==3 && raw.height==1);
    assert((raw.bgra==std::vector<std::uint8_t>{
        0,0,255,255,     // red:  Windows BGRA bytes
        0,255,0,255,     // green
        255,0,0,255      // blue
    }));
    const auto other=colorize_native_raster(raster,bank12,0);
    assert((other.bgra==std::vector<std::uint8_t>{
        0,255,0,255, 0,0,0,255, 0,0,0,255
    }));
    assert(raster.pixels[0]==8); // source indexed bytes immutable

    auto menu=remap_native_code2_title_cursor(raster);
    assert((menu.pixels==std::vector<std::uint8_t>{0xEB,0xEC,0xED}));
    const auto displayed=colorize_native_raster(
        raster,bank12,0,NativeRasterColorMode::Code2TitleCursor);
    assert((displayed.bgra==std::vector<std::uint8_t>{
        255,0,0,255, 0,0,0,255, 0,0,0,255
    }));
    // The original only remaps frame zero and keeps the resource key.
    raster.frames=2;
    raster.pixels={8,9,10, 8,9,10};
    auto first=remap_native_code2_title_cursor(raster);
    assert(first.pixels[0]==0xEB);
    assert(first.pixels[3]==8);
    must_fail([&]{static_cast<void>(colorize_native_raster(
        raster,bank12,1,NativeRasterColorMode::Code2TitleCursor));});
    auto second=colorize_native_raster(raster,bank8,1);
    assert(second.bgra[2]==255);

    raster.frames=1;
    raster.width=2;
    raster.pixels={8,0x1B};
    raster.transparent_index=0x1B;
    auto transparent=remap_native_code2_title_cursor(raster);
    assert(transparent.pixels[0]==0xEB);
    assert(transparent.pixels[1]==0x1B);
    const auto composite=colorize_native_raster(
        raster,bank12,0,NativeRasterColorMode::Code2TitleCursor);
    assert((std::vector<std::uint8_t>(
        composite.bgra.begin()+4,composite.bgra.end())
        ==std::vector<std::uint8_t>{35,27,22,255}));

    NativePalette6 invalid=bank8;
    invalid.rgb[3*8]=64;
    raster.transparent_index=255;
    must_fail([&]{static_cast<void>(colorize_native_raster(raster,invalid,0));});
    must_fail([&]{static_cast<void>(colorize_native_raster(raster,bank8,1));});
    raster.pixels.pop_back();
    must_fail([&]{static_cast<void>(colorize_native_raster(raster,bank8,0));});
}
}
int main(){run();}
