#pragma once
#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/ResourceStore.h"
#include "games/ravenloft/game/OriginalRenderQueue.h"
#include "games/ravenloft/game/SceneProjector.h"
#include "games/ravenloft/game/SpriteRasterizer.h"
#include "games/ravenloft/game/WorldScene.h"
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

struct SpritePassStats {
    std::size_t projectedScenery=0;
    std::size_t projectedNpcs=0;
    std::size_t queuedRecords=0;
    std::size_t drawCalls=0;
    std::size_t rasterizedDraws=0;
};

class SpritePassRenderer {
public:
    SpritePassRenderer(const SceneProjector& projector,const data::ResourceStore& resources)
        :projector_(projector),resources_(resources){}

    SpritePassStats render(ssi::render::IndexedFrameBuffer& fb,
                           const WorldScene& scene,
                           std::int32_t cameraXFixed,
                           std::int32_t cameraYFixed,
                           int headingDegrees) const;
private:
    const SceneProjector& projector_;
    const data::ResourceStore& resources_;
};

} // namespace ravenloft::game
