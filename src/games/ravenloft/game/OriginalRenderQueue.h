#pragma once
#include "games/ravenloft/game/SceneProjector.h"
#include "games/ravenloft/game/WorldScene.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace ravenloft::game {

// CODE.2 render-record category values written by the four list builders
// called from 0x2C294.  The opening slice currently emits the first two:
// 0 = NPCs (0x2B6D0), 1 = scenery (0x2BBA0).
enum class OriginalRenderCategory : std::uint16_t {
    Npc = 0,
    Scenery = 1,
    Object = 2,
    Effect = 3,
};

enum class OriginalSpritePart : std::uint8_t {
    Main,
    PairedUpper,
};

struct OriginalSpriteDraw {
    OriginalRenderCategory category=OriginalRenderCategory::Scenery;
    OriginalSpritePart part=OriginalSpritePart::Main;
    std::size_t sourceIndex=0;
    std::uint16_t resource=0;
    std::int16_t screenX=0;
    std::uint16_t scale8_8=0;
    std::uint8_t verticalSourceOffsetRows=0;
};

struct OriginalRenderRecord {
    OriginalRenderCategory category=OriginalRenderCategory::Scenery;
    std::size_t sourceIndex=0;
    std::uint16_t scale8_8=0;
    std::vector<OriginalSpriteDraw> draws;
};

// Reproduces the ordering behavior of CODE.2 0x2C294..0x2C7C1 for the
// categories required by the opening demo. The original builds category 0
// records first, then category 1, and repeatedly selects the *smallest* scale
// with a strict '<' comparison. A stable ascending sort is therefore
// equivalent, including equal-scale tie behavior.
class OriginalRenderQueue {
public:
    static std::vector<OriginalRenderRecord> build(const ProjectedScene& projected,
                                                    const WorldScene& scene);
};

} // namespace ravenloft::game
