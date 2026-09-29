#include "games/ravenloft/game/FloorPlaneRenderer.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
int normalizeHeading(int h){ h%=360; if(h<0)h+=360; return h; }

// 16x16 signed multiply followed by the executable's SHLD ...,3 sequence.
// Only the low 16 bits are consumed by the original row record.
std::uint16_t mulShift13(std::int16_t a,std::int16_t b){
    const auto p=static_cast<std::int32_t>(a)*static_cast<std::int32_t>(b);
    return static_cast<std::uint16_t>(static_cast<std::uint32_t>(p)>>13u);
}
}

FloorPlaneStats FloorPlaneRenderer::render(ssi::render::IndexedFrameBuffer& fb,
                                            const data::IndexedImage& texture,
                                            std::int32_t cameraXFixed,
                                            std::int32_t cameraYFixed,
                                            int headingDegrees,
                                            const std::array<std::uint8_t,128>* shade) const {
    if(texture.width!=64||texture.height!=64||texture.pixels.size()!=4096)
        throw std::runtime_error("Ravenloft floor plane requires a 64x64 type-6 texture");
    if(fb.width()<256||fb.height()<256)
        throw std::runtime_error("Ravenloft logical world framebuffer must be at least 256x256");

    const int heading=normalizeHeading(headingDegrees);
    const int startAngle=360-heading-32;
    const int stepAngle=360-heading+90;
    const auto startSin=trig_.sin14(startAngle);
    const auto startCos=trig_.cos14(startAngle);
    const auto stepSin=trig_.sin14(stepAngle);
    const auto stepCos=trig_.cos14(stepAngle);
    const auto& distance=trig_.planeDistanceTable(); // DS:19660
    const auto& start=trig_.planeStartTable();       // DS:193E0

    // CODE.2 takes the high byte of the fractional WORD of each 16.16 party
    // coordinate and adds it to the row start.  The integer tile coordinate is
    // deliberately absent because the 64x64 floor repeats once per world unit.
    const auto fracX=static_cast<std::uint8_t>((static_cast<std::uint32_t>(cameraXFixed)&0xFFFFu)>>8u);
    const auto fracY=static_cast<std::uint8_t>((static_cast<std::uint32_t>(cameraYFixed)&0xFFFFu)>>8u);

    FloorPlaneStats stats;
    for(std::size_t row=0;row<128;++row){
        const auto duWord=mulShift13(stepSin,distance[row]);
        const auto dvWord=mulShift13(stepCos,distance[row]);
        const auto startUByte=static_cast<std::uint8_t>(mulShift13(startSin,start[row]) + fracX);
        const auto startVByte=static_cast<std::uint8_t>(mulShift13(startCos,start[row]) + fracY);

        std::uint32_t u=static_cast<std::uint32_t>(startUByte)<<24u;
        std::uint32_t v=static_cast<std::uint32_t>(startVByte)<<24u;
        const std::uint32_t du=static_cast<std::uint32_t>(duWord)<<16u;
        const std::uint32_t dv=static_cast<std::uint32_t>(dvWord)<<16u;
        const auto darkness=shade?(*shade)[row]:0u;
        const int y=255-static_cast<int>(row); // 0x50F8 then -80 bytes per logical VGA scanline.

        for(int x=0;x<256;++x){
            const auto tx=(u>>26u)&63u;
            const auto ty=(v>>26u)&63u;
            auto color=texture.pixels[ty*64u+tx];
            // Low-level blitter uses SUB AL,AH, i.e. 8-bit wrapping subtraction.
            color=static_cast<std::uint8_t>(color-darkness);
            fb.set(x,y,color);
            u+=du;
            v+=dv;
            ++stats.pixels;
        }
        ++stats.rows;
    }
    return stats;
}

} // namespace ravenloft::game
