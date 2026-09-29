#pragma once
#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/ResourceStore.h"
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/game/FloorPlaneRenderer.h"
#include "games/ravenloft/game/LightingSystem.h"
#include "games/ravenloft/game/OpeningSequence.h"
#include "games/ravenloft/game/ProjectionSystem.h"
#include "games/ravenloft/game/SceneProjector.h"
#include "games/ravenloft/game/SkyRenderer.h"
#include "games/ravenloft/game/SpritePassRenderer.h"
#include "games/ravenloft/game/WorldScene.h"
#include <cstdint>

namespace ravenloft::game {

struct WorldRenderStats {
    SkyRenderStats sky{};
    FloorPlaneStats floor{};
    SpritePassStats sprites{};
};

class WorldRenderer {
public:
    WorldRenderer(const data::InitDatabase& init,
                  const data::ResourceStore& resources,
                  const data::TrigTable& trig);

    WorldRenderStats render(ssi::render::IndexedFrameBuffer& framebuffer,
                            const WorldScene& scene,
                            std::int32_t cameraXFixed,
                            std::int32_t cameraYFixed,
                            int headingDegrees,
                            const LightingState& lighting) const;
    WorldRenderStats render(ssi::render::IndexedFrameBuffer& framebuffer,
                            const WorldScene& scene,
                            const PartyState& party,
                            const LightingState& lighting) const;
private:
    const data::ResourceStore& resources_;
    SkyRenderer sky_;
    FloorPlaneRenderer floor_;
    ProjectionSystem projection_;
    SceneProjector sceneProjector_;
    SpritePassRenderer sprites_;
};

}
