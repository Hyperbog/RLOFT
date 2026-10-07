#include "rloft/vga_planar_memory.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

int main() {
    rloft::IndexedFramebuffer source;
    const auto width =
        static_cast<std::size_t>(
            rloft::IndexedFramebuffer::kWidth);
    const auto height =
        static_cast<std::size_t>(
            rloft::IndexedFramebuffer::kHeight);
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            source.mutable_pixels()[y * width + x] =
                static_cast<std::uint8_t>(
                    (x + y * 7U) & 0xFFU);
        }
    }

    rloft::VgaPlanarMemory video;
    video.import_page(source, 0U);

    rloft::IndexedFramebuffer recovered;
    video.export_page(recovered, 0U);
    assert(recovered.pixels() == source.pixels());

    // Four pixels x=4..7 occupy the same physical A0000 byte address.
    const auto address =
        rloft::VgaPlanarMemory::kPhysicalBase +
        3U * rloft::VgaPlanarMemory::kRowBytes +
        1U;
    const auto i = 3U * width + 4U;
    const auto old1 = recovered.pixels()[i + 1U];
    const auto old2 = recovered.pixels()[i + 2U];
    video.write(
        static_cast<std::uint32_t>(address),
        0x09U, 0xE2U); // plane 0 + plane 3 only
    video.export_page(recovered, 0U);
    assert(recovered.pixels()[i + 0U] == 0xE2U);
    assert(recovered.pixels()[i + 1U] == old1);
    assert(recovered.pixels()[i + 2U] == old2);
    assert(recovered.pixels()[i + 3U] == 0xE2U);
    assert(video.read(
        static_cast<std::uint32_t>(address), 0U) == 0xE2U);
    assert(video.read(
        static_cast<std::uint32_t>(address), 3U) == 0xE2U);

    // The unmodified three-plane write pass with mask zero is a no-op.
    const auto before = recovered.pixels();
    video.write(
        static_cast<std::uint32_t>(address), 0U, 0x55U);
    video.export_page(recovered, 0U);
    assert(recovered.pixels() == before);

    // A bank flip targets page 1 (0xA8000), not the visible page 0.
    video.import_page(source, 1U);
    video.write(
        static_cast<std::uint32_t>(
            address +
            rloft::VgaPlanarMemory::kPageBytes),
        0x0FU, 0x77U);
    video.export_page(recovered, 0U);
    assert(recovered.pixels() == before);
    video.export_page(recovered, 1U);
    for (unsigned lane = 0U; lane < 4U; ++lane) {
        assert(recovered.pixels()[i + lane] == 0x77U);
    }

    bool rejected = false;
    try { video.write(0x90000U, 1U, 0U); }
    catch (const std::out_of_range&) { rejected = true; }
    assert(rejected);
    rejected = false;
    try {
        video.write(
            rloft::VgaPlanarMemory::kPhysicalBase,
            0x10U, 0U);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try { video.export_page(recovered, 2U); }
    catch (const std::out_of_range&) { rejected = true; }
    assert(rejected);
    return 0;
}
