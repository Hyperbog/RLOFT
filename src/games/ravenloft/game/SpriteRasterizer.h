#pragma once
#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/GraphicsDecode.h"
#include <cstdint>

namespace ravenloft::game {

// Software interpretation of the ordinary masked-sprite geometry in
// CODE.2 0x36CEC.  It deliberately keeps palette/distance-dither handling
// separate; this class is responsible only for the source mask and the exact
// fixed-point destination geometry used by the opening forest sprites.
class SpriteRasterizer {
public:
    static bool draw(ssi::render::IndexedFrameBuffer& framebuffer,
                     std::int16_t screenX,
                     std::uint16_t projectedScale8_8,
                     const data::IndexedImage& sprite,
                     std::uint8_t verticalSourceOffsetRows=0);
};

} // namespace ravenloft::game
