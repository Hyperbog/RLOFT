#pragma once

#include "Vga9CA0.h"

#include <cstdint>
#include <span>

namespace ravenloft::game {

class VgaTransferA484 {
public:
    // Direct reconstruction of CODE.1 0xA484..0xA4E6.
    // CODE.1 0xA4E7 is alignment.
    static void capture(std::span<std::uint8_t> destination,
                        std::int32_t first,
                        std::int32_t second,
                        std::int32_t firstCount,
                        std::int32_t secondCount,
                        const VgaState9CA0& state,
                        const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA4E8..0xA554.
    // CODE.1 0xA555..0xA557 is alignment.
    static void restore(std::span<const std::uint8_t> source,
                        std::int32_t first,
                        std::int32_t second,
                        std::int32_t firstCount,
                        std::int32_t secondCount,
                        const VgaState9CA0& state,
                        const VgaOps9CA0& ops);

    // Direct reconstruction of CODE.1 0xA558..0xA57B.
    static std::uint32_t resourcePayloadSize(std::uint32_t frameCount,
                                             std::uint32_t width,
                                             std::uint32_t heightUnits,
                                             std::uint32_t unused,
                                             std::uint32_t marker) noexcept;

    // Direct reconstruction of CODE.1 0xA57C..0xA5C5.
    // CODE.1 0xA5C6..0xA5C7 is alignment.
    static void packPlanarRow(std::uint32_t frameIndex,
                              std::uint32_t pixelCount,
                              std::span<const std::uint8_t> source,
                              std::span<std::uint8_t> resource,
                              std::uint8_t marker);
};

} // namespace ravenloft::game
