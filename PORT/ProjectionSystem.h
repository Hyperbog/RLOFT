#pragma once
#include "games/ravenloft/data/TrigTable.h"
#include <cstdint>

namespace ravenloft::game {

struct ProjectedPoint {
    bool visible=false;
    std::int16_t screenX=0;
    std::uint16_t scale8_8=0;
    std::int32_t depthFixed=0;
    std::int32_t lateralFixed=0;
};

// Exact reimplementation of CODE.2 0x35798 for the world->screen X/scale
// transform used by NPC/scenery render-list construction.
class ProjectionSystem {
public:
    explicit ProjectionSystem(const data::TrigTable& trig):trig_(trig){}

    ProjectedPoint project(std::int32_t worldXFixed,
                           std::int32_t worldYFixed,
                           std::int32_t cameraXFixed,
                           std::int32_t cameraYFixed,
                           int headingDegrees) const;

private:
    const data::TrigTable& trig_;
};

} // namespace ravenloft::game
