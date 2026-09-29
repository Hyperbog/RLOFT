#include "games/ravenloft/game/SkyRenderer.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::game {
namespace {
constexpr int kClipLeft=32;
constexpr int kClipTop=4;
constexpr int kClipRightExclusive=288;
constexpr int kClipBottomExclusive=132;

int normalizeHeading(int h){
    h%=360;
    if(h<0)h+=360;
    return h;
}
}

std::uint32_t SkyRenderer::nextRandom(std::uint32_t& state){
    // CODE.2 0x6BBC6: ANSI-C style LCG, result is high 15 bits.
    state=state*0x41C64E6Du+0x3039u;
    return (state>>16u)&0x7FFFu;
}

int SkyRenderer::horizontalX(int elementAngle,int elementWidth,int headingDegrees){
    // CODE.2 0x6C604..0x6C64E. IDIV truncates toward zero, as C++ signed
    // integer division does. The +0x12 bias is part of the original formula.
    const int delta=elementAngle-headingDegrees;
    const int scaled=(delta*128+18)/36;
    const int halfWidth=(elementWidth-(elementWidth<0?-1:0))/2;
    return 160-scaled-halfWidth;
}

bool SkyRenderer::intersectsSkyClip(int x,int width){
    return x<kClipRightExclusive && x+width>=kClipLeft;
}

std::vector<SkyDrawCommand> SkyRenderer::buildDrawList(std::uint8_t skySelector,
                                                        int headingDegrees,
                                                        std::uint8_t skyShade) const {
    if(skySelector>=init_.skyGroups().size())throw std::out_of_range("Sky selector");
    const auto& group=init_.skyGroups()[skySelector];
    std::vector<SkyDrawCommand> out;
    out.reserve(32+group.elements.size());

    // 0x6C41B seeds the game's RNG from heading, draws 32 strips, then 0x6C53E
    // restores the previous RNG seed. Reproducing the local sequence avoids
    // perturbing the modern runtime's gameplay RNG.
    std::uint32_t rng=static_cast<std::uint32_t>(normalizeHeading(headingDegrees));
    for(int i=0;i<32;++i){
        const auto random15=nextRandom(rng);
        const int variant=static_cast<int>((random15*7u)>>15u); // 0..6
        SkyDrawCommand c;
        c.resource=static_cast<std::uint16_t>(group.resourceBase+variant);
        c.screenX=32+i*8;
        c.screenY=4;
        c.shade=skyShade;
        c.backgroundStrip=true;
        out.push_back(c);
    }

    for(std::size_t i=0;i<group.elements.size();++i){
        const auto& e=group.elements[i];
        if(e.resource==0)break; // terminator used by the shipped groups
        const int resolved=e.resource<8?e.resource:e.resource+group.resourceBase;
        if(resolved<0||resolved>0xFFFF)throw std::runtime_error("Sky resource index out of range");

        auto shade=static_cast<int>(std::min<std::uint8_t>(skyShade,8));
        if(i==0){
            shade=(shade-(shade<0?-1:0))/2;
            if(e.resource!=0x18)shade=0;
        }

        // Original first tries the literal angle, then +360, then -360.
        const int angles[3]={e.angle,e.angle+360,e.angle-360};
        int x=0;
        bool visible=false;
        for(int a:angles){
            x=horizontalX(a,e.width,headingDegrees);
            if(intersectsSkyClip(x,e.width)){visible=true;break;}
        }
        if(!visible)continue;

        SkyDrawCommand c;
        c.resource=static_cast<std::uint16_t>(resolved);
        c.screenX=x;
        c.screenY=e.vertical;
        c.shade=static_cast<std::uint8_t>(shade);
        c.backgroundStrip=false;
        out.push_back(c);
    }
    return out;
}

bool SkyRenderer::drawClipped(ssi::render::IndexedFrameBuffer& framebuffer,
                              const SkyDrawCommand& command) const {
    const auto image=resources_.skyBitmap(command.resource);
    // Opening demo begins at shade 0. The resource placement and clipping are
    // exact; palette-remap shading for later day/night values is intentionally
    // handled as a separate renderer concern rather than guessed here.
    if(command.shade!=0)throw std::runtime_error("Non-zero Ravenloft sky shading is not implemented yet");

    bool wrote=false;
    const int dstBaseX=command.screenX-kClipLeft;
    const int dstBaseY=command.screenY-kClipTop;
    for(int sy=0;sy<image.height;++sy){
        const int absoluteY=command.screenY+sy;
        if(absoluteY<kClipTop||absoluteY>=kClipBottomExclusive)continue;
        const int dy=dstBaseY+sy;
        if(dy<0||dy>=framebuffer.height())continue;
        for(int sx=0;sx<image.width;++sx){
            const int absoluteX=command.screenX+sx;
            if(absoluteX<kClipLeft||absoluteX>=kClipRightExclusive)continue;
            const int dx=dstBaseX+sx;
            if(dx<0||dx>=framebuffer.width())continue;
            const auto index=static_cast<std::size_t>(sy)*image.width+static_cast<std::size_t>(sx);
            if(index>=image.alpha.size()||!image.alpha[index])continue;
            framebuffer.set(dx,dy,image.pixels[index]);
            wrote=true;
        }
    }
    return wrote;
}

SkyRenderStats SkyRenderer::render(ssi::render::IndexedFrameBuffer& framebuffer,
                                   std::uint8_t skySelector,
                                   int headingDegrees,
                                   std::uint8_t skyShade) const {
    SkyRenderStats stats;
    for(const auto& command:buildDrawList(skySelector,headingDegrees,skyShade)){
        if(command.backgroundStrip)++stats.backgroundStrips;
        else ++stats.panoramaElements;
        if(drawClipped(framebuffer,command))++stats.rasterizedDraws;
    }
    return stats;
}

} // namespace ravenloft::game
