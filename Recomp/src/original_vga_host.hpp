#pragma once
// Bounded host-side test adapter for original CODE.2 VGA interfaces.
// Implements bounded VGA latch/read-map, modes 0-3, palette and CRTC start.
// CRTC splits, retrace/IRQ, chain-4 and original game callback integration
// remain UNIMPLEMENTED: synthetic tests are not original DOS frames.
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
            if (gc_index_==0) { set_reset_=value & 15u; return true; }
            if (gc_index_==1) { enable_set_reset_=value & 15u; return true; }
            if (gc_index_==3) { data_rotate_=value & 31u; return true; }
            if (gc_index_==4) { read_plane_=value & 3u; return true; }
            if (gc_index_==5 && (value & 8u)==0) { write_mode_=value & 3u; return true; }
            if (gc_index_==8) { bit_mask_=value; return true; }
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
    // VGA hardware write modes 0..3, read-side four-plane latches and ALU.
    // Read mode 1 and unchained/chain-4 address routing remain unsupported.
    bool write_aperture(std::uint16_t offset, std::uint8_t value) noexcept {
        const unsigned rotate = data_rotate_ & 7u;
        const std::uint8_t rotated = static_cast<std::uint8_t>(
            (value >> rotate) | (value << ((8u - rotate) & 7u)));
        for (unsigned plane=0; plane<4; ++plane) {
            if ((map_mask_ & (1u<<plane))==0) continue;
            if (write_mode_==1) { planes_[plane][offset]=latches_[plane]; continue; }
            std::uint8_t source=rotated;
            std::uint8_t mask=bit_mask_;
            if (write_mode_==0 && (enable_set_reset_ & (1u<<plane)))
                source=(set_reset_ & (1u<<plane)) ? 0xffu : 0;
            if (write_mode_==2)
                source=(value & (1u<<plane)) ? 0xffu : 0;
            if (write_mode_==3) {
                source=(set_reset_ & (1u<<plane)) ? 0xffu : 0;
                mask=static_cast<std::uint8_t>(bit_mask_ & rotated);
            }
            const std::uint8_t latch=latches_[plane];
            switch ((data_rotate_ >> 3) & 3u) {
            case 1: source=static_cast<std::uint8_t>(source & latch); break;
            case 2: source=static_cast<std::uint8_t>(source | latch); break;
            case 3: source=static_cast<std::uint8_t>(source ^ latch); break;
            default: break;
            }
            planes_[plane][offset]=static_cast<std::uint8_t>(
                (source & mask) | (latch & static_cast<std::uint8_t>(~mask)));
        }
        return true;
    }
    std::uint8_t read_aperture(std::uint16_t offset) noexcept {
        for (unsigned plane=0;plane<4;++plane) latches_[plane]=planes_[plane][offset];
        return latches_[read_plane_];
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
    std::uint8_t set_reset_=0,enable_set_reset_=0,data_rotate_=0;
    std::uint8_t write_mode_=0,bit_mask_=255;
    std::array<std::uint8_t,4> latches_{};
    std::uint8_t dac_index_=0,dac_component_=0;
    std::uint16_t crtc_start_=0;
};
} // namespace rloft::graphics
