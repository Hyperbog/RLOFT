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
        auto b=app61();
        code2::function_2ab98(self().machine,b);
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
        auto gfx=graphics();
        code2::function_29a84(gfx);
        return 0;
    }
    static u32 source_29930(u32 mode,u32 descriptor) noexcept {
        auto b=graphics();
        return code2::function_29930(self().machine,b,mode,descriptor);
    }
    static u32 source_2a104(u32 count) noexcept {
        code2::Application59Boundary b{};
        b.call_2ac10=&source_line_compare;
        code2::function_2a104(b,count);
        return 0;
    }
    static u32 source_29fec(u32 page) noexcept {
        return code2::function_29fec(self().machine,page);
    }
    static u32 source_29ffc(u32 page) noexcept {
        code2::Application59Boundary b{};
        b.call_2abf4=&source_set_crtc_start;
        code2::function_29ffc(self().machine,b,page);
        return 0;
    }
    static u32 source_296c0(u32 y0,u32 x0,u32 y1,u32 x1) noexcept {
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
    void run_original_290f4(u32 mode) {
        if(mode!=3u && mode!=0xFFu)
            throw std::invalid_argument("Only source-observed startup and exit video modes");
        Scope scope(*this);
        auto callbacks=app56();
        code2::function_290f4(machine,callbacks,mode);
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
