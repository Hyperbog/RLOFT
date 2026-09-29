#pragma once
#include "games/ravenloft/game/ProjectionSystem.h"
#include "games/ravenloft/game/SpriteViewSelector.h"
#include "games/ravenloft/game/WorldScene.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

struct ProjectedScenery {
    std::size_t sourceIndex=0;
    ProjectedPoint projection{};
    SpriteViewSelection lowerView{};
    bool hasPairedUpper=false;
    SpriteViewSelection upperView{};
    std::uint8_t upperVerticalOffsetRows=0;
};

struct ProjectedNpc {
    std::size_t sourceIndex=0;
    ProjectedPoint projection{};
    SpriteViewSelection view{};
};

struct ProjectedScene {
    std::vector<ProjectedScenery> scenery;
    std::vector<ProjectedNpc> npcs;
};

// Builds the sprite-facing/render inputs exactly from the original world
// coordinates and CODE.2 0x35798 + 0x2B2F4 math.  It intentionally does not
// decide painter ordering or vertical raster placement; those remain tied to
// the original draw-list/raster routines and are traced separately.
class SceneProjector {
public:
    explicit SceneProjector(const ProjectionSystem& projection):projection_(projection){}

    ProjectedScene project(const WorldScene& scene,
                           std::int32_t cameraXFixed,
                           std::int32_t cameraYFixed,
                           int headingDegrees) const;

private:
    const ProjectionSystem& projection_;
};

} // namespace ravenloft::game
