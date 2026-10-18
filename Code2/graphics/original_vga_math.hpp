#pragma once
// Derived from original CODE.2 object-1 x86 0x2AB38..0x2AB6B.
// The original EBX is a full 32-bit linear guest address, not a 16-bit
// video-aperture offset. Host VGA byte-plane indexing is handled separately.
// Addressing only; not an original VGA display or DOS parity certificate.
#include <cstdint>
namespace rloft::graphics {
constexpr std::uint32_t kWidth = 320;
constexpr std::uint32_t kHeight = 400;
constexpr std::uint32_t kBytesPerScanlinePerPlane = 80;
constexpr std::uint32_t kVgaPlaneBytes = 65536;
struct PlanarAddress { std::uint32_t offset; std::uint8_t plane; };
// Actual original EBX = y*16 + y*64 + (x>>2) + guest_framebuffer_base;
// ECX = x&3; the page-selected guest base is a separate original step.
constexpr PlanarAddress code2_planar_address(std::uint32_t x,
        std::uint32_t y, std::uint32_t start = 0) noexcept {
    return {start + y * kBytesPerScanlinePerPlane + (x >> 2),
            static_cast<std::uint8_t>(x & 3u)};
}
// Host display conversion only; the game keeps indexed bytes and raw DAC6.
// The 6-to-8-bit bit replication is not an original pixel-RGB conversion.
constexpr std::uint8_t dac6_to_byte(std::uint8_t source) noexcept {
    const std::uint8_t six = static_cast<std::uint8_t>(source & 63u);
    return static_cast<std::uint8_t>((six << 2) | (six >> 4));
}
} // namespace rloft::graphics
