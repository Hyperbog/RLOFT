// Part 26: media-free source execution of the original scene and actor
// post-load control flow. The callbacks below are TEST INSTRUMENTATION;
// they do not draw pixels or replace the live game renderer.
#include "../../Code2/Code2.cpp"
#include "original_code2_video_boot.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u32=code2::u32;
using s32=code2::s32;
struct Trace {
    std::vector<std::string> calls;
    bool valid=true;
    u32 next_handle=0x1001u;
    void record(std::string name) {calls.push_back(std::move(name));}
    void check(bool condition) {if(!condition)valid=false;}
};
Trace* trace=nullptr;
code2::Machine* actor_machine=nullptr;
void require(bool good,const char* reason) {
    if(!good)throw std::runtime_error(reason);
}
void page(u32 p) {trace->check(p==0u);trace->record("291B0");}
void image(u32 y,u32 x,u32 resource,u32 slot) {
    const auto n=trace->calls.size();
    trace->check(x==0u && slot==0u &&
       ((n==1u&&y==0u&&resource==22u) ||
        (n==2u&&y==200u&&resource==23u)));
    trace->record("2E510");
}
void release(u32 resource,u32 slot) {
    const auto n=trace->calls.size();
    trace->check(slot==0u &&
        ((n==3u&&resource==22u)||(n==4u&&resource==23u)));
    trace->record("2EFD0");
}
u32 capture_source(u32 y,u32 x,u32 w,u32 h,u32 mask) {
    const auto n=trace->calls.size();
    trace->check(x==0x20u&&h==0x100u&&mask==0xFFu &&
        ((n==5u&&y==0u&&w==4u) ||
         (n==6u&&y==0x104u&&w==4u) ||
         (n==7u&&y==0x106u&&w==0x0Bu)));
    trace->record("30070");
    return trace->next_handle++;
}
void palette(u32 addr,u32 first,u32 mode,u32 filename,u32 count) {
    trace->check(addr==0xC054u&&first==0u&&mode==2u&&
                 filename==0x1128u&&count==0x100u);
    trace->record("325B0");
}
void dac(u32 addr) {trace->check(addr==0xC054u);trace->record("29234");}
void pointer(u32 p,s32 xy,u32 shape) {
    trace->check(p==0u&&xy==0&&shape==0u);trace->record("1A5B0");
}
void phase22d70() {trace->record("22D70");}
void phase2f868() {trace->record("2F868");}
void phase30b2c() {trace->record("30B2C");}

void test_original_compositor_dispatch() {
    std::vector<code2::u8> memory(code2::kObject2VirtualSize,0u);
    code2::Machine machine{};machine.data=memory.data();
    code2::store_le32(memory.data()+0x258DCu,15u);
    code2::store_le32(memory.data()+0xC034u,0u);
    memory[0xC050u]=0u;
    memory[0xD681u]=1u;
    Trace local{};local.calls.reserve(24u);trace=&local;
    code2::Application45Boundary b{};
    b.call_291b0=&page;
    b.call_2e510=&image;
    b.call_2efd0=&release;
    b.call_30070=&capture_source;
    b.call_325b0=&palette;
    b.call_29234=&dac;
    b.call_1a5b0=&pointer;
    b.call_22d70=&phase22d70;
    b.call_2f868=&phase2f868;
    b.call_30b2c=&phase30b2c;
    code2::function_1f8e0(machine,b,2u);
    const std::vector<std::string> expected={
        "291B0","2E510","2E510","2EFD0","2EFD0",
        "30070","30070","30070","325B0","29234",
        "1A5B0","22D70","2F868","30B2C"};
    require(local.valid&&local.calls==expected,
            "1F8FC source compositor callback arguments/order changed");
    require(code2::load_le32(memory.data()+0xC034u)==2u &&
            memory[0xC050u]==1u && memory[0xCF58u]==0u &&
            code2::load_le32(memory.data()+0xBF48u)==0x1001u &&
            code2::load_le32(memory.data()+0xBF4Cu)==0x1002u &&
            code2::load_le32(memory.data()+0x2222Cu)==0x1003u &&
            memory[0xD681u]==2u &&
            code2::load_le32(memory.data()+0xD8E0u)==0xFFFFFFFFu &&
            code2::load_le32(memory.data()+0xD8E4u)==0xFFFFFFFFu,
            "1F8FC source scene bookkeeping or page flip differs");
    local.calls.clear();
    // The source 1F8E0 JE returns without 1F8FC for unchanged art mode.
    code2::function_1f8e0(machine,b,2u);
    require(local.calls.empty()&&memory[0xCF58u]==1u&&
            memory[0xC050u]==1u,
            "1F8E0 unchanged-mode early return differs");
    trace=nullptr;
    std::cout<<"ORIGINAL_CODE2_1F8FC_SYNTHETIC_SOURCE_DISPATCH_PASS"
             <<" frames=1 resource_requests=2 page_flips=1"
             <<" callback_order=SOURCE"
             <<" image_pixels=NOT_RENDERED"
             <<" persistent_guest=NOT_CONNECTED\n";
}
u32 quest_bit(u32 bit) {return code2::function_1c1dc(*actor_machine,bit);}
u32 actor_kind(u32 type) {return code2::function_2acdc(type);}
// Actual 30070 + 292BC + 2A114 read-plane capture, tested against
// independently generated VGA pixels and unmodified original raster I/O.
// Synthetic allocator provides only storage; CODE.2 owns all metadata.
u32 capture_heap_cursor=0u;
u32 capture_allocate(u32 n) noexcept {
    const u32 ptr=capture_heap_cursor;
    capture_heap_cursor+=(n+3u)&~3u;
    return ptr;
}
void test_original_source_vga_capture() {
    std::vector<code2::u8> memory(0x50000u,0u);
    code2::Machine machine{};machine.data=memory.data();
    rloft::graphics::OriginalVgaHost video{};
    rloft::source_video_boot::Bridge bridge(machine,video,memory.size());
    bridge.run_original_290f4(3u);
    require(code2::load_le16(memory.data()+0xEF3Fu)==400u &&
            code2::load_le16(memory.data()+0xEF41u)==320u,
            "Original 290F4 source geometry unavailable");
    for(u32 row=0u;row<274u;++row)
        for(u32 x=32u;x<288u;++x)
            require(video.write_direct_pixel(x,row,
                      static_cast<code2::u8>((row*31u+x*17u)&255u)),
                    "Generated VGA capture witness pixel failed");
    capture_heap_cursor=0x31000u;
    const struct {u32 row;u32 rows;} plans[]={{0u,4u},{0x104u,4u},{0x106u,11u}};
    for(const auto& p:plans) {
        const u32 image=bridge.run_original_30070_capture(
            p.row,0x20u,p.rows,0x100u,0xFFu,&capture_allocate);
        require(image>=0x31000u &&
                code2::load_le16(memory.data()+image)==p.rows*0x100u &&
                code2::load_le16(memory.data()+image+2u)==1u &&
                code2::load_le16(memory.data()+image+4u)==p.rows &&
                memory[image+6u]==32u && memory[image+7u]==0u,
                "CODE2 30070 allocated wrong source image metadata");
        const u32 per_row=64u;
        const u32 payload=image+8u;
        // 2A114 writes all four VGA planes, each in row-major byte
        // order; compare every byte against independently authored
        // pixel values, not against the source buffer itself.
        for(u32 plane=0u;plane<4u;++plane)
            for(u32 row=0u;row<p.rows;++row)
                for(u32 x=0u;x<per_row;++x) {
                    const u32 screen_x=32u+x*4u+plane;
                    const u32 screen_row=p.row+row;
                    const u32 expected=(screen_row*31u+screen_x*17u)&255u;
                    const u32 actual=memory[payload+plane*p.rows*per_row+
                                            row*per_row+x];
                    require(actual==expected,
                            "30070 2A114 source bytes disagree with VGA plane pixels");
                }
    }
    require(capture_heap_cursor>0x31000u &&
            !video.unsupported_read(),
            "Original capture requested unsupported VGA read");
    std::cout<<"ORIGINAL_CODE2_30070_292BC_2A114_SOURCE_VGA_CAPTURE_PASS"
             <<" capture_rectangles=3"
             <<" original_source_pixels=ALL_PLANES_MATCH"
             <<" original_source_heap_metadata=YES"
             <<" frame_mode=ISOLATED_SYNTHETIC"
             <<" live_scene=NOT_CONNECTED\\n";
}
// Original CODE.2 1A5B0 no-mouse branch: scanout -> push output page
// -> source 2A020 copy all four planar pages -> restore output page.
// Source video registers and image bytes are checked separately.
struct SourcePresentProbe {
    code2::Machine* machine{};
    rloft::source_video_boot::Bridge* bridge{};
    rloft::graphics::OriginalVgaHost* video{};
    u32 scanout=0xFFFFFFFFu, copied=0xFFFFFFFFu;
    u32 pushes=0u,pops=0u;
    static SourcePresentProbe*& active() {
        static thread_local SourcePresentProbe* p=nullptr;return p;
    }
    static void scanout_page(u32 page) noexcept {
        active()->scanout=page;
        try {active()->bridge->run_original_291c8(page);}
        catch(...) {std::terminate();}
    }
    static void push(u32 page) noexcept {
        try {active()->bridge->run_original_2e590(page);++active()->pushes;}
        catch(...) {std::terminate();}
    }
    static void pop() noexcept {
        try {active()->bridge->run_original_2e5d4();++active()->pops;}
        catch(...) {std::terminate();}
    }
    static void copy(u32 page) noexcept {
        active()->copied=page;
        try {active()->bridge->run_original_2a020_page_copy(page);}
        catch(...) {std::terminate();}
    }
    u32 captures=0u,restores=0u,draws=0u,guest_copies=0u;
    static void cursor_capture(u32 dst,s32 x,s32 y,u32 w,u32 h) noexcept {
        try {
            active()->bridge->run_original_292bc_cursor_capture(dst,x,y,w,h);
            ++active()->captures;
        }catch(...) {std::terminate();}
    }
    static void cursor_restore(u32 src,s32 x,s32 y,u32 w,u32 h) noexcept {
        try {
            active()->bridge->run_original_2932c_cursor_restore(src,x,y,w,h);
            ++active()->restores;
        }catch(...) {std::terminate();}
    }
    static void cursor_draw(u32 frame,s32 x,s32 y,u32 image) noexcept {
        try {
            active()->bridge->run_original_29804_cursor_sprite(frame,x,y,image);
            ++active()->draws;
        }catch(...) {std::terminate();}
    }
    static u32 cursor_hit(s32,s32) noexcept {return 0u;}
    static void memory_copy(u32 dst,u32 src,u32 n) noexcept {
        if(n!=0x400u || (dst!=0x9E3Au&&dst!=0xA23Au) ||
           (src!=0x9E3Au&&src!=0xA23Au))std::terminate();
        (void)code2::function_30cd4(*active()->machine,dst,src,n);
        ++active()->guest_copies;
    }
};
void test_original_source_present_fallback() {
    std::vector<code2::u8> mem(0x50000u,0u);
    code2::Machine machine{};machine.data=mem.data();
    rloft::graphics::OriginalVgaHost vga;
    rloft::source_video_boot::Bridge bridge(machine,vga,mem.size());
    bridge.run_original_290f4(3u);
    for(u32 y=0u;y<400u;y+=13u)
        for(u32 x=0u;x<320u;x+=7u)
            require(vga.write_direct_pixel(x,y,
              static_cast<code2::u8>((x*11u+y*19u+1u)&255u)),
               "VGA initial frame pixel failed");
    SourcePresentProbe probe{&machine,&bridge,&vga};
    SourcePresentProbe::active()=&probe;
    code2::Application33Boundary video{};
    video.call_291c8=&SourcePresentProbe::scanout_page;
    video.call_2a020=&SourcePresentProbe::copy;
    code2::Application34Boundary mouse{};
    mouse.app33=&video;
    code2::Application35Boundary compositor{};
    compositor.app34=&mouse;
    compositor.call_2e590=&SourcePresentProbe::push;
    compositor.call_2e5d4=&SourcePresentProbe::pop;
    require(code2::load_le16(mem.data()+0x85FCu)==0u,
            "1A5B0 isolated fallback requires mouse-disabled original branch");
    code2::function_1a5b0(machine,compositor,0u,0xFFFFFFFFu,0u);
    require(probe.scanout==0u && probe.copied==1u &&
            probe.pushes==1u && probe.pops==1u &&
            mem[0xF0D0u]==0u && mem[0xEF4Du]==0u &&
            mem[0xEF4Eu]==0u && mem[0x9E39u]==1u,
            "Original 1A5B0 did not execute page copy/restore order");
    bridge.run_original_291c8(1u);
    for(u32 y=0u;y<400u;y+=13u)
        for(u32 x=0u;x<320u;x+=7u)
            require(vga.indexed_scanout(x,y)==
                   static_cast<code2::u8>((x*11u+y*19u+1u)&255u),
                    "2A020 latch-mode page copy differs from expected pixel");
    SourcePresentProbe::active()=nullptr;
    std::cout<<"ORIGINAL_CODE2_1A5B0_2A020_VGA_PRESENT_PASS"
             <<" branch=MOUSE_ABSENT_SYNTHETIC"
             <<" original_page_copy=FULL_PLANES"
             <<" source_scanout=291C8"
             <<" original_page_stack=2E590_2E5D4"
             <<" retained_gameplay=NOT_CONNECTED\\n";
}
// Original 1F494 is the party HUD dispatcher. Exercise a concrete
// source-backed status branch with the original actor flags and array
// strides. The callbacks here OBSERVE requests only; their existence
// never constitutes RES0 portrait pixel parity.
struct SceneEntryProbe {
    static SceneEntryProbe*& active() {
        static thread_local SceneEntryProbe* p=nullptr;return p;
    }
    std::vector<std::string> calls{};
    bool valid=true;
    static void page(u32 p) {
        active()->valid &= p==0u;
        active()->calls.push_back("291B0");
    }
    static void hud_image(u32 y,u32 x,u32 res,u32 archive) {
        auto* self=active();
        const u32 i=static_cast<u32>(self->calls.size()-1u);
        const u32 expected_x=(i%2u)*0x35u+(i/2u)*0xD3u+2u;
        self->valid &= y==0x113u && x==expected_x &&
                       res==35u && archive==0u && i<4u;
        self->calls.push_back("HUD_RES0");
    }
    static void ray() {active()->calls.push_back("334C4_RAY_REQUEST");}
    static void background(u32 p) {
        active()->valid &= p==3u;
        active()->calls.push_back("33E28_REQUEST");
    }
    static void ground(u32 p) {
        active()->valid &= p==4u;
        active()->calls.push_back("33DE0_REQUEST");
    }
    static void prepare() {active()->calls.push_back("34DE8_REQUEST");}
    static void actors() {active()->calls.push_back("1BDAC_REQUEST");}
    static void scene() {active()->calls.push_back("2C294_REQUEST");}
    static void effects() {active()->calls.push_back("274A4_REQUEST");}
    static void resource() {active()->calls.push_back("2F868_REQUEST");}
};
// Authentic active-pointer 1A5B0 source branch: same code2 source
// cursor capture/raster, 2A020 VGA page copy, then 2932C/2A178
// restore on the alternate page. Cursor image bytes are a *synthetic*
// test pattern (not the original RES0 cursor). This tests original
// presentation machinery, not DOS screenshot parity.
void test_original_source_present_active_mouse() {
    std::vector<code2::u8> mem(0x50000u,0u);
    code2::Machine machine{};machine.data=mem.data();
    rloft::graphics::OriginalVgaHost vga;
    rloft::source_video_boot::Bridge bridge(machine,vga,mem.size());
    bridge.run_original_290f4(3u);
    for(u32 y=0u;y<400u;++y)
        for(u32 x=0u;x<320u;++x)
            require(vga.write_direct_pixel(x,y,19u),
                    "Active cursor frame source VGA priming failed");
    const u32 sprite=0x880Du;
    code2::store_le16(mem.data()+sprite,256u);
    code2::store_le16(mem.data()+sprite+2u,11u);
    code2::store_le16(mem.data()+sprite+4u,16u);
    mem[sprite+6u]=2u;
    mem[sprite+7u]=0u;
    std::fill(mem.begin()+sprite+8u,
              mem.begin()+sprite+8u+11u*256u,77u);
    code2::store_le16(mem.data()+0x85FCu,0xFFFFu);
    code2::store_le16(mem.data()+0x9E37u,1u);
    code2::store_le16(mem.data()+0x9E31u,200u);
    code2::store_le16(mem.data()+0x9E33u,160u);
    code2::store_le16(mem.data()+0xA6B4u,200u);
    code2::store_le16(mem.data()+0xA6B6u,160u);
    code2::store_le16(mem.data()+0x85FEu,0xFFFFu);
    code2::store_le16(mem.data()+0x8600u,0xFFFFu);
    code2::store_le16(mem.data()+0xA6C0u,1u);
    SourcePresentProbe probe{&machine,&bridge,&vga};
    SourcePresentProbe::active()=&probe;
    code2::Application33Boundary video{};
    video.call_291c8=&SourcePresentProbe::scanout_page;
    video.call_2a020=&SourcePresentProbe::copy;
    video.call_30cd4=&SourcePresentProbe::memory_copy;
    code2::Application34Boundary mouse{};
    mouse.app33=&video;
    code2::Application35Boundary compositor{};
    compositor.app34=&mouse;
    compositor.call_2e590=&SourcePresentProbe::push;
    compositor.call_2e5d4=&SourcePresentProbe::pop;
    compositor.call_292bc=&SourcePresentProbe::cursor_capture;
    compositor.call_2932c=&SourcePresentProbe::cursor_restore;
    compositor.call_2ae30=&SourcePresentProbe::cursor_hit;
    compositor.call_29804=&SourcePresentProbe::cursor_draw;
    code2::function_1a5b0(machine,compositor,0u,0xFFFFFFFFu,0u);
    require(probe.scanout==0u && probe.copied==1u &&
            probe.captures==1u && probe.draws==1u &&
            probe.restores==1u && probe.guest_copies==1u &&
            probe.pushes==3u && probe.pops==3u &&
            mem[0xF0D0u]==0u && mem[0xEF4Du]==0u &&
            mem[0xEF4Eu]==0u && mem[0x9E39u]==0u &&
            code2::load_le16(mem.data()+0x9E37u)==1u,
            "Original 1A5B0 active cursor presentation source contract changed");
    require(vga.indexed_scanout(160u,200u)==77u,
            "Original active pointer not rasterized to active page");
    bridge.run_original_291c8(1u);
    require(vga.indexed_scanout(160u,200u)==19u,
            "Original 2932C/2A178 did not restore alternate cursor background");
    SourcePresentProbe::active()=nullptr;
    std::cout<<"ORIGINAL_CODE2_1A5B0_MOUSE_ACTIVE_VGA_PRESENT_PASS"
             <<" source=1A288_29804_2A020_2932C_2A178"
             <<" mouse_image=SYNTHETIC_TEST_FIXTURE"
             <<" pixel_copy_and_restore=SOURCE"
             <<" live_gameplay=NOT_CONNECTED\\n";
}
void test_original_source_hud_and_world_entry() {
    std::vector<code2::u8> mem(code2::kObject2VirtualSize,0u);
    code2::Machine machine{};machine.data=mem.data();
    code2::store_le32(mem.data()+0x258DCu,15u);
    code2::store_le32(mem.data()+0xC034u,4u);
    mem[0xC050u]=0u;
    for(u32 slot=0u;slot<4u;++slot) {
        mem[0xCF5Au+slot]=1u;
        code2::store_le16(mem.data()+0x218F0u+slot*0x248u,0x8000u);
    }
    SceneEntryProbe check{};
    SceneEntryProbe::active()=&check;
    code2::Application44Boundary hud{};
    hud.call_291b0=&SceneEntryProbe::page;
    hud.call_2e510=&SceneEntryProbe::hud_image;
    code2::function_1f494(machine,hud,0u);
    require(check.valid && check.calls==
            std::vector<std::string>({"291B0","HUD_RES0","HUD_RES0",
                                      "HUD_RES0","HUD_RES0"}),
            "Original 1F494 did not visit four authentic party HUD status images");
    for(u32 slot=0u;slot<4u;++slot)
        require(mem[0xCF5Au+slot]==0u,
                "Original 1F494 did not clear party redraw dirty bit");
    check.calls.clear();
    code2::store_le16(mem.data()+0x255D4u,0x1234u);
    code2::store_le16(mem.data()+0x255D0u,0x5678u);
    code2::store_le32(mem.data()+0x22220u,0x800u);
    mem[0x258A8u]=0xFFu;
    mem[0x258B0u]=3u;
    mem[0x258B1u]=4u;
    code2::Application44Boundary world{};
    world.call_291b0=&SceneEntryProbe::page;
    world.call_334c4=&SceneEntryProbe::ray;
    world.call_33e28=&SceneEntryProbe::background;
    world.call_33de0=&SceneEntryProbe::ground;
    world.call_34de8=&SceneEntryProbe::prepare;
    world.call_1bdac=&SceneEntryProbe::actors;
    world.call_2c294=&SceneEntryProbe::scene;
    world.call_274a4=&SceneEntryProbe::effects;
    world.call_2f868=&SceneEntryProbe::resource;
    code2::function_1f1c8(machine,world);
    const std::vector<std::string> expected={
        "291B0","334C4_RAY_REQUEST","33E28_REQUEST",
        "33DE0_REQUEST","34DE8_REQUEST","1BDAC_REQUEST",
        "2C294_REQUEST","274A4_REQUEST","2F868_REQUEST"};
    require(check.valid && check.calls==expected &&
            code2::load_le32(mem.data()+0xFDBCu)==0xA0000u &&
            code2::load_le32(mem.data()+0xC044u)==0x12u &&
            code2::load_le32(mem.data()+0xC048u)==0x56u &&
            code2::load_le32(mem.data()+0xFF50u)==0x80u,
            "Original 1F1C8 render-entry state/callback schedule changed");
    SceneEntryProbe::active()=nullptr;
    std::cout<<"ORIGINAL_CODE2_1F494_1F1C8_SOURCE_ENTRY_PASS"
             <<" party_redraws=4 source_1F3FC=SKIPPED_BY_RETAIL_HIGH_BIT"
             <<" world_entry=1EDAC_334C4_33E28_33DE0"
             <<" active_world_raster=NOT_CONNECTED"
             <<" party_portrait_pixels=NOT_RENDERED\\n";
}
void test_original_actor_postload() {
    std::vector<code2::u8> memory(code2::kObject2VirtualSize,0u);
    code2::Machine machine{};machine.data=memory.data();
    actor_machine=&machine;
    code2::Application31Boundary b{};
    b.call_1c1dc=&quest_bit;
    b.call_2acdc=&actor_kind;
    // Party characters have no active status bits in this synthetic test.
    // No random, dialog or gameplay callback is silently supplied.
    code2::store_le32(memory.data()+0xC6C8u,22u);
    code2::store_le16(memory.data()+0x258A2u,1u);
    constexpr u32 actor=0x14D80u;
    code2::store_le16(memory.data()+actor+0x0Eu,9u);
    code2::store_le16(memory.data()+actor+0x12u,0u);
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)!=0u,
            "17A68 did not set original nonpositive-health actor mark");
    code2::store_le16(memory.data()+actor+0x12u,10u);
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)==0u,
            "17A68 did not clear original actor mark on re-entry");
    // Original type 8 obeys quest bit 3A rather than host fiction.
    code2::store_le16(memory.data()+actor+0x0Eu,8u);
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)!=0u,
            "17A68 quest-unset original type8 case changed");
    code2::function_1c124(machine,0x3Au);
    require(code2::function_1c1dc(machine,0x3Au)!=0u,
            "1C124 original quest bit was not stored");
    code2::function_17a68(machine,b);
    require((memory[actor+0x0Bu]&2u)==0u,
            "17A68 quest-set original type8 case changed");
    actor_machine=nullptr;
    std::cout<<"ORIGINAL_CODE2_17A68_SYNTHETIC_ACTOR_POSTLOAD_PASS"
             <<" original_17838=EXECUTED"
             <<" flags=SOURCE_1C1DC"
             <<" type_predicate=SOURCE_2ACDC"
             <<" actor_AI=NOT_CONNECTED\n";
}
}
int main() {
    try {
        test_original_compositor_dispatch();
        test_original_source_vga_capture();
        test_original_source_present_fallback();
        test_original_source_present_active_mouse();
        test_original_source_hud_and_world_entry();
        test_original_actor_postload();
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"ORIGINAL_CODE2_1F8FC_17A68_SOURCE_FAIL: "
                 <<e.what()<<"\n";
        return 1;
    }
}
