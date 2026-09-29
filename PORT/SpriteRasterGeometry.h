#pragma once
#include "games/ravenloft/data/GraphicsDecode.h"
#include <cstdint>

namespace ravenloft::game {

struct SpriteRasterRect {
    bool drawable=false;
    std::int32_t x0=0,y0=0,x1=0,y1=0;
    std::int32_t scaledWidth=0,scaledHeight=0;
    std::int32_t scaleFactor=0; // CODE.2 0x21C7F after projected scale >> 3
};

// Exact rectangle setup from CODE.2 0x21BF0 for ordinary RES3 sprites.
// The original caller temporarily biases the high byte of the projected scale
// by 0x7D before entering 0x21BF0; this API accepts the unbiased 8.8 scale
// produced by ProjectionSystem and performs the routine's subsequent >>3.
class SpriteRasterGeometry {
public:
    static SpriteRasterRect compute(std::int16_t screenX,
                                    std::uint16_t scale8_8,
                                    const data::IndexedImage& sprite,
                                    std::uint16_t minimumScale=40);
};

} // namespace ravenloft::game
