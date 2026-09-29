#include "VgaClippedAA20.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u);
}

void selectPlane(std::uint8_t plane,
                 const VgaOps9CA0& ops) {
    ops.out16(
        0x3C4,
        static_cast<std::uint16_t>(
            0x02u |
            (static_cast<std::uint16_t>(
                 static_cast<std::uint8_t>(1u << plane)) << 8u)));
}

void advance(std::uintptr_t& address,
             std::int32_t delta) noexcept {
    if (delta >= 0) {
        address += static_cast<std::uintptr_t>(delta);
    } else {
        address -= static_cast<std::uintptr_t>(-delta);
    }
}

void drawImpl(std::uint32_t frameIndex,
              std::int32_t first,
              std::int32_t second,
              const ClipWork6854& work,
              std::span<const std::uint8_t> resource,
              std::uint8_t adjustment,
              bool adjusted,
              bool repeatInitialPlaneSkip,
              const VgaState9CA0& state,
              const VgaOps9CA0& ops) {
    const std::uint32_t sourceLead =
        static_cast<std::uint32_t>(work.sourceLeadBytes) >> 2u;
    const std::uint32_t sourceTail =
        static_cast<std::uint32_t>(work.sourceTailBytes) >> 2u;
    const std::int32_t visibleWidth = work.width;

    std::uint32_t leadingRows =
        (static_cast<std::uint32_t>(work.topSkip) + 3u) >> 2u;

    const std::uint32_t bottomRemainder =
        static_cast<std::uint32_t>(work.bottomSkip) & 3u;
    const bool hasBottomSkip = work.bottomSkip != 0;

    std::uint32_t trailingRows =
        hasBottomSkip
            ? (static_cast<std::uint32_t>(work.bottomSkip) >> 2u)
            : 0u;

    std::uint32_t visibleRows =
        (static_cast<std::uint32_t>(work.height) +
         bottomRemainder) >> 2u;

    std::uint32_t initialPlaneSkip = 0;

    if (visibleRows == 0) {
        visibleRows = 1;

        if (!hasBottomSkip) {
            initialPlaneSkip =
                static_cast<std::uint32_t>(4 - work.height);
            --leadingRows;
        }
    }

    const std::uint32_t topPlaneRemainder =
        static_cast<std::uint32_t>(work.topSkip) & 3u;

    const auto mapped = ops.mapAEA8(first, second);
    std::uintptr_t destination = mapped.address;

    std::uint8_t phase =
        static_cast<std::uint8_t>(
            (static_cast<std::uint32_t>(mapped.plane) + 4u -
             topPlaneRemainder) & 3u);

    const std::uint32_t frameSize = read16(resource, 0);
    std::size_t source =
        static_cast<std::size_t>(frameSize * frameIndex + 8u);

    std::uint8_t transparentValue = 0xFFu;
    if ((resource[7] & 0x80u) != 0) {
        transparentValue = resource[source++];
    }

    std::uint32_t planeCount = 4;
    const std::uint32_t planeChunk = frameSize >> 2u;

    if (repeatInitialPlaneSkip) {
        while (initialPlaneSkip != 0) {
            source += planeChunk;
            --initialPlaneSkip;
            --planeCount;
            phase = static_cast<std::uint8_t>((phase + 1u) & 3u);
        }
    } else if (initialPlaneSkip != 0) {
        source += planeChunk;
        --initialPlaneSkip;
        --planeCount;
        phase = static_cast<std::uint8_t>((phase + 1u) & 3u);
    }

    std::int32_t rowAdvance =
        static_cast<std::int32_t>(state.rowStride2B41) -
        static_cast<std::int32_t>(visibleRows) -
        static_cast<std::int32_t>(trailingRows);

    selectPlane(phase, ops);

    while (planeCount != 0) {
        const std::uintptr_t planeDestination = destination;

        std::int32_t columnsRemaining = visibleWidth;

        while (columnsRemaining != 0) {
            source += sourceLead;
            source += leadingRows;

            for (std::uint32_t i = 0;
                 i < visibleRows;
                 ++i) {
                const std::uint8_t sourceValue = resource[source++];

                if (sourceValue != transparentValue) {
                    const std::uint8_t output =
                        adjusted
                            ? static_cast<std::uint8_t>(sourceValue - adjustment)
                            : sourceValue;
                    ops.writeMemory8(destination, output);
                }

                ++destination;
            }

            destination += trailingRows;
            source += trailingRows;
            advance(destination, rowAdvance);

            --columnsRemaining;
        }

        destination = planeDestination;
        source += sourceTail;

        --planeCount;
        if (planeCount == 0) {
            break;
        }

        const bool wrapped = phase == 3u;
        phase = static_cast<std::uint8_t>((phase + 1u) & 3u);

        if (wrapped) {
            ++destination;

            if (leadingRows != 0) {
                --leadingRows;
                --rowAdvance;
                --destination;

                if (hasBottomSkip) {
                    ++trailingRows;
                } else {
                    ++visibleRows;
                }
            } else if (hasBottomSkip) {
                ++trailingRows;
                --visibleRows;

                if (visibleRows == 0) {
                    break;
                }
            }
        }

        selectPlane(phase, ops);
    }
}

} // namespace

void VgaClippedAA20::draw(
    std::uint32_t frameIndex,
    std::int32_t first,
    std::int32_t second,
    const ClipWork6854& work,
    std::span<const std::uint8_t> resource,
    const VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    drawImpl(
        frameIndex,
        first,
        second,
        work,
        resource,
        0,
        false,
        true,
        state,
        ops);
}

void VgaClippedAA20::drawAdjusted(
    std::uint32_t frameIndex,
    std::int32_t first,
    std::int32_t second,
    const ClipWork6854& work,
    std::span<const std::uint8_t> resource,
    std::uint8_t adjustment,
    const VgaState9CA0& state,
    const VgaOps9CA0& ops) {
    drawImpl(
        frameIndex,
        first,
        second,
        work,
        resource,
        adjustment,
        true,
        false,
        state,
        ops);
}

} // namespace ravenloft::game
