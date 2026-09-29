#pragma once

#include "Vga9CA0.h"

#include <cstdint>
#include <span>

namespace ravenloft::game {

class VgaTranslateA8F4 {
public:
    // Direct observable reconstruction of CODE.1 0xA8F4..0xAA1F.
    // The original patches the DWORD displacement at 0xA97B and 0xA9E2
    // with the final argument, turning those instructions into byte lookups
    // through the supplied 256-byte translation table.
    static void draw(std::uint32_t frameIndex,
                     std::int32_t first,
                     std::int32_t second,
                     std::span<const std::uint8_t> resource,
                     std::span<const std::uint8_t, 256> translation,
                     const VgaState9CA0& state,
                     const VgaOps9CA0& ops);
};

} // namespace ravenloft::game
