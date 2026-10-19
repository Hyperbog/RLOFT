#ifdef NDEBUG
#undef NDEBUG
#endif
#include "original_code2_369d8_raster.hpp"
#include "original_code2_36cec_scene.hpp"
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
    c.run_width=5u; // restore valid grouped width after width=8 rejection
    // Authentic 369D8 ABI: ESI is image+signed adjustment, not
    // necessarily in the image. The generated [ESI+disp32] read is.
    // Reproduce a negative display-list adjustment exceeding the
    // decoded two-byte prefix; only the effective guest read is bounded.
    word(guest,list+2,0xffd8); // image - 40
    word(guest,code+0x200u+2u,42u); // source - 40 + 42 = image + 2
    const auto negative_adjustment=execute_original_369d8(guest,vga,c);
    assert(negative_adjustment.indexed_writes==2u);
    word(guest,code+0x200u+2u,36u); // source - 40 + 36 = image - 4
    rejected=false;
    try {(void)execute_original_369d8(guest,vga,c);}
    catch(const std::runtime_error&) {rejected=true;}
    assert(rejected); // no out-of-allocation guest read is permitted
    // Rejected address and unknown opcode are different host boundaries.
    // Diagnostic classification must never claim both are bad opcodes.
    {
        const auto bad_read=execute_sintab_scale(
            code+0x200u,body-40u,0xA8000u,0u,
            code,code+0x300u,body-2u,body+0x80u,
            [&](std::uint32_t at)->std::uint8_t{return guest.at(at);},
            [](std::uint32_t,std::uint8_t){});
        assert(!bad_read.ok &&
            bad_read.failure==SintabRasterFailure::source_read_out_of_range);
        const auto saved=guest[code+0x200u];
        guest[code+0x200u]=0x90u; // intentionally unsupported synthetic NOP
        const auto unknown=execute_sintab_scale(
            code+0x200u,body,0xA8000u,0u,
            code,code+0x300u,body-2u,body+0x80u,
            [&](std::uint32_t at)->std::uint8_t{return guest.at(at);},
            [](std::uint32_t,std::uint8_t){});
        assert(!unknown.ok &&
            unknown.failure==SintabRasterFailure::unknown_instruction &&
            unknown.failure_opcode==0x90u &&
            unknown.failure_ip==code+0x200u);
        guest[code+0x200u]=saved;
    }
    word(guest,list+2,0xfffe);
    word(guest,code+0x200u+2u,4u);
    // Entire original 36CEC -> 369D8 traversal, SYNTHETIC actor/rays.
    // This is a bounded unit test, never a claim of genuine saved visibility.
    std::vector<std::uint8_t> scene(0xB0000u,0u);
    auto dword=[&](unsigned at,unsigned value) {
        word(scene,at,value&65535u);
        word(scene,at+2u,value>>16u);
    };
    constexpr unsigned scene_code=0x2000u,scene_image=0x1002u;
    dword(0x18FE4u,0x2204u); // scale-block 1 -> source SINTAB
    dword(0xC738u,1u);
    dword(0xC730u,508u);
    dword(0x258C4u,0x4000u); // 509 source lookup words
    dword(0xC6D8u,256u); // source ray count
    dword(0x15A18u,100u); // x0 foreground occlusion
    dword(0x15A18u+28u,1u); // x1 background = sprite visible
    word(scene,scene_code+2u,0x210u); // stop RET
    word(scene,scene_code+4u,0x200u); // entry
    word(scene,scene_code+0x102u+2u*31u,1u);
    word(scene,scene_code+0x102u+2u*32u,1u);
    for(unsigned i=0u;i<sizeof(bytes)/sizeof(*bytes);++i)
        scene[scene_code+0x200u+i]=static_cast<std::uint8_t>(bytes[i]);
    word(scene,scene_image-2u,0x2Cu); // real decompressed prefix
    word(scene,scene_image+0u,64u);
    word(scene,scene_image+2u,31u);
    word(scene,scene_image+4u,32u);
    word(scene,scene_image+6u,0u);
    word(scene,scene_image+8u,128u);
    word(scene,scene_image+10u,14u); // shared two-column list
    word(scene,scene_image+12u,14u);
    word(scene,scene_image+14u,2u); // original stop selector
    word(scene,scene_image+16u,0u); // source adjustment
    word(scene,scene_image+18u,4u); // entry selector
    word(scene,scene_image+20u,0u); // terminator
    rloft::graphics::OriginalVgaHost scene_vga;
    Original36cecCall sprite{};
    sprite.screen_x=1u;
    sprite.res3_id=1829u;
    sprite.projected_size=8u;
    sprite.ray_depth=10u;
    const auto integrated=raster_original_36cec_scene(
        scene,scene_vga,sprite,scene_image,0x2Cu,
        scene_code,scene_code+0x300u);
    assert(integrated.source_columns==2u);
    assert(integrated.visible_runs==1u &&
           integrated.occluded_columns==1u);
    assert(integrated.indexed_writes==1u);
    assert(scene_vga.indexed_scanout(0u,130u)==0u);
    assert(scene_vga.indexed_scanout(1u,130u)==32u);
    assert(scene[scene_code+0x210u]==0x88u);
    // Original 36CEC uses strictly LESS THAN depth threshold.
    const auto edge=original_36cec_clip_run(0,3,256,10u,
        [&](unsigned x){return x==2?10u:1u;});
    assert(edge.x==0 && edge.width==2u);
    (void)addr;
}
