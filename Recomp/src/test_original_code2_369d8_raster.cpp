#ifdef NDEBUG
#undef NDEBUG
#endif
#include "original_code2_369d8_raster.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>
using namespace rloft_recomp;
static void word(std::vector<std::uint8_t>& m,unsigned at,unsigned value) {
    m[at]=static_cast<std::uint8_t>(value);
    m[at+1]=static_cast<std::uint8_t>(value>>8);
}
int main() {
    // Generated SINTAB x86: read source+4; AH=AL; subtract ECX;
    // write AL to VGA; write AH one 80-byte row below; return.
    std::vector<std::uint8_t> guest(0xB0000,0);
    constexpr unsigned code=0x2000,body=0x1002,list=body+0x20;
    word(guest,code+2,0x210); // temporary RET before second store
    word(guest,code+4,0x200); // generated code entry
    const unsigned bytes[]={0x8a,0x86,4,0,0,0,0x88,0xc4,0x29,0xc8,
                            0x88,0x87,0,0,0,0,0x88,0xa7,80,0,0,0,0xc3};
    for(unsigned i=0;i<sizeof(bytes)/sizeof(*bytes);++i)
        guest[code+0x200+i]=static_cast<std::uint8_t>(bytes[i]);
    guest[body+2]=42;
    word(guest,list,2);word(guest,list+2,0xfffe);
    word(guest,list+4,4);word(guest,list+6,0);
    Original369d8Context c{};
    c.image_begin=body-2;c.image_end=body+0x80;
    c.source_body=body;c.list_pointer=list;
    c.sintab_begin=code;c.sintab_end=code+0x300;
    c.screen_x=2;c.run_width=5;c.packed_lookup=0x0402;
    c.renderer_phase=4;c.video_page=1;
    rloft::graphics::OriginalVgaHost vga;
    const auto out=execute_original_369d8(guest,vga,c);
    const unsigned addr=0x8000+4*80;
    assert(out.descriptors==1 && out.plane_passes==2 &&
           out.indexed_writes==2);
    assert(vga.indexed_scanout(2,4)==0); // active display start not set yet
    assert(out.lookup_after==0x0204 && out.last_plane_mask==7);
    assert(guest[code+0x210]==0x88); // instruction patch not retained
    // Check actual planes by making VGA page 1 the visible CRTC start.
    assert(vga.write_indexed_port(0x3D4u,0x800Cu)); // high CRTC start
    assert(vga.write_indexed_port(0x3D4u,0x000Du)); // low CRTC start
    // Indexed source shade 42 - 2 = 40 at original planar x=2..6.
    // This host presents the 320x400 VGA surface via CRTC start 0x8000.
    for(unsigned x=2;x<7;++x)
        assert(vga.indexed_scanout(x,4)==40);
    assert(vga.indexed_scanout(7,4)==0);
    // Source mask table 1..7, phases 0..3: all 28 retail combinations.
    for(unsigned offset=0;offset<4;++offset)
      for(unsigned width=1;width<=7;++width) {
        unsigned bits=0;
        for(unsigned p=0;p<3;++p) {
            const auto mask=original_369d8_mask(offset,width,p);
            for(unsigned i=0;i<4;++i)bits+=(mask>>i)&1u;
        }
        assert(bits==width);
      }
    // Individual 36B42: odd x swaps AL/AH; original lookup restored.
    c.screen_x=1;c.run_width=2;c.individual_columns=true;
    auto separated=execute_original_369d8(guest,vga,c);
    assert(separated.descriptors==2 && separated.plane_passes==2);
    assert(separated.lookup_after==0x0402);
    assert(vga.indexed_scanout(1,4)==38);
    assert(vga.indexed_scanout(2,4)==40);
    // Failed run cannot commit any staged pixels or change original code.
    const auto old_indices=vga.scanout_indices();
    const auto old_guest=guest;
    c.screen_x=2;c.run_width=5;c.individual_columns=false;
    word(guest,list+4,0x102); // invalid selector, after preflight
    bool rejected=false;
    try {(void)execute_original_369d8(guest,vga,c);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected && vga.scanout_indices()==old_indices);
    word(guest,list+4,4);
    assert(guest==old_guest);
    c.run_width=8;rejected=false;
    try {(void)execute_original_369d8(guest,vga,c);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    (void)addr;
}
