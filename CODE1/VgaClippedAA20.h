#pragma once

#include "Graphics66C0.h"
#include "Vga9CA0.h"

#include <cstdint>
#include <span>

namespace ravenloft::game {

class VgaClippedAA20 {
public:
    // Direct observable reconstruction of CODE.1 0xAA20..0xAC0E.
    // CODE.1 0xAC0F is alignment.
    static void draw(std::uint32_t frameIndex,
                     std::int32_t first,
                     std::int32_t second,
                     const ClipWork6854& work,
                     std::span<const std::uint8_t> resource,
                     const VgaState9CA0& state,
                     const VgaOps9CA0& ops);

    // Direct observable reconstruction of CODE.1 0xAC10..0xAE0A.
    // CODE.1 0xAE0B is alignment.
    static void drawAdjusted(std::uint32_t frameIndex,
                             std::int32_t first,
                             std::int32_t second,
                             const ClipWork6854& work,
                             std::span<const std::uint8_t> resource,
                             std::uint8_t adjustment,
                             const VgaState9CA0& state,
                             const VgaOps9CA0& ops);
};

} // namespace ravenloft::game
