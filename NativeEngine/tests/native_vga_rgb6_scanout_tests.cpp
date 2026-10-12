#include "rloft_native/native_vga_rgb6_scanout.hpp"
#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_sha256.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
template<class F> void rejects(F fn) {
    bool rejected=false;
    try { fn(); } catch(const std::exception&) { rejected=true; }
    assert(rejected);
}
std::vector<std::uint8_t> palettes() {
    std::vector<std::uint8_t> data(1+15U*768U,0);
    data[0]=15;
    for(unsigned bank=0;bank<15;++bank)
        for(unsigned index=0;index<256;++index)
            for(unsigned component=0;component<3;++component)
                data[1+bank*768U+index*3U+component]=
                    static_cast<std::uint8_t>(
                        (index/((component==0)?1U:
                                (component==1)?4U:16U)+bank+component)&63U);
    return data;
}
std::vector<std::uint8_t> code2() {
    constexpr std::size_t start=0x75F24U+0x81ACU;
    std::vector<std::uint8_t> source(start+96U,0);
    for(unsigned level=0;level<32;++level) {
        source[start+level*3U]=
            static_cast<std::uint8_t>(native_world_palette_bank(level));
        source[start+level*3U+1U]=
            static_cast<std::uint8_t>(level+2U);
        source[start+level*3U+2U]=
            static_cast<std::uint8_t>(31U-level);
    }
    return source;
}
void test() {
    const auto original=code2();
    const auto colors=palettes();
    auto table=decode_native_original_level_palettes(original);
    assert(table.original_code2_sha256==native_sha256_hex(original));
    assert(table.original_triplets_sha256.size()==64);
    for(unsigned level=0;level<32;++level){
        const auto entry=table.original_display_triples[level];
        assert(entry[0]==native_world_palette_bank(level));
        assert(entry[1]==level+2);
        assert(entry[2]==31-level);
    }
    auto modified=original;
    constexpr std::size_t base=0x75F24U+0x81ACU;
    modified[base+29*3U]=2; // source original level29 = bank0
    rejects([&]{static_cast<void>(
        decode_native_original_level_palettes(modified));});
    modified=original;
    modified[base]=15;
    rejects([&]{static_cast<void>(
        decode_native_original_level_palettes(modified));});
    modified=original;
    modified.pop_back();
    rejects([&]{static_cast<void>(
        decode_native_original_level_palettes(modified));});

    NativeIndexedVgaPlanes planar;
    planar.masked_write(0,0x0f,1);
    planar.masked_write(1,0x01,42); // x4; other lanes x5..7 zero
    planar.masked_write(32768U+399U*80U+79U,8,255); // last pixel page1
    const auto p0=scanout_native_world_vga_rgb6(
        planar,table,colors,0,0,320,400);
    assert(p0.rgb6.size()==320U*400U*3U);
    assert(p0.bank==4 && p0.height==400 && p0.page==0);
    assert(p0.rgb6_sha256==native_sha256_hex(p0.rgb6));
    for(unsigned x=0;x<4;++x) {
        for(unsigned comp=0;comp<3;++comp) {
            assert(p0.rgb6[x*3U+comp]==
                   colors[1U+4U*768U+1U*3U+comp]);
        }
    }
    for(unsigned comp=0;comp<3;++comp)
        assert(p0.rgb6[4U*3U+comp]==
               colors[1U+4U*768U+42U*3U+comp]);
    const auto p29=scanout_native_world_vga_rgb6(
        planar,table,colors,29,0,320,200);
    assert(p29.bank==0 && p29.height==200);
    assert(p29.rgb6_sha256!=p0.rgb6_sha256);
    const auto p1=scanout_native_world_vga_rgb6(
        planar,table,colors,0,1,320,400);
    assert(p1.bank==4 && p1.page==1);
    const auto last=3U*(399U*320U+319U);
    for(unsigned comp=0;comp<3;++comp)
        assert(p1.rgb6[last+comp]==
               colors[1U+4U*768U+255U*3U+comp]);
    for(unsigned comp=0;comp<3;++comp)
        assert(p1.rgb6[comp]==colors[1U+4U*768U+comp]);
    assert(p1.rgb6_sha256!=p0.rgb6_sha256);
    for(unsigned level=0;level<32;++level) {
        const auto r=scanout_native_world_vga_rgb6(
            planar,table,colors,level,0,320,200);
        assert(r.rgb6.size()==320U*200U*3U);
        assert(r.bank==native_world_palette_bank(level));
        for(const auto value:r.rgb6)assert(value<=63);
    }
    auto bad=colors;
    bad[1+4U*768U]=64;
    rejects([&]{static_cast<void>(
        scanout_native_world_vga_rgb6(planar,table,bad,0));});
    rejects([&]{static_cast<void>(
        scanout_native_world_vga_rgb6(planar,table,colors,32));});
    rejects([&]{static_cast<void>(
        scanout_native_world_vga_rgb6(planar,table,colors,0,2));});
    rejects([&]{static_cast<void>(
        scanout_native_world_vga_rgb6(planar,table,colors,0,0,321));});
    rejects([&]{static_cast<void>(
        scanout_native_world_vga_rgb6(planar,table,colors,0,0,0));});
    rejects([&]{static_cast<void>(
        scanout_native_world_vga_rgb6(planar,table,colors,0,0,320,256));});
    const auto direct=scanout_native_vga_rgb6(
        planar,decode_native_palette(colors,4),4);
    assert(direct.rgb6_sha256==p0.rgb6_sha256);
}
} // namespace
int main() { test(); }
