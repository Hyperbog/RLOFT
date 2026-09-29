#include "games/ravenloft/game/SpritePassRenderer.h"

namespace ravenloft::game {

SpritePassStats SpritePassRenderer::render(ssi::render::IndexedFrameBuffer& fb,
                                           const WorldScene& scene,
                                           std::int32_t cameraXFixed,
                                           std::int32_t cameraYFixed,
                                           int headingDegrees) const {
    SpritePassStats stats;
    const auto projected=projector_.project(scene,cameraXFixed,cameraYFixed,headingDegrees);
    stats.projectedScenery=projected.scenery.size();
    stats.projectedNpcs=projected.npcs.size();
    const auto queue=OriginalRenderQueue::build(projected,scene);
    stats.queuedRecords=queue.size();
    for(const auto& record:queue){
        for(const auto& draw:record.draws){
            ++stats.drawCalls;
            const auto sprite=resources_.sprite(draw.resource);
            if(SpriteRasterizer::draw(fb,draw.screenX,draw.scale8_8,sprite,draw.verticalSourceOffsetRows))
                ++stats.rasterizedDraws;
        }
    }
    return stats;
}

} // namespace ravenloft::game
