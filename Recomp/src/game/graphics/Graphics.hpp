#pragma once
// Bounded host-side test adapter for original CODE.2 VGA interfaces.
// Implements bounded VGA latch/read-map, modes 0-3, palette and CRTC start.
// Supports CODE.2 CRTC split, repeat-scanline mode, deterministic retrace
// transitions; not an analog VGA model or DOS screenshot certification.
#include "../../../../Code2/graphics/original_vga_math.hpp"
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
    bool write_port8(std::uint16_t port, std::uint8_t value) noexcept;
    // Indexed register reads are required by authentic CODE.2 29930 and
    // 2AC10 read-modify-write sequences. Port 3DA retrace is deliberately
    // deterministic: each read advances one host presentation phase.
    std::uint8_t read_port8(std::uint16_t port) noexcept;
    bool unsupported_read() const noexcept;
    std::uint64_t retrace_reads() const noexcept;
    std::uint8_t bios_mode() const noexcept;
    void bios_set_mode(std::uint8_t mode) noexcept;
    std::uint16_t line_compare() const noexcept;
    std::uint32_t repeat_scanlines() const noexcept;
    // x86 OUT DX,AX sends AL to port, AH to port+1.
    bool write_indexed_port(std::uint16_t port, std::uint16_t ax) noexcept;
    // VGA hardware write modes 0..3, read-side four-plane latches and ALU.
    // Read mode 1 and unchained/chain-4 address routing remain unsupported.
    bool write_aperture(std::uint16_t offset, std::uint8_t value) noexcept;
    std::uint8_t read_aperture(std::uint16_t offset) noexcept;
    bool write_direct_pixel(std::uint32_t x,std::uint32_t y,std::uint8_t color) noexcept;
    // Raw planar scanout indexed bytes. Unmodeled CRTC modes and line
    // compare must not be mistaken for exact physical VGA frames.
    std::uint8_t indexed_scanout(std::uint32_t x,std::uint32_t y) const noexcept;
    std::vector<std::uint8_t> scanout_indices() const;
    Rgb6 palette(std::uint8_t index) const noexcept;
    Rgba8 rgb_at(std::uint32_t x,std::uint32_t y) const noexcept;
    std::uint16_t crtc_start() const noexcept;
    std::uint8_t map_mask() const noexcept;
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
    std::array<std::uint8_t,5> sequencer_{};
    std::array<std::uint8_t,25> crtc_=[] {
        std::array<std::uint8_t,25> r{};
        r[7]=0x10u; r[9]=0x40u; r[24]=0xffu;
        return r;
    }();
    std::uint8_t gc_mode_=0,gc_misc_=0,bios_mode_=0x13;
    std::uint64_t retrace_reads_=0;
    bool retrace_high_=false,unsupported_read_=false;
};
} // namespace rloft::graphics
