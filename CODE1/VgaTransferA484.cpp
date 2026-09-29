#include "VgaTransferA484.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u);
}

void selectReadPlane(std::uint8_t plane,
                     const VgaOps9CA0& ops) {
    ops.out16(
        0x3CE,
        static_cast<std::uint16_t>(
            0x04u |
            (static_cast<std::uint16_t>(plane) << 8u)));
}

void selectWritePlane(std::uint8_t mask,
                      const VgaOps9CA0& ops) {
    ops.out16(
        0x3C4,
        static_cast<std::uint16_t>(
            0x02u |
            (static_cast<std::uint16_t>(mask) << 8u)));
}

} // namespace

void VgaTransferA484::capture(
    std::span<std::uint8_t> destination,
    std::int32_t first,
    std::int32_t second,
    std::int32_t firstCount,
    std::int32_t secondCount,
    const VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const auto mapped = ops.mapAEA8(first, second);
    const std::uintptr_t rowStart = mapped.address;

    const std::uint32_t bytesPerRow =
        static_cast<std::uint32_t>(
            secondCount + mapped.plane + 3) >> 2u;

    const std::int32_t rowAdvance =
        static_cast<std::int32_t>(state.rowStride2B41) -
        static_cast<std::int32_t>(bytesPerRow);

    std::size_t out = 0;

    for (std::uint8_t plane = 0; plane < 4; ++plane) {
        selectReadPlane(plane, ops);

        std::uintptr_t source = rowStart;

        for (std::int32_t row = 0; row < firstCount; ++row) {
            for (std::uint32_t i = 0; i < bytesPerRow; ++i) {
                destination[out++] = ops.readMemory8(source++);
            }

            if (rowAdvance >= 0) {
                source += static_cast<std::uintptr_t>(rowAdvance);
            } else {
                source -= static_cast<std::uintptr_t>(-rowAdvance);
            }
        }
    }
}

void VgaTransferA484::restore(
    std::span<const std::uint8_t> source,
    std::int32_t first,
    std::int32_t second,
    std::int32_t firstCount,
    std::int32_t secondCount,
    const VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    const auto mapped = ops.mapAEA8(first, second);
    const std::uintptr_t rowStart = mapped.address;

    const std::uint32_t bytesPerRow =
        static_cast<std::uint32_t>(
            secondCount + mapped.plane + 3) >> 2u;

    const std::int32_t rowAdvance =
        static_cast<std::int32_t>(state.rowStride2B41) -
        static_cast<std::int32_t>(bytesPerRow);

    std::size_t in = 0;
    std::uint8_t planeMask = 1;

    for (std::uint8_t plane = 0; plane < 4; ++plane) {
        selectWritePlane(planeMask, ops);

        std::uintptr_t destination = rowStart;

        for (std::int32_t row = 0; row < firstCount; ++row) {
            for (std::uint32_t i = 0; i < bytesPerRow; ++i) {
                ops.writeMemory8(destination++, source[in++]);
            }

            if (rowAdvance >= 0) {
                destination += static_cast<std::uintptr_t>(rowAdvance);
            } else {
                destination -= static_cast<std::uintptr_t>(-rowAdvance);
            }
        }

        planeMask = static_cast<std::uint8_t>(planeMask << 1u);
    }

    selectWritePlane(0xFFu, ops);
}

std::uint32_t VgaTransferA484::resourcePayloadSize(
    std::uint32_t frameCount,
    std::uint32_t width,
    std::uint32_t heightUnits,
    std::uint32_t,
    std::uint32_t marker) noexcept {
    std::uint32_t value = width * heightUnits;
    value *= frameCount;
    value <<= 3u;

    if (marker != 0xFFu) {
        value += frameCount;
    }

    return value;
}

void VgaTransferA484::packPlanarRow(
    std::uint32_t frameIndex,
    std::uint32_t pixelCount,
    std::span<const std::uint8_t> source,
    std::span<std::uint8_t> resource,
    std::uint8_t marker) {
    const std::uint32_t frameSize = read16(resource, 0);

    std::size_t destination =
        static_cast<std::size_t>(frameSize * frameIndex + 8u);

    if (marker != 0xFFu) {
        resource[destination++] = marker;
    }

    const std::uint32_t planeLength = pixelCount >> 2u;
    const std::uint32_t gap = planeLength - 1u;

    std::size_t in = 0;

    for (std::uint32_t column = 0;
         column < planeLength;
         ++column) {
        const std::size_t base = destination;

        resource[destination++] = source[in++];
        destination += gap;
        resource[destination++] = source[in++];
        destination += gap;
        resource[destination++] = source[in++];
        destination += gap;
        resource[destination++] = source[in++];
        destination += gap;

        destination = base + 1u;
    }
}

} // namespace ravenloft::game
