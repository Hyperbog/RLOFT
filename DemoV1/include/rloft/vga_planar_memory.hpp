#pragma once

#include <array>
#include <cstddef>
#include <optional>
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

    // The DOS sprite loop writes sequencer register index 02h via
    // OUT DX,AX at port 03C4h for every source-plane pass. The
    // register remains observable after the pixels are written.
    // Its initial value is UNKNOWN until a source-backed OUT occurs.
    // Original 2A114 selects graphics-controller read-map register
    // index 04h via OUT DX,AX. The selector survives the capture.
    void set_graphics_read_map_04(std::uint8_t selected_plane);
    [[nodiscard]] std::optional<std::uint8_t>
    graphics_read_map_04() const noexcept;
    void set_sequencer_map_mask_02(std::uint8_t plane_mask);
    [[nodiscard]] std::optional<std::uint8_t>
    sequencer_map_mask_02() const noexcept;
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
    std::optional<std::uint8_t> sequencer_map_mask_02_{};
    std::optional<std::uint8_t> graphics_read_map_04_{};
};

} // namespace rloft
