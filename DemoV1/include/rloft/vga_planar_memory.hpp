#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/startup_presenter.hpp"

namespace rloft {

// Portable representation of the planar VGA 64 KiB A0000 aperture used by
// CODE.2 3395C/369D8: writes share one byte address but target the planes
// selected by sequencer register 02h. No host VGA I/O is performed.
//
// This models hardware byte writes only. The generated SINTAB rasterizer
// still has to be translated before scene pixels can be supplied.
class VgaPlanarMemory final {
public:
    static constexpr std::uint32_t kPhysicalBase = 0xA0000U;
    static constexpr std::size_t kApertureBytes = 0x10000U;
    static constexpr std::size_t kPageBytes = 0x8000U;
    static constexpr std::size_t kRowBytes = 80U;

    VgaPlanarMemory();

    void clear(std::uint8_t value = 0U) noexcept;
    void write(
        std::uint32_t physical_address,
        std::uint8_t plane_mask,
        std::uint8_t color_index);
    [[nodiscard]] std::uint8_t read(
        std::uint32_t physical_address,
        std::uint8_t plane) const;

    // Bidirectional bridge to the demo's existing 320x400 indexed frame.
    // VGA page 0 and page 1 each occupy 0x8000 bytes per plane and do not
    // overlap. The last 768 bytes of each page are not display pixels.
    void import_page(
        const IndexedFramebuffer& source,
        unsigned page);
    void export_page(
        IndexedFramebuffer& destination,
        unsigned page) const;

private:
    [[nodiscard]] static std::size_t offset_for(
        std::uint32_t physical_address);
    [[nodiscard]] static std::size_t page_start(unsigned page);
    std::array<std::vector<std::uint8_t>, 4> planes_;
};

} // namespace rloft
