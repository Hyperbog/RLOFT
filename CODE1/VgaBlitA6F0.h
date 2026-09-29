#pragma once

#include "Vga9CA0.h"

#include <cstdint>
#include <span>

namespace ravenloft::game {

class VgaBlitA6F0 {
public:
    // Direct observable reconstruction of CODE.1 0xA6F0..0xA7E7.
    static void draw(std::uint32_t frameIndex,
                     std::int32_t first,
                     std::int32_t second,
                     std::span<const std::uint8_t> resource,
                     const VgaState9CA0& state,
                     const VgaOps9CA0& ops);

    // Direct observable reconstruction of CODE.1 0xA7E8..0xA8F3.
    // Original code patches immediate bytes at 0xA866 and 0xA8BF with the
    // final argument; the C++ reconstruction applies that byte subtraction
    // explicitly instead of self-modifying executable memory.
    static void drawAdjusted(std::uint32_t frameIndex,
                             std::int32_t first,
                             std::int32_t second,
                             std::span<const std::uint8_t> resource,
                             std::uint8_t adjustment,
                             const VgaState9CA0& state,
                             const VgaOps9CA0& ops);
};

} // namespace ravenloft::game
