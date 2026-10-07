#include "rloft/vga_planar_memory.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

VgaPlanarMemory::VgaPlanarMemory() {
    for (auto& plane : planes_) {
        plane.resize(kApertureBytes, 0U);
    }
}

void VgaPlanarMemory::clear(std::uint8_t value) noexcept {
    for (auto& plane : planes_) {
        std::fill(plane.begin(), plane.end(), value);
    }
}

std::size_t VgaPlanarMemory::offset_for(
    std::uint32_t physical_address) {
    if (physical_address < kPhysicalBase ||
        physical_address - kPhysicalBase >= kApertureBytes) {
        throw std::out_of_range(
            "VGA write does not address original A0000 aperture");
    }
    return static_cast<std::size_t>(
        physical_address - kPhysicalBase);
}

std::size_t VgaPlanarMemory::page_start(
    unsigned page) {
    if (page > 1U) {
        throw std::out_of_range(
            "VGA 320x400 page index must be 0 or 1");
    }
    return static_cast<std::size_t>(page) *
        kPageBytes;
}

void VgaPlanarMemory::write(
    std::uint32_t physical_address,
    std::uint8_t plane_mask,
    std::uint8_t color_index) {
    if ((plane_mask & 0xF0U) != 0U) {
        throw std::out_of_range(
            "VGA sequencer write mask exceeds four planes");
    }
    const auto offset =
        offset_for(physical_address);
    for (std::size_t plane = 0U;
         plane < planes_.size(); ++plane) {
        if ((plane_mask &
             (1U << plane)) != 0U) {
            planes_[plane][offset] = color_index;
        }
    }
}

std::uint8_t VgaPlanarMemory::read(
    std::uint32_t physical_address,
    std::uint8_t plane) const {
    if (plane >= planes_.size()) {
        throw std::out_of_range(
            "VGA read plane must be in 0..3");
    }
    return planes_[plane][
        offset_for(physical_address)];
}

void VgaPlanarMemory::import_page(
    const IndexedFramebuffer& source,
    unsigned page) {
    const auto start = page_start(page);
    const auto& pixels = source.pixels();
    constexpr auto kWidth =
        static_cast<std::size_t>(
            IndexedFramebuffer::kWidth);
    constexpr auto kHeight =
        static_cast<std::size_t>(
            IndexedFramebuffer::kHeight);
    if (pixels.size() != kWidth * kHeight ||
        kWidth != kRowBytes * 4U ||
        kHeight * kRowBytes > kPageBytes) {
        throw std::runtime_error(
            "indexed framebuffer is not 320x400 VGA planar geometry");
    }
    for (std::size_t y = 0; y < kHeight; ++y) {
        for (std::size_t x = 0; x < kWidth; ++x) {
            planes_[x & 3U][
                start + y * kRowBytes + (x >> 2U)] =
                pixels[y * kWidth + x];
        }
    }
}

void VgaPlanarMemory::export_page(
    IndexedFramebuffer& destination,
    unsigned page) const {
    const auto start = page_start(page);
    auto& pixels = destination.mutable_pixels();
    constexpr auto kWidth =
        static_cast<std::size_t>(
            IndexedFramebuffer::kWidth);
    constexpr auto kHeight =
        static_cast<std::size_t>(
            IndexedFramebuffer::kHeight);
    if (pixels.size() != kWidth * kHeight ||
        kWidth != kRowBytes * 4U ||
        kHeight * kRowBytes > kPageBytes) {
        throw std::runtime_error(
            "indexed framebuffer is not 320x400 VGA planar geometry");
    }
    for (std::size_t y = 0; y < kHeight; ++y) {
        for (std::size_t x = 0; x < kWidth; ++x) {
            pixels[y * kWidth + x] =
                planes_[x & 3U][
                    start + y * kRowBytes + (x >> 2U)];
        }
    }
}

} // namespace rloft
