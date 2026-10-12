#include "rloft_native/native_type6_column_sampler.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
template<class F> void rejects(F fn) {
    bool threw=false;
    try{fn();}catch(const std::exception&){threw=true;}
    assert(threw);
}
NativeType1Raster texture(unsigned w) {
    NativeType1Raster t;
    t.frames=1;t.width=static_cast<std::uint16_t>(w);
    t.height=128;
    t.transparent_index=255;
    t.pixels.resize(std::size_t(w)*128U);
    for(unsigned y=0;y<128;++y)
        for(unsigned x=0;x<w;++x)
            t.pixels[std::size_t(y)*w+x]=
                static_cast<std::uint8_t>((y+x*3U)&255U);
    return t;
}
void run() {
    auto t=texture(64);
    auto narrow=texture(32);
    NativeIndexedVgaPlanes v;
    const auto empty=v.planes_sha256();
    const auto h128=NativeType6ProjectedColumn{&t,7,9,128,5,0,0};
    const auto h256=NativeType6ProjectedColumn{&t,7,10,256,5,0,0};
    const auto h64=NativeType6ProjectedColumn{&narrow,31,11,64,254,0,0};
    const auto h32=NativeType6ProjectedColumn{&t,63,12,32,3,1,144};
    const auto result=draw_native_type6_columns(
        {h128,h256,h64,h32},v);
    assert(result.columns==4);
    assert(result.source_texel_reads==480);
    assert(result.plane_writes==480);
    assert(result.first_scanline==0);
    assert(result.last_scanline==287); // page-1 row top=256, h32 ends 287
    assert(result.resulting_four_plane_sha256==v.planes_sha256());
    assert(v.planes_sha256()!=empty);
    // Original 0x800000/128 step -> exact texel y at centered rows 64..191
    assert(v.indexed_pixel(9,63)==0);
    assert(v.indexed_pixel(9,64)==16); // texel[7,0]=21, shade=5
    assert(v.indexed_pixel(9,65)==17);
    assert(v.indexed_pixel(9,191)==143);
    assert(v.indexed_pixel(9,192)==0);
    // Original 0x800000/256 step -> source index y/2, 256 rows
    assert(v.indexed_pixel(10,0)==16);
    assert(v.indexed_pixel(10,1)==16);
    assert(v.indexed_pixel(10,2)==17);
    assert(v.indexed_pixel(10,255)==143);
    // Wrap-around byte subtraction: texel[31,0]=93, minus 254 = 95.
    assert(v.indexed_pixel(11,95)==0);
    assert(v.indexed_pixel(11,96)==95);
    assert(v.indexed_pixel(11,97)==97); // source index 2
    assert(v.indexed_pixel(11,159)==221); // source index126
    // Page one is stored at +32768 bytes/plane, unmodified page zero.
    assert(v.indexed_pixel(12,256,0)==0);
    assert(v.indexed_pixel(12,256,1)==186); // 63*3 -3
    assert(v.indexed_pixel(12,257,1)==190); // h32 phase jumps by 4
    assert(v.indexed_pixel(12,319,1)==0);

    // Reused destination column: latter strip must overwrite earlier.
    const auto short_strip=NativeType6ProjectedColumn{&narrow,0,9,1,0,0,0};
    auto changed=draw_native_type6_columns({short_strip},v);
    assert(changed.columns==1);
    assert(v.indexed_pixel(9,127)==0);
    assert(v.indexed_pixel(9,126)==78);

    // All-or-nothing: even a later invalid source cannot partially
    // overwrite any VGA plane from earlier valid calls.
    const auto before=v.planes_sha256();
    auto bad=h128;bad.original_source_column=64;
    rejects([&]{static_cast<void>(draw_native_type6_columns(
        {h128,bad},v));});
    assert(v.planes_sha256()==before);
    bad=h128;bad.projected_height=0;
    rejects([&]{static_cast<void>(draw_native_type6_columns(
        {h256,bad},v));});
    assert(v.planes_sha256()==before);
    bad=h128;bad.projected_height=257;
    rejects([&]{static_cast<void>(draw_native_type6_columns(
        {h256,bad},v));});
    assert(v.planes_sha256()==before);
    bad=h128;bad.active_400_line_page=2;
    rejects([&]{static_cast<void>(draw_native_type6_columns({bad},v));});
    bad=h128;bad.viewport_x=320;
    rejects([&]{static_cast<void>(draw_native_type6_columns({bad},v));});
    bad=h128;bad.working_region_top_row=145;
    rejects([&]{static_cast<void>(draw_native_type6_columns({bad},v));});
    bad=h128;bad.texture=nullptr;
    rejects([&]{static_cast<void>(draw_native_type6_columns({bad},v));});
    auto incomplete=t;incomplete.pixels.pop_back();
    bad=h128;bad.texture=&incomplete;
    rejects([&]{static_cast<void>(draw_native_type6_columns({bad},v));});
    bad=h128;bad.texture=&narrow;bad.original_source_column=32;
    rejects([&]{static_cast<void>(draw_native_type6_columns({bad},v));});
    rejects([&]{static_cast<void>(draw_native_type6_columns({},v));});
    std::vector<NativeType6ProjectedColumn> too_many(1025,h128);
    rejects([&]{static_cast<void>(draw_native_type6_columns(
        too_many,v));});
    assert(v.planes_sha256()==before);
}
}
int main(){run();}
