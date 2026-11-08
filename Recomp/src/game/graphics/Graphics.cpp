// CODE.2 bounded VGA host port, latch, DAC and planar scanout service.
// Operates on the original caller-owned VGA instance, not a second device.
#include "Graphics.hpp"

namespace rloft::graphics {
bool OriginalVgaHost::write_port8(std::uint16_t port, std::uint8_t value) noexcept{
        switch (port) {
        case 0x3C4: seq_index_=value; return true;
        case 0x3C5:
            if (seq_index_==2) { map_mask_=value & 15u; sequencer_[2]=value; return true; }
            if (seq_index_==4) { sequencer_[4]=value; return true; }
            return false;
        case 0x3CE: gc_index_=value; return true;
        case 0x3CF:
            if (gc_index_==0) { set_reset_=value & 15u; return true; }
            if (gc_index_==1) { enable_set_reset_=value & 15u; return true; }
            if (gc_index_==3) { data_rotate_=value & 31u; return true; }
            if (gc_index_==4) { read_plane_=value & 3u; return true; }
            if (gc_index_==5 && (value & 8u)==0) { write_mode_=value & 3u; gc_mode_=value; return true; }
            if (gc_index_==6) { gc_misc_=value; return true; }
            if (gc_index_==8) { bit_mask_=value; return true; }
            return false;
        case 0x3D4: crtc_index_=value; return true;
        case 0x3D5:
            // Original CODE.2 touches 07/09/0C/0D/14/17/18. Other
            // indexed writes fail closed until mapped from original bytes.
            if (crtc_index_!=7 && crtc_index_!=9 && crtc_index_!=12 &&
                crtc_index_!=13 && crtc_index_!=20 && crtc_index_!=23 &&
                crtc_index_!=24) return false;
            crtc_[crtc_index_]=value;
            crtc_start_=static_cast<std::uint16_t>(
                (std::uint16_t(crtc_[12])<<8) | crtc_[13]);
            return true;
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

std::uint8_t OriginalVgaHost::read_port8(std::uint16_t port) noexcept{
        switch (port) {
        case 0x3C5:
            if (seq_index_==2 || seq_index_==4) return sequencer_[seq_index_];
            break;
        case 0x3CF:
            switch (gc_index_) {
            case 0: return set_reset_;
            case 1: return enable_set_reset_;
            case 3: return data_rotate_;
            case 4: return read_plane_;
            case 5: return gc_mode_;
            case 6: return gc_misc_;
            case 8: return bit_mask_;
            default: break;
            }
            break;
        case 0x3D5:
            if (crtc_index_==7 || crtc_index_==9 || crtc_index_==12 ||
                crtc_index_==13 || crtc_index_==20 || crtc_index_==23 ||
                crtc_index_==24) return crtc_[crtc_index_];
            break;
        case 0x3DA:
            retrace_high_=!retrace_high_;
            ++retrace_reads_;
            return retrace_high_ ? 0x08u : 0u;
        default: break;
        }
        unsupported_read_=true;
        return 0;
    }

bool OriginalVgaHost::unsupported_read() const noexcept{ return unsupported_read_; }

std::uint64_t OriginalVgaHost::retrace_reads() const noexcept{ return retrace_reads_; }

std::uint8_t OriginalVgaHost::bios_mode() const noexcept{ return bios_mode_; }

void OriginalVgaHost::bios_set_mode(std::uint8_t mode) noexcept{
        bios_mode_=mode;
        // The VGA guest initializer will program register 09 after BIOS 13h;
        // this resets the model's standard 200-line logical repeat.
        if (mode==0x13) crtc_[9]=0x41u;
    }

std::uint16_t OriginalVgaHost::line_compare() const noexcept{
        return static_cast<std::uint16_t>(
            crtc_[24] | ((crtc_[7] & 0x10u) ? 0x100u : 0u)
                      | ((crtc_[9] & 0x40u) ? 0x200u : 0u));
    }

std::uint32_t OriginalVgaHost::repeat_scanlines() const noexcept{
        // Register 09 low five bits define maximum scan-line count; modes
        // configured by CODE.2 use 0 (400-row) or 1 (200-row scan double).
        return static_cast<std::uint32_t>(crtc_[9]&31u)+1u;
    }

bool OriginalVgaHost::write_indexed_port(std::uint16_t port, std::uint16_t ax) noexcept{
        if (port!=0x3C4 && port!=0x3CE && port!=0x3D4) return false;
        write_port8(port, static_cast<std::uint8_t>(ax));
        return write_port8(static_cast<std::uint16_t>(port+1),
                           static_cast<std::uint8_t>(ax>>8));
    }

bool OriginalVgaHost::write_aperture(std::uint16_t offset, std::uint8_t value) noexcept{
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

std::uint8_t OriginalVgaHost::read_aperture(std::uint16_t offset) noexcept{
        for (unsigned plane=0;plane<4;++plane) latches_[plane]=planes_[plane][offset];
        return latches_[read_plane_];
    }

bool OriginalVgaHost::write_direct_pixel(std::uint32_t x,std::uint32_t y,std::uint8_t color) noexcept{
        if (x>=kWidth || y>=kHeight) return false;
        const PlanarAddress addr=code2_planar_address(x,y);
        map_mask_=static_cast<std::uint8_t>(1u<<addr.plane);
        write_aperture(static_cast<std::uint16_t>(addr.offset),color);
        return true;
    }

std::uint8_t OriginalVgaHost::indexed_scanout(std::uint32_t x,std::uint32_t y) const noexcept{
        // Line compare is a physical scanline threshold. Immediately after
        // the compare line the VGA address generator restarts at plane byte 0.
        const bool after_split=y>line_compare();
        const std::uint32_t physical_row=after_split?y-line_compare()-1u:y;
        const std::uint32_t source_row=physical_row/repeat_scanlines();
        const std::uint32_t start=after_split?0u:crtc_start_;
        const PlanarAddress addr=code2_planar_address(x,source_row,start);
        return planes_[addr.plane][static_cast<std::uint16_t>(addr.offset)];
    }

std::vector<std::uint8_t> OriginalVgaHost::scanout_indices() const{
        std::vector<std::uint8_t> frame(kWidth*kHeight);
        for (std::uint32_t y=0;y<kHeight;++y)
            for(std::uint32_t x=0;x<kWidth;++x)
                frame[y*kWidth+x]=indexed_scanout(x,y);
        return frame;
    }

Rgb6 OriginalVgaHost::palette(std::uint8_t index) const noexcept{ return palette_[index]; }

Rgba8 OriginalVgaHost::rgb_at(std::uint32_t x,std::uint32_t y) const noexcept{
        const Rgb6 c=palette_[indexed_scanout(x,y)];
        return {dac6_to_byte(c.r),dac6_to_byte(c.g),dac6_to_byte(c.b),255};
    }

std::uint16_t OriginalVgaHost::crtc_start() const noexcept{ return crtc_start_; }

std::uint8_t OriginalVgaHost::map_mask() const noexcept{ return map_mask_; }
} // namespace rloft::graphics
