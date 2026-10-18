#pragma once
// Bounded host-side test adapter for original CODE.2 VGA interfaces.
// Latch ROP/write modes, CRTC splits, retrace/IRQ and actual game callbacks
// remain UNIMPLEMENTED: tests here are not original frame reproduction.
#include "../../Code2/graphics/original_vga_math.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft::graphics {
struct Rgb6 { std::uint8_t r=0, g=0, b=0; };
struct Rgba8 { std::uint8_t r=0, g=0, b=0, a=255; };
class OriginalVgaHost {
public:
    // Minimal indexed port subset actually used at original CODE.2
    // 0x29AA4, 0x29CA0 and 0x2ABF4; reject unsupported data registers.
    bool write_port8(std::uint16_t port, std::uint8_t value) noexcept {
        switch (port) {
        case 0x3C4: seq_index_=value; return true;
        case 0x3C5:
            if (seq_index_==2) { map_mask_=value & 15u; return true; }
            return false;
        case 0x3CE: gc_index_=value; return true;
        case 0x3CF:
            if (gc_index_==4) { read_plane_=value & 3u; return true; }
            return false;
        case 0x3D4: crtc_index_=value; return true;
        case 0x3D5:
            if (crtc_index_==12) {
                crtc_start_=static_cast<std::uint16_t>((crtc_start_ & 0x00FFu) | (std::uint16_t(value)<<8));
                return true;
            }
            if (crtc_index_==13) {
                crtc_start_=static_cast<std::uint16_t>((crtc_start_ & 0xFF00u) | value);
                return true;
            }
            return false;
        case 0x3C8: dac_index_=value; dac_component_=0; return true;
        case 0x3C9:
            if (dac_component_==0) palette_[dac_index_].r=value & 63u;
            if (dac_component_==1) palette_[dac_index_].g=value & 63u;
            if (dac_component_==2) palette_[dac_index_].b=value & 63u;
            if (++dac_component_==3) { dac_component_=0; ++dac_index_; }
            return true;
        default: return false;
        }
    }
    // x86 OUT DX,AX sends AL to port, AH to port+1.
    bool write_indexed_port(std::uint16_t port, std::uint16_t ax) noexcept {
        if (port!=0x3C4 && port!=0x3CE && port!=0x3D4) return false;
        write_port8(port, static_cast<std::uint8_t>(ax));
        return write_port8(static_cast<std::uint16_t>(port+1),
                           static_cast<std::uint8_t>(ax>>8));
    }
    // ONLY direct write-mode-0 byte transfer with full bit mask and no
    // set/reset/rotate. Do not route unknown register modes here.
    void write_aperture(std::uint16_t offset, std::uint8_t value) noexcept {
        for (std::size_t plane=0; plane<4; ++plane)
            if ((map_mask_ & (1u<<plane))!=0) planes_[plane][offset]=value;
    }
    std::uint8_t read_aperture(std::uint16_t offset) const noexcept {
        return planes_[read_plane_][offset];
    }
    bool write_direct_pixel(std::uint32_t x,std::uint32_t y,std::uint8_t color) noexcept {
        if (x>=kWidth || y>=kHeight) return false;
        const PlanarAddress addr=code2_planar_address(x,y);
        map_mask_=static_cast<std::uint8_t>(1u<<addr.plane);
        write_aperture(static_cast<std::uint16_t>(addr.offset),color);
        return true;
    }
    // Raw planar scanout indexed bytes. Unmodeled CRTC modes and line
    // compare must not be mistaken for exact physical VGA frames.
    std::uint8_t indexed_scanout(std::uint32_t x,std::uint32_t y) const noexcept {
        const PlanarAddress addr=code2_planar_address(x,y,crtc_start_);
        return planes_[addr.plane][static_cast<std::uint16_t>(addr.offset)];
    }
    std::vector<std::uint8_t> scanout_indices() const {
        std::vector<std::uint8_t> frame(kWidth*kHeight);
        for (std::uint32_t y=0;y<kHeight;++y)
            for(std::uint32_t x=0;x<kWidth;++x)
                frame[y*kWidth+x]=indexed_scanout(x,y);
        return frame;
    }
    Rgb6 palette(std::uint8_t index) const noexcept { return palette_[index]; }
    Rgba8 rgb_at(std::uint32_t x,std::uint32_t y) const noexcept {
        const Rgb6 c=palette_[indexed_scanout(x,y)];
        return {dac6_to_byte(c.r),dac6_to_byte(c.g),dac6_to_byte(c.b),255};
    }
    std::uint16_t crtc_start() const noexcept { return crtc_start_; }
    std::uint8_t map_mask() const noexcept { return map_mask_; }
private:
    std::array<std::array<std::uint8_t,kVgaPlaneBytes>,4> planes_{};
    std::array<Rgb6,256> palette_{};
    std::uint8_t seq_index_=0,gc_index_=0,crtc_index_=0;
    std::uint8_t map_mask_=15,read_plane_=0;
    std::uint8_t dac_index_=0,dac_component_=0;
    std::uint16_t crtc_start_=0;
};
} // namespace rloft::graphics
