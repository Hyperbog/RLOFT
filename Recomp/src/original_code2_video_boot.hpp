#pragma once
// Original CODE.2 290F4 source video initialization: execute the certified
// canonical functions 290F4, 29930, 2AB98, 2ABF4, 2AC10, 29FEC,
// 29FFC, 2A104 and 296C0 on ONE retained guest Machine and ONE VGA host.
// Include the canonical Code2.cpp translation unit BEFORE this header.
//
// Video is a bounded in-memory VGA device. No Windows display, DOS BIOS,
// gameplay frame or actual user's GPU is asserted here.
#include "original_vga_host.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace rloft { namespace source_video_boot {
using u8=std::uint8_t;
using u16=std::uint16_t;
using u32=std::uint32_t;
struct Bridge {
    code2::Machine& machine;
    rloft::graphics::OriginalVgaHost& video;
    std::size_t guest_extent;
    bool invalid_access=false;
    u32 port_writes=0;
    u32 bios_changes=0;
    u32 source_mode_calls=0;
    u32 source_palette_stream_calls=0;
    u32 source_page_push_calls=0;
    u32 source_page_pop_calls=0;
    u32 source_page_select_calls=0;
    u32 source_rectangle_fill_calls=0;
    u32 source_cursor_background_captures=0;
    u32 source_frame_background_captures=0;
    u32 source_cursor_sprite_draws=0;
    u32 source_cursor_clipped_sprite_draws=0;
    explicit Bridge(code2::Machine& guest,
                    rloft::graphics::OriginalVgaHost& v,
                    std::size_t memory_bytes)
        :machine(guest),video(v),guest_extent(memory_bytes) {
        if(!machine.data || guest_extent<0x30000u)
            throw std::invalid_argument("CODE2 VGA requires initialized guest DGROUP");
    }
    static Bridge*& current() noexcept {
        static thread_local Bridge* ptr=nullptr;
        return ptr;
    }
    struct Scope {
        Bridge* previous;
        explicit Scope(Bridge& b) :previous(current()) {
            if(previous)throw std::logic_error("Nested CODE2 VGA source bridge");
            current()=&b;
        }
        ~Scope(){current()=previous;}
    };
    static Bridge& self() noexcept {return *current();}
    static u8 guest_read(u32 p) noexcept {
        auto& b=self();
        if(p>=b.guest_extent){b.invalid_access=true;return 0;}
        return b.machine.data[p];
    }
    static void guest_write(u32 p,u8 value) noexcept {
        auto& b=self();
        if(p>=b.guest_extent){b.invalid_access=true;return;}
        b.machine.data[p]=value;
    }
    static void guest_write16(u32 p,u16 value) noexcept {
        guest_write(p,static_cast<u8>(value));
        guest_write(p+1u,static_cast<u8>(value>>8u));
    }
    static u8 in8(u16 p) noexcept {return self().video.read_port8(p);}
    static void out8(u16 p,u8 value) noexcept {
        auto& b=self();
        if(!b.video.write_port8(p,value))b.invalid_access=true;
        else ++b.port_writes;
    }
    static void out16(u16 port,u16 value) noexcept {
        auto& b=self();
        if(!b.video.write_indexed_port(port,value))b.invalid_access=true;
        else b.port_writes+=2u;
    }
    static u8 vga_read(u32 addr) noexcept {
        auto& b=self();
        if(addr<0xA0000u || addr>=0xB0000u){
            b.invalid_access=true;return 0;
        }
        return b.video.read_aperture(static_cast<u16>(addr-0xA0000u));
    }
    static void vga_write(u32 addr,u8 value) noexcept {
        auto& b=self();
        if(addr<0xA0000u || addr>=0xB0000u ||
           !b.video.write_aperture(static_cast<u16>(addr-0xA0000u),value))
            b.invalid_access=true;
    }
    static u8 get_mode() noexcept {return self().video.bios_mode();}
    static void set_mode(u8 mode) noexcept {
        auto& b=self();b.video.bios_set_mode(mode);++b.bios_changes;
    }
    static code2::Application61Boundary app61() noexcept {
        code2::Application61Boundary b{};
        b.in8=&in8;b.out8=&out8;b.out16=&out16;
        b.vga_read8=&vga_read;b.vga_write8=&vga_write;
        return b;
    }
    static void source_clear_planes() noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=2AB98_BEGIN\n"<<std::flush;
        auto b=app61();
        code2::function_2ab98(self().machine,b);
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=2AB98_END\n"<<std::flush;
    }
    static void source_set_crtc_start(u32 position) noexcept {
        auto b=app61();
        code2::function_2abf4(b,position);
    }
    static void source_line_compare(u32 count) noexcept {
        auto b=app61();
        code2::function_2ac10(self().machine,b,count);
    }
    static u32 source_29a84() noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=29A84_BEGIN\n"<<std::flush;
        auto gfx=graphics();
        code2::function_29a84(gfx);
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=29A84_END\n"<<std::flush;
        return 0;
    }
    static u32 source_29930(u32 mode,u32 descriptor) noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=29930_ENTRY mode="<<mode<<" desc="<<descriptor<<"\n"<<std::flush;
        auto b=graphics();
        return code2::function_29930(self().machine,b,mode,descriptor);
    }
    static u32 source_2a104(u32 count) noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=2A104_ENTRY count="<<count<<"\n"<<std::flush;
        code2::Application59Boundary b{};
        b.call_2ac10=&source_line_compare;
        code2::function_2a104(b,count);
        return 0;
    }
    static u32 source_29fec(u32 page) noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=29FEC_ENTRY page="<<page<<"\n"<<std::flush;
        return code2::function_29fec(self().machine,page);
    }
    static u32 source_29ffc(u32 page) noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=29FFC_ENTRY page="<<page<<"\n"<<std::flush;
        code2::Application59Boundary b{};
        b.call_2abf4=&source_set_crtc_start;
        code2::function_29ffc(self().machine,b,page);
        return 0;
    }
    static u32 source_296c0(u32 y0,u32 x0,u32 y1,u32 x1) noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=296C0_ENTRY\n"<<std::flush;
        code2::function_296c0(self().machine,
            static_cast<code2::s32>(y0),static_cast<code2::s32>(x0),
            static_cast<code2::s32>(y1),static_cast<code2::s32>(x1));
        return 0;
    }
    static void retrace() noexcept {
        // Same deterministic retrace latch that the source 2ABC4
        // reads via the bounded original VGA host.
        auto b=app61();
        code2::function_2abc4(b);
    }
    static code2::Graphics2Boundary graphics() noexcept {
        code2::Graphics2Boundary b{};
        b.read8=&guest_read;
        b.write8=&guest_write;
        b.write16=&guest_write16;
        b.in8=&in8;
        b.out8=&out8;
        b.out16=&out16;
        b.sync_2ab98=&source_clear_planes;
        b.sync_2abc4=[](u32){retrace();};
        b.call_2abf4=&source_set_crtc_start;
        b.call_2ac10=&source_line_compare;
        b.bios_get_mode=&get_mode;
        b.bios_set_mode=&set_mode;
        return b;
    }
    static u32 source_29ca0(u32 src,u32 first,u32 count) noexcept {
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=29CA0_ENTRY count="<<count<<"\n"<<std::flush;
        ++self().source_palette_stream_calls;
        auto gfx=graphics();
        code2::function_29ca0(gfx,src,first,count);
        return 0;
    }
    static code2::Application56Boundary app56() noexcept {
        code2::Application56Boundary b{};
        b.call_29a84=&source_29a84;
        b.call_29930=&source_29930;
        b.call_2a104=&source_2a104;
        b.call_29fec=&source_29fec;
        b.call_29ffc=&source_29ffc;
        b.call_296c0=&source_296c0;
        b.call_29ca0=&source_29ca0;
        return b;
    }
    // Original 20294 is a page-stack push, two source 29B40 planar
    // rectangles, and a page-stack pop. Do not replace with a memset
    // or a host-only acknowledgement: this path must keep both the
    // source memory globals and real VGA plane registers synchronized.
    static code2::Application58Boundary app58() noexcept {
        code2::Application58Boundary b{};
        b.in8=&in8;
        b.out8=&out8;
        b.out16=&out16;
        b.vga_read8=&vga_read;
        b.vga_write8=&vga_write;
        return b;
    }
    static void source_page_select(u32 page) noexcept {
        auto cb=app56();
        code2::function_291b0(self().machine,cb,page);
        ++self().source_page_select_calls;
    }
    static void source_page_stack_error(u32) noexcept {
        self().invalid_access=true; // original 2F580 error branch
    }
    static code2::Application67Boundary app67() noexcept {
        code2::Application67Boundary b{};
        b.call_291b0=&source_page_select;
        b.call_2f580=&source_page_stack_error;
        return b;
    }
    static void source_page_push(u32 page) noexcept {
        const u8 depth=self().machine.data[0xF0D0u];
        if(depth>=10u){self().invalid_access=true;return;}
        auto cb=app67();
        code2::function_2e590(self().machine,cb,page);
        ++self().source_page_push_calls;
    }
    static void source_page_pop() noexcept {
        if(self().machine.data[0xF0D0u]==0u) {
            self().invalid_access=true;return;
        }
        auto cb=app67();
        code2::function_2e5d4(self().machine,cb);
        ++self().source_page_pop_calls;
    }
    static void source_planar_rectangle(u32 row,u32 x,u32 rows,
                                        u32 pixels,u32 color) noexcept {
        // Source 29B40 expects row, horizontal offset, number of
        // rows, horizontal pixels and VGA color in that order.
        auto cb=app58();
        code2::function_29b40(self().machine,cb,
            static_cast<code2::s32>(row),static_cast<code2::s32>(x),
            static_cast<code2::s32>(rows),
            static_cast<code2::s32>(pixels),color);
        ++self().source_rectangle_fill_calls;
    }
    void run_original_2e590(u32 page) {
        if(page>1u || machine.data[0xF0D0u]>=10u)
            throw std::invalid_argument("Original 2E590 page-stack push bounds");
        Scope scope(*this);
        source_page_push(page);
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 2E590 page push VGA failure");
    }
    void run_original_291b0(u32 page) {
        if(page>1u)throw std::invalid_argument("Original 291B0 page outside double buffer");
        Scope scope(*this);
        source_page_select(page);
        if(invalid_access || video.unsupported_read() ||
           machine.data[0xEF4Du]!=page)
            throw std::runtime_error("Original 291B0 source page selection failed");
    }
    // Original 291C8 changes the display CRTC start through 29FFC and
    // independently records the selected scanout page in EF4E.
    void run_original_291c8(u32 page) {
        if(page>1u)
            throw std::invalid_argument("Original 291C8 scanout page outside double buffer");
        Scope scope(*this);
        auto callbacks=app56();
        code2::function_291c8(machine,callbacks,page);
        if(invalid_access || video.unsupported_read() ||
           machine.data[0xEF4Eu]!=page)
            throw std::runtime_error("Original 291C8 source scanout selection failed");
    }
    // Original 1A288 -> 292BC -> 2A114 snapshots planar VGA bytes
    // before drawing a cursor. Preserve VGA read-map sequencing.
    static u32 source_cursor_2a114(u32 dst,u32 row,u32 x,u32 rows,u32 pixels) noexcept {
        auto& b=self();
        // Original 292BC forwards (x,y,width,height) as the 2A114
        // (row,column,row_count,pixel_count) argument order: the cursor
        // 32x16 rectangle arrives as 32 source rows of 16 pixels.
        if(rows!=32u || pixels!=16u || row>368u || x>304u ||
           dst<0x9E3Au || std::uint64_t(dst)+768u>b.guest_extent) {
            b.invalid_access=true;return 0u;
        }
        code2::Application60Boundary io{};
        io.out16=&out16;
        io.vga_read8=&vga_read;
        io.vga_write8=&vga_write;
        code2::function_2a114(b.machine,io,dst,row,x,rows,pixels);
        ++b.source_cursor_background_captures;
        return 1u;
    }
    void run_original_292bc_cursor_capture(u32 dst,code2::s32 x,
                                            code2::s32 y,u32 w,u32 h) {
        if((dst!=0x9E3Au && dst!=0xA23Au) || w!=32u || h!=16u)
            throw std::invalid_argument("Original 1A288 cursor rectangle changed");
        Scope scope(*this);
        auto cb=app56();
        cb.call_2a114=&source_cursor_2a114;
        code2::function_292bc(machine,cb,dst,x,y,
                               static_cast<code2::s32>(w),
                               static_cast<code2::s32>(h));
        if(invalid_access || video.unsupported_read() ||
           source_cursor_background_captures!=1u)
            throw std::runtime_error("Original 292BC/2A114 cursor snapshot failed");
    }
    // Generic original CODE.2 30070 -> 292BC -> 2A114 capture.
    // Source storage is allocated through the *guest's* 3104C;
    // original 2A114 reads VGA planes in plane-major sequence.
    static u32 source_capture_2a114(u32 dst,u32 row,u32 x,
                                     u32 rows,u32 pixels) noexcept {
        auto& b=self();
        if(rows==0u || pixels==0u || row>=400u || x>=320u ||
           rows>400u-row || pixels>320u-x) {
            b.invalid_access=true;return 0u;
        }
        const u32 plane=x&3u;
        const u32 bytes=(pixels+plane+3u)>>2u;
        const std::uint64_t length=std::uint64_t(rows)*bytes*4u;
        if(dst>=b.guest_extent || length>b.guest_extent-dst) {
            b.invalid_access=true;return 0u;
        }
        code2::Application60Boundary io{};
        io.out16=&out16;
        io.vga_read8=&vga_read;
        io.vga_write8=&vga_write;
        code2::function_2a114(b.machine,io,dst,row,x,rows,pixels);
        ++b.source_frame_background_captures;
        return 1u;
    }
    u32 run_original_30070_capture(u32 row,u32 x,u32 rows,u32 pixels,
                                     u32 tag,u32 (*allocate)(u32)) {
        if(!allocate || rows==0u || pixels==0u ||
           row>=400u || x>=320u || rows>400u-row ||
           pixels>320u-x || (x&3u)!=0u ||
           (pixels&7u)!=0u ||
           rows>0xFFFFu || pixels>0x7F8u || tag>255u)
            throw std::invalid_argument("Original 30070 VGA rectangle invalid");
        // CODE.2 computes a 16-bit allocation length after 2A1E8.
        const u32 expected=code2::function_2a1e8(
            1u,rows,pixels/8u,8u,tag);
        if(expected==0u || expected>0xFFFFu-8u)
            throw std::out_of_range("Original 30070 image exceeds source word allocation");
        Scope scope(*this);
        const u32 captures_before=source_frame_background_captures;
        code2::Application56Boundary video_callbacks{};
        video_callbacks.call_2a114=&source_capture_2a114;
        code2::Application70Boundary resource{};
        resource.app56=&video_callbacks;
        resource.call_3104c=allocate;
        const u32 image=code2::function_30070(
            machine,resource,static_cast<code2::s32>(row),
            static_cast<code2::s32>(x),static_cast<code2::s32>(rows),
            static_cast<code2::s32>(pixels),tag);
        if(invalid_access || video.unsupported_read() ||
           source_frame_background_captures!=captures_before+1u ||
           image==0u || std::uint64_t(image)+expected+8u>guest_extent ||
           code2::load_le16(machine.data+image)!=expected ||
           code2::load_le16(machine.data+image+2u)!=1u ||
           code2::load_le16(machine.data+image+4u)!=rows ||
           machine.data[image+6u]!=pixels/8u ||
           machine.data[image+7u]!=(tag==0xFFu?0u:0x80u)) {
            std::cerr<<"CODE2_30070_CAPTURE_DIAGNOSTIC invalid_access="<<invalid_access
                     <<" unsupported_read="<<video.unsupported_read()
                     <<" calls="<<source_frame_background_captures
                     <<" captures_before="<<captures_before
                     <<" image="<<image<<" expected="<<expected
                     <<" extent="<<guest_extent
                     <<" stride="<<code2::load_le16(machine.data+image)
                     <<" frames="<<code2::load_le16(machine.data+image+2u)
                     <<" rows="<<code2::load_le16(machine.data+image+4u)
                     <<" units="<<unsigned(machine.data[image+6u])
                     <<" flags="<<unsigned(machine.data[image+7u])
                     <<" requested_row="<<row<<" requested_x="<<x
                     <<" requested_rows="<<rows<<" requested_pixels="<<pixels
                     <<" tag="<<tag<<"\\n";
            throw std::runtime_error("Original 30070 VGA plane capture incomplete");
        }
        return image;
    }
    // Retail 1A5B0 -> 2A020 uses VGA GC write mode 1: each
    // aperture read captures the four plane latches and the subsequent
    // write copies the same four bytes to the other original page.
    // This is not a host-side RGBA/bitmap copy.
    void run_original_2a020_page_copy(u32 target_page) {
        if(target_page>1u || !video.bios_mode() ||
           machine.data[0xEF4Du]>1u ||
           (target_page==machine.data[0xEF4Du]))
            throw std::invalid_argument("Original 2A020 page copy invalid source/destination");
        Scope scope(*this);
        code2::Application59Boundary cb{};
        cb.out16=&out16;
        cb.read8=&vga_read;
        cb.write8=&vga_write;
        code2::function_2a020(machine,cb,target_page);
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 2A020 VGA plane/latch copy unsupported");
    }
    // The cursor background restore uses original 2932C clipped
    // dispatch and original 2A178 VGA sequencer plane writes.
    static u32 source_cursor_restore_2a178(
        u32 src,u32 row,u32 x,u32 rows,u32 pixels) noexcept {
        auto& b=self();
        if((src!=0x9E3Au && src!=0xA23Au) ||
           rows!=32u || pixels!=16u ||
           row>368u || x>304u ||
           std::uint64_t(src)+768u>b.guest_extent) {
            b.invalid_access=true;return 0u;
        }
        code2::Application60Boundary io{};
        io.out16=&out16;
        io.vga_read8=&vga_read;
        io.vga_write8=&vga_write;
        code2::function_2a178(b.machine,io,src,row,x,rows,pixels);
        return 1u;
    }
    void run_original_2932c_cursor_restore(
        u32 src,code2::s32 x,code2::s32 y,u32 w,u32 h) {
        if((src!=0x9E3Au && src!=0xA23Au) || w!=32u || h!=16u)
            throw std::invalid_argument("Original 2932C cursor restore ABI changed");
        Scope scope(*this);
        code2::Application56Boundary cb{};
        cb.call_2a178=&source_cursor_restore_2a178;
        code2::function_2932c(machine,cb,src,x,y,
                               static_cast<code2::s32>(w),
                               static_cast<code2::s32>(h));
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 2932C/2A178 cursor restore failed");
    }
    // Original cursor image rasterization: CODE2 29804->29414->2A380
    // shares the same physical VGA aperture as the source 2A114
    // background capture. Ordinary guest RAM and VGA are distinct.
    static bool cursor_aperture(u32 address) noexcept {
        return address>=0xA0000u && address<0xB0000u;
    }
    static u8 cursor_read8(u32 address) noexcept {
        return cursor_aperture(address)?vga_read(address):guest_read(address);
    }
    static u16 cursor_read16(u32 address) noexcept {
        return static_cast<u16>(cursor_read8(address) |
                                  (static_cast<u16>(cursor_read8(address+1u))<<8u));
    }
    static void cursor_write8(u32 address,u8 value) noexcept {
        if(cursor_aperture(address))vga_write(address,value);
        else guest_write(address,value);
    }
    static void cursor_write16(u32 address,u16 value) noexcept {
        cursor_write8(address,static_cast<u8>(value));
        cursor_write8(address+1u,static_cast<u8>(value>>8u));
    }
    static void cursor_map(u32 row,u32 x,u32* address,u8* plane) noexcept {
        if(!address || !plane){self().invalid_access=true;return;}
        const auto map=code2::function_2ab38(self().machine,row,x);
        *address=map.addr;
        *plane=static_cast<u8>(map.plane);
    }
    static code2::Graphics2Boundary cursor_gfx() noexcept {
        auto cb=graphics();
        cb.read8=&cursor_read8;
        cb.read16=&cursor_read16;
        cb.write8=&cursor_write8;
        cb.write16=&cursor_write16;
        cb.translate_2ab38=&cursor_map;
        return cb;
    }
    static u32 cursor_source_2a380(u32 frame,u32 y,u32 x,u32 image) noexcept {
        auto& b=self();
        if(std::uint64_t(image)+8u>b.guest_extent ||
           frame>=code2::load_le16(b.machine.data+image+2u) ||
           y>399u || x>319u){b.invalid_access=true;return 0u;}
        auto graphics=cursor_gfx();
        code2::function_2a380(b.machine,graphics,frame,y,x,image);
        ++b.source_cursor_sprite_draws;
        return 1u;
    }
    static u32 cursor_source_2a6b0(u32 frame,u32 row,u32 x,
                                    u32 clip,u32 image) noexcept {
        auto& b=self();
        if(image!=0x880Du || frame>=11u ||
           std::uint64_t(clip)+0x18u>b.guest_extent ||
           std::uint64_t(image)+8u>b.guest_extent ||
           row>399u || x>319u) {
            b.invalid_access=true;
            return 0u;
        }
        auto vga=app61();
        code2::function_2a6b0(b.machine,vga,frame,row,x,clip,image);
        ++b.source_cursor_sprite_draws;
        ++b.source_cursor_clipped_sprite_draws;
        return 1u;
    }
    void run_original_29804_cursor_sprite(u32 frame,code2::s32 x,
                                          code2::s32 y,u32 image) {
        if(image!=0x880Du || frame>=11u || x<0 || x>319 ||
           y<0 || y>399 || machine.data[0xF0D0u]!=1u)
            throw std::invalid_argument("Original 29804 mouse sprite draw order or image changed");
        Scope scope(*this);
        code2::Application57Boundary callbacks{};
        callbacks.call_2a380=&cursor_source_2a380;
        callbacks.call_2a6b0=&cursor_source_2a6b0;
        (void)code2::function_29804(machine,callbacks,frame,
                                     static_cast<u32>(x),static_cast<u32>(y),image);
        if(invalid_access || video.unsupported_read() ||
           source_cursor_sprite_draws!=1u)
            throw std::runtime_error("Original 29804/29414/2A380-or-2A6B0 cursor sprite not drawn");
    }
    // Source 1F8FC -> 2E510 -> 2E4A4 -> 2939C -> 2A380.
    // Unlike the cursor-only 29804 wrapper, this accepts the actual
    // RES0 frame image address and renders on the currently selected
    // original VGA page. No host-generated art or page is substituted.
    void run_original_2939c_res_image(u32 frame,u32 row,u32 x,u32 image) {
        if(image==0u || std::uint64_t(image)+8u>guest_extent ||
           row>=400u || x>=320u)
            throw std::out_of_range("1F8FC RES image header outside source guest");
        const u8* d=machine.data;
        const u32 stride=code2::load_le16(d+image);
        const u32 frames=code2::load_le16(d+image+2u);
        const u32 rows=code2::load_le16(d+image+4u);
        const u32 width=static_cast<u32>(d[image+6u])*8u;
        // The retail 2A380 raster writes through the linear, four-plane
        // VGA aperture. A source sprite can cross the 320px scanline
        // boundary (e.g. Level22 terminal panel x=266, width=56) without
        // overflowing the backing page. Reject actual page overflow, not
        // a valid original linear row spill.
        const std::uint64_t last_pixel=
            (std::uint64_t(row)+rows-1u)*320u+x+width-1u;
        if(!stride || !frames || !rows || !width ||
           frame>=frames || row+rows>400u ||
           last_pixel>=128000u ||
           std::uint64_t(image)+8u+
               static_cast<std::uint64_t>(frame+1u)*stride>guest_extent)
            throw std::out_of_range("1F8FC original RES0 image metadata invalid");
        Scope scope(*this);
        code2::Application56Boundary blit{};
        blit.call_2a380=&cursor_source_2a380;
        code2::function_2939c(blit,frame,row,x,image);
        if(invalid_access || video.unsupported_read() ||
           source_cursor_sprite_draws!=1u)
            throw std::runtime_error("1F8FC original 2939C/2A380 VGA blit failed");
    }
    // Original party portrait: 1F494 -> 298F0 -> 2F29C -> 2A584.
    // The 256-byte color table is a guest DGROUP address; translation
    // executes in the compiled original planar blitter, not the host.
    u32 source_hud_palette_draws=0u;
    static u32 source_hud_translated(u32 frame,u32 row,u32 x,
                                     u32 image,u32 table) noexcept {
        auto& b=self();
        auto gfx=cursor_gfx();
        code2::function_2a584(b.machine,gfx,frame,row,x,image,table);
        ++b.source_hud_palette_draws;
        return 1u;
    }
    void check_hud_image(u32 frame,u32 row,u32 x,u32 image) const {
        if(image==0u || std::uint64_t(image)+8u>guest_extent ||
           row>=400u || x>=320u)
            throw std::out_of_range("Original HUD image pointer or origin outside guest/VGA");
        const u8* d=machine.data;
        const u32 stride=code2::load_le16(d+image);
        const u32 frames=code2::load_le16(d+image+2u);
        const u32 rows=code2::load_le16(d+image+4u);
        const u32 pixels=static_cast<u32>(d[image+6u])*8u;
        const std::uint64_t last_pixel=
            (std::uint64_t(row)+rows-1u)*320u+x+pixels-1u;
        if(!stride || !frames || !rows || !pixels ||
           frame>=frames || rows>400u-row ||
           last_pixel>=128000u ||
           std::uint64_t(image)+8u+
             static_cast<std::uint64_t>(frame+1u)*stride>guest_extent)
            throw std::out_of_range("Original HUD source image metadata invalid");
    }
    void run_original_2975c_hud_frame(u32 frame,u32 row,u32 x,
                                     u32 res,u32 slot,u32 image,
                                     u32 (*lookup)(u32,u32)) {
        if(!lookup)throw std::invalid_argument("Original HUD RES lookup absent");
        check_hud_image(frame,row,x,image);
        Scope scope(*this);
        code2::Application57Boundary cb{};
        cb.call_2f29c=lookup;
        cb.call_2a380=&cursor_source_2a380;
        (void)code2::function_2975c(cb,frame,row,x,res,slot);
        if(invalid_access || video.unsupported_read() ||
           source_cursor_sprite_draws!=1u)
            throw std::runtime_error("Original 2975C HUD VGA draw failed");
    }
    void run_original_298f0_hud_portrait(u32 frame,u32 row,u32 x,
                                         u32 res,u32 slot,u32 table,
                                         u32 image,u32 (*lookup)(u32,u32)) {
        if(!lookup || std::uint64_t(table)+256u>guest_extent)
            throw std::invalid_argument("Original HUD RES lookup/table absent");
        check_hud_image(frame,row,x,image);
        Scope scope(*this);
        code2::Application57Boundary cb{};
        cb.call_2f29c=lookup;
        cb.call_2a584=&source_hud_translated;
        (void)code2::function_298f0(cb,frame,row,x,res,slot,table);
        if(invalid_access || video.unsupported_read() ||
           source_hud_palette_draws!=1u)
            throw std::runtime_error("Original 298F0/2A584 portrait source failed");
    }
    // Original active-exterior sprite: 314B0 -> 29838 -> 2A478.
    // Retains the x86 subtractive transparent-key VGA write semantics;
    // resource decoding and original archive addressing are handled by the
    // caller's 2F29C, never by a substituted host image.
    void run_original_2a478_subtractive(u32 frame,u32 row,u32 x,
                                       u32 image,u32 delta) {
        if(delta>255u)
            throw std::out_of_range("Original 2A478 palette delta outside byte range");
        check_hud_image(frame,row,x,image);
        Scope scope(*this);
        auto gfx=cursor_gfx();
        code2::function_2a478(machine,gfx,frame,row,x,image,delta);
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 2A478 subtractive VGA raster failed");
    }
    // The real retail 29564 -> 2A8A0 clipped, palette-subtractive
    // transparent planar blit. Clip fields are six original guest dwords,
    // NOT an inferred host rectangle. Preserve original source arithmetic
    // and VGA sequencer masks on the same retained CODE.2 Machine.
    void run_original_2a8a0_clipped(
        u32 frame,u32 row,u32 x,u32 clip,u32 image,u32 delta) {
        if(delta>8u || !image ||
           std::uint64_t(clip)+0x18u>guest_extent ||
           std::uint64_t(image)+8u>guest_extent ||
           row>=400u || x>=320u)
            throw std::out_of_range("Original 2A8A0 clip input invalid");
        const u32 frames=code2::load_le16(machine.data+image+2u);
        const u32 stride=code2::load_le16(machine.data+image);
        const u32 original_rows=code2::load_le16(machine.data+image+4u);
        const u32 visible_rows=code2::load_le32(machine.data+clip+8u);
        const u32 visible_pixels=code2::load_le32(machine.data+clip+0x14u);
        if(!frames || frame>=frames || !stride || !original_rows ||
           !visible_rows || visible_rows>original_rows ||
           visible_rows>400u-row ||
           !visible_pixels || visible_pixels>320u ||
           std::uint64_t(image)+8u+
             std::uint64_t(frame+1u)*stride>guest_extent)
            throw std::out_of_range("Original 2A8A0 clipped IMAGE bounds invalid");
        Scope scope(*this);
        auto b=app61();
        code2::function_2a8a0(machine,b,frame,row,x,clip,image,delta);
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 2A8A0 clipped source VGA raster failed");
    }
    // Compiled original 34A74 -> 342E4/34954 four-plane angled
    // RES4 texture sampler. All ES:EDI writes remain in the retained
    // indexed VGA aperture; do not construct any host-side texture.
    static void oblique_patch8(u32 pc,u8) noexcept {
        if(pc!=0x34AF9u && pc!=0x34B69u &&
           pc!=0x34AF6u && pc!=0x34B66u &&
           pc!=0x34B07u && pc!=0x34B77u)
            self().invalid_access=true;
    }
    void run_original_34a74_oblique() {
        const u32 image=code2::load_le32(machine.data+0xC040u);
        const u32 lines=code2::load_le32(machine.data+0xFF50u);
        if(!image || std::uint64_t(image)+4098u>guest_extent ||
           code2::load_le16(machine.data+image)<4096u ||
           !lines || lines>256u ||
           std::uint64_t(0xFF58u)+6u*lines>guest_extent)
            throw std::out_of_range("Original 34A74 RES4 texture or scanline geometry unavailable");
        Scope scope(*this);
        code2::Runtime108Boundary driver{};
        driver.patch_code8=&oblique_patch8;
        code2::Runtime106Boundary raster{};
        raster.out16=&out16;
        raster.store_es8=&vga_write;
        code2::function_34a74(machine,driver,raster);
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 34A74 oblique source VGA raster failed");
    }
    // 33E6C/34110 still executes the original fixed-point polygon
    // scanline arithmetic. Canonical 34110 writes ES:EBX as a guest
    // absolute A0000h address; mirror exactly those source-computed bytes
    // through this device's genuine VGA sequencer/aperture.
    // No host texture sampler, interpolator or replacement pixels.
    bool run_original_34110_polygon() {
        const u8* d=machine.data;
        code2::s32 first=static_cast<code2::s16>(
            code2::load_le16(d+0xFDE2u));
        code2::s32 last=static_cast<code2::s16>(
            code2::load_le16(d+0xFE06u));
        const code2::s32 right=static_cast<code2::s32>(
            code2::load_le32(d+0xFDB4u));
        const code2::s32 left=static_cast<code2::s32>(
            code2::load_le32(d+0xFDB8u));
        const bool draws=first<right && last>left;
        if(draws) {
            if(last>right)last=right;
            if(first<left)first=left;
        }
        const u32 scan=code2::load_le32(d+0xFD20u);
        const u32 page=code2::load_le32(d+0xFDBCu);
        if(draws && (first<0 || first>last ||
            last>=320 || scan>=400u ||
            (page!=0xA0000u && page!=0xA8000u)))
            throw std::out_of_range(
                "Original 34110 VGA scanline bounds outside source mode");
        Scope scope(*this);
        code2::Runtime104Boundary raster{};
        raster.out16=&out16;
        code2::function_34110(machine,raster);
        if(draws) {
            for(code2::s32 x=first;x<=last;++x) {
                const std::uint64_t addr=
                    std::uint64_t(page)+static_cast<u32>(x)*0x50u+(scan>>2u);
                if(addr<0xA0000u || addr>=0xB0000u || addr>=guest_extent)
                    throw std::out_of_range(
                        "Original 34110 physical VGA address outside aperture");
                vga_write(static_cast<u32>(addr),machine.data[addr]);
            }
        }
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 34110 source polygon raster failed");
        return draws;
    }
    void run_original_29b40(u32 row,u32 x,u32 rows,u32 pixels,u32 color) {
        if(row+rows>400u || x+pixels>320u ||
           rows==0u || pixels==0u || color>255u)
            throw std::invalid_argument("Original 29B40 rect outside 320x400");
        Scope scope(*this);
        source_planar_rectangle(row,x,rows,pixels,color);
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 29B40 source planar VGA write failed");
    }
    void run_original_2e5d4() {
        if(machine.data[0xF0D0u]==0u)
            throw std::invalid_argument("Original 2E5D4 empty page stack");
        Scope scope(*this);
        source_page_pop();
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original 2E5D4 page pop VGA failure");
    }
    void run_original_20294() {
        if(machine.data[0xF0D0u]!=0u ||
           code2::load_le16(machine.data+0xEF3Fu)!=400u)
            throw std::runtime_error("Original 20294 source initial page state invalid");
        const u8 saved=machine.data[0xEF4Du];
        Scope scope(*this);
        code2::Application45Boundary b{};
        b.call_2e590=&source_page_push;
        b.call_291b0=&source_page_select;
        b.call_29b40=&source_planar_rectangle;
        b.call_2e5d4=&source_page_pop;
        code2::function_20294(machine,b);
        if(invalid_access || video.unsupported_read() ||
           machine.data[0xF0D0u]!=0u ||
           machine.data[0xEF4Du]!=saved ||
           source_page_push_calls!=1u || source_page_pop_calls!=1u ||
           source_rectangle_fill_calls!=2u)
            throw std::runtime_error("Original 20294 did not clear two source pages");
    }
    void run_original_290f4(u32 mode) {
        if(mode!=3u && mode!=0xFFu)
            throw std::invalid_argument("Only source-observed startup and exit video modes");
        Scope scope(*this);
        auto callbacks=app56();
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=290F4_BEGIN\n"<<std::flush;
        code2::function_290f4(machine,callbacks,mode);
        std::cerr<<"CODE2_VGA_SOURCE_STAGE=290F4_END\n"<<std::flush;
        ++source_mode_calls;
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original CODE2 290F4 made unsupported VGA I/O");
        if(mode==3u) {
            const auto* d=machine.data;
            if(video.bios_mode()!=0x13u || d[0xEF3Cu]!=3u ||
               code2::load_le16(d+0xEF3Fu)!=400u ||
               code2::load_le16(d+0xEF41u)!=320u ||
               code2::load_le32(d+0xEFFDu)!=400u)
                throw std::runtime_error(
                    "Original CODE2 290F4 mode3 source geometry not established");
        }
    }
    void stream_original_29234(u32 src) {
        if(std::uint64_t(src)+768u>guest_extent)
            throw std::out_of_range("Original CODE2 29234 DAC outside guest");
        Scope scope(*this);
        auto callbacks=app56();
        code2::function_29234(callbacks,src);
        if(invalid_access || video.unsupported_read())
            throw std::runtime_error("Original CODE2 29234 VGA palette unsupported");
    }
};
}} // namespace rloft::source_video_boot
