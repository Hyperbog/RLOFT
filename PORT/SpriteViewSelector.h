#pragma once
#include <cstdint>

namespace ravenloft::game {

// Reimplementation of the normal 4/8-view branch of CODE.2 0x6B2F4
// (object1 offset 0x2B2F4).  This is the branch exercised by every piece of
// scenery in level 22 and by the opening ASSASSIN.  The rarer 2/16-view
// branches are deliberately rejected until their game content is required.
struct SpriteViewSelection {
    std::uint16_t resource = 0;
    std::uint8_t cameraSector = 0;   // normalized 45-degree sector, 0..7
    std::uint8_t viewOffset = 0;     // selected view inside the actor's view group
    std::uint8_t direction = 0;      // runtime +0x14 bits 3..5
    std::uint8_t animationPhase = 0; // runtime +0x14 bits 6..8; seeds views*phase
};

class SpriteViewSelector {
public:
    static SpriteViewSelection selectNormal(std::uint16_t absoluteBaseResource,
                                            std::uint8_t viewCount,
                                            std::uint16_t placementField14,
                                            std::int32_t projectedScreenX,
                                            std::int32_t screenCenterX,
                                            std::int32_t partyHeadingDegrees);
};

} // namespace ravenloft::game
