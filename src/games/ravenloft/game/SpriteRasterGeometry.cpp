#include "games/ravenloft/game/SpriteRasterGeometry.h"

namespace ravenloft::game {
namespace {
std::int32_t truncDiv(std::int32_t a,std::int32_t b){ return a/b; }
std::int32_t scaledCoord(std::int32_t v,std::int32_t factor){
    // 0x21C94..0x21D15: (value * factor + 0x40) / 0x80.
    return truncDiv(v*factor+0x40,0x80);
}
}

SpriteRasterRect SpriteRasterGeometry::compute(std::int16_t screenX,
                                                std::uint16_t scale8_8,
                                                const data::IndexedImage& sprite,
                                                std::uint16_t minimumScale){
    SpriteRasterRect out;
    // 0x21BFB..0x21C13: the caller has temporarily added 0x7D00 to
    // record+2.  The routine subtracts that marker and then arithmetic-shifts
    // the original projected 8.8 value right by three before applying the
    // minimum-size test and rectangle math.
    const auto scale=static_cast<std::int32_t>(scale8_8) >> 3;
    if(scale<minimumScale) return out; // 0x21C13 -> 0x21DA5

    const auto halfWidth=(static_cast<std::int32_t>(sprite.width) - (static_cast<std::int32_t>(sprite.width)<0 ? -1:0))/2;
    const auto spanWidth=static_cast<std::int32_t>(sprite.right)-static_cast<std::int32_t>(sprite.left);
    const auto spanHeight=static_cast<std::int32_t>(sprite.bottom)-static_cast<std::int32_t>(sprite.top);

    const auto quarter=truncDiv(scale,4);
    auto work=scale-truncDiv(scale,8);
    const auto factor=work*4;
    out.scaleFactor=factor;

    const auto topScaled=scaledCoord(static_cast<std::uint16_t>(sprite.top),factor);
    const auto leftScaled=scaledCoord(static_cast<std::uint16_t>(sprite.left),factor);
    auto heightScaled=scaledCoord(static_cast<std::uint16_t>(spanHeight),factor);
    const auto widthScaled=scaledCoord(static_cast<std::uint16_t>(spanWidth),factor);
    const auto halfWidthScaled=scaledCoord(static_cast<std::uint16_t>(halfWidth),factor);

    const auto x0=static_cast<std::int32_t>(screenX)-(halfWidthScaled-leftScaled);
    // 0x21D31..0x21D5F: vertically center the scaled 256-row source space,
    // then add the sprite's scaled top bound.
    const auto centerTerm=truncDiv(256-factor,2);
    const auto baseY=quarter+4+centerTerm+truncDiv(factor,8);
    const auto y0=topScaled+baseY;
    if(y0+heightScaled>260) heightScaled=260-y0;

    out.drawable=heightScaled>0 && widthScaled>0;
    out.x0=x0;out.y0=y0;out.x1=x0+widthScaled;out.y1=y0+heightScaled;
    out.scaledWidth=widthScaled;out.scaledHeight=heightScaled;
    return out;
}

} // namespace ravenloft::game
