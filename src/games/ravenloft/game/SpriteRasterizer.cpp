#include "games/ravenloft/game/SpriteRasterizer.h"
#include "games/ravenloft/game/SpriteRasterGeometry.h"
#include <algorithm>

namespace ravenloft::game {
namespace {
int scaledCoord(int v,int factor){
    // CODE.2 0x21C94..0x21D15: (value * factor + 0x40) / 0x80.
    return (v*factor+0x40)>>7;
}
}

bool SpriteRasterizer::draw(ssi::render::IndexedFrameBuffer& fb,
                            std::int16_t screenX,
                            std::uint16_t projectedScale8_8,
                            const data::IndexedImage& sprite,
                            std::uint8_t verticalSourceOffsetRows){
    // Use the executable's own screen-rectangle reconstruction from 0x21BF0
    // as the geometry authority.  The low-level VGA routine at 0x36CEC uses
    // precomputed scale tables and self-modifying column blitters; resampling
    // the already-decoded indexed sprite into the exact recovered rectangle
    // preserves the original screen placement while keeping the modern port
    // independent of VGA planar-memory implementation details.
    // 0x21BF0 receives caller-supplied minimum thresholds only when building
    // special interaction rectangles.  The actual sprite blitter 0x36CEC has
    // no 0x28 minimum: 0x36CF1..0x36D0D draws whenever (scale >> 3) > 0.
    // Reuse the recovered rectangle math for software placement, but apply the
    // draw routine's real positive-scale gate rather than the interaction gate.
    // CODE.2 0x36CF1..0x36D23: the draw routine converts the 8.8
    // projected scale to its integer table index with >>3, rejects zero,
    // then clamps that index to SINTAB C738.  The shipped table value is
    // 127.  0x21BF0 (interaction geometry) does not perform this clamp, so
    // apply it here before reusing that recovered rectangle math for the
    // modern software blit.
    constexpr std::uint16_t kRendererMaximumScale=127;
    const auto drawScale=static_cast<std::uint16_t>(
        std::min<std::uint32_t>(static_cast<std::uint32_t>(projectedScale8_8)>>3,kRendererMaximumScale)<<3);
    auto rect=SpriteRasterGeometry::compute(screenX,drawScale,sprite,1);
    if(!rect.drawable) return false;

    const int factor=rect.scaleFactor;
    // 0x36E3A..0x36E51 scales the optional vertical source-row displacement
    // using the same effective factor: (factor * offset + 0x40) >> 7.
    const int verticalShift=scaledCoord(static_cast<int>(verticalSourceOffsetRows),factor);

    const int left=std::max<int>(0,sprite.left);
    const int right=std::min<int>(static_cast<int>(sprite.width)-1,sprite.right);
    const int top=std::max<int>(0,sprite.top);
    const int bottom=std::min<int>(static_cast<int>(sprite.height),sprite.bottom);
    if(left>right||top>=bottom) return false;

    bool wrote=false;
    for(int sy=top;sy<bottom;++sy){
        const int dy0=rect.y0-verticalShift+scaledCoord(sy-top,factor);
        const int dy1=rect.y0-verticalShift+scaledCoord(sy+1-top,factor);
        if(dy1<=0||dy0>=fb.height()) continue;
        for(int sx=left;sx<=right;++sx){
            const auto si=static_cast<std::size_t>(sy)*sprite.width+static_cast<std::size_t>(sx);
            if(si>=sprite.alpha.size()||!sprite.alpha[si]) continue;
            const auto color=sprite.pixels[si];
            const int dx0=rect.x0+scaledCoord(sx-left,factor);
            const int dx1=rect.x0+scaledCoord(sx+1-left,factor);
            const int xa=std::max(0,std::min(dx0,dx1));
            const int xb=std::min<int>(fb.width(),std::max(dx0,dx1));
            const int ya=std::max(0,std::min(dy0,dy1));
            const int yb=std::min<int>(fb.height(),std::max(dy0,dy1));
            for(int y=ya;y<yb;++y)for(int x=xa;x<xb;++x){fb.set(x,y,color);wrote=true;}
        }
    }
    return wrote;
}

} // namespace ravenloft::game
