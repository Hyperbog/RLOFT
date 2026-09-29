#pragma once

#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/ResourceStore.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

struct SkyDrawCommand {
    std::uint16_t resource=0;
    int screenX=0;   // original absolute VGA coordinate
    int screenY=0;   // original absolute VGA coordinate
    std::uint8_t shade=0;
    bool backgroundStrip=false;
};

struct SkyRenderStats {
    std::size_t backgroundStrips=0;
    std::size_t panoramaElements=0;
    std::size_t rasterizedDraws=0;
};

// Faithful implementation of the opening-sky path rooted at CODE.2 0x6C3D4.
// The original clips this pass to VGA rect x=32..287, y=4..131, which maps to
// the upper 256x128 half of the port's local 256x256 world framebuffer.
class SkyRenderer {
public:
    SkyRenderer(const data::InitDatabase& init,const data::ResourceStore& resources)
        :init_(init),resources_(resources){}

    std::vector<SkyDrawCommand> buildDrawList(std::uint8_t skySelector,
                                               int headingDegrees,
                                               std::uint8_t skyShade=0) const;

    SkyRenderStats render(ssi::render::IndexedFrameBuffer& framebuffer,
                          std::uint8_t skySelector,
                          int headingDegrees,
                          std::uint8_t skyShade=0) const;

private:
    static std::uint32_t nextRandom(std::uint32_t& state);
    static int horizontalX(int elementAngle,int elementWidth,int headingDegrees);
    static bool intersectsSkyClip(int x,int width);
    bool drawClipped(ssi::render::IndexedFrameBuffer& framebuffer,
                     const SkyDrawCommand& command) const;

    const data::InitDatabase& init_;
    const data::ResourceStore& resources_;
};

} // namespace ravenloft::game
