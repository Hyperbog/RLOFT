#include "games/ravenloft/game/WorldRenderer.h"
#include <stdexcept>

namespace ravenloft::game {

WorldRenderer::WorldRenderer(const data::InitDatabase& init,
                             const data::ResourceStore& resources,
                             const data::TrigTable& trig)
    :resources_(resources),sky_(init,resources),floor_(trig),projection_(trig),
     sceneProjector_(projection_),sprites_(sceneProjector_,resources){}

WorldRenderStats WorldRenderer::render(ssi::render::IndexedFrameBuffer& framebuffer,
                                       const WorldScene& scene,
                                       std::int32_t cameraXFixed,
                                       std::int32_t cameraYFixed,
                                       int headingDegrees,
                                       const LightingState& lighting) const{
    if(framebuffer.width()!=256||framebuffer.height()!=256)
        throw std::runtime_error("Ravenloft world renderer requires the original 256x256 logical viewport");

    // Give undrawn pixels the shipped level fallback. Outdoor level 22 then
    // overwrites its upper half through 0x314B0 sky and lower half through the
    // 0x34998/0x34A74 floor path before masked world sprites are composited.
    framebuffer.clear(scene.floorFallbackColor());
    WorldRenderStats stats;
    if(scene.skySelector()!=0xff)
        stats.sky=sky_.render(framebuffer,scene.skySelector(),headingDegrees,lighting.skyShade);
    if(scene.floorResource())
        stats.floor=floor_.render(framebuffer,resources_.texture(*scene.floorResource()),
                                  cameraXFixed,cameraYFixed,headingDegrees,&lighting.floorShade);
    stats.sprites=sprites_.render(framebuffer,scene,cameraXFixed,cameraYFixed,headingDegrees);
    return stats;
}

WorldRenderStats WorldRenderer::render(ssi::render::IndexedFrameBuffer& framebuffer,
                                       const WorldScene& scene,
                                       const PartyState& party,
                                       const LightingState& lighting) const{
    return render(framebuffer,scene,party.position.x,party.position.y,party.headingDegrees,lighting);
}

}
