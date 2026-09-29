#pragma once

#include <cstdint>
#include <span>

namespace ravenloft::game {

class ImageTransformsA5C8 {
public:
    // Direct reconstruction of CODE.1 0xA5C8..0xA674.
    // CODE.1 0xA675..0xA677 is alignment.
    static void transformA5C8(std::uint32_t frameIndex,
                              std::span<std::uint8_t> resource);

    // Direct reconstruction of CODE.1 0xA678..0xA6EF.
    static void transformA678(std::uint32_t frameIndex,
                              std::span<std::uint8_t> resource);
};

} // namespace ravenloft::game
