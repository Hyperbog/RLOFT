#include "games/ravenloft/game/SpriteViewSelector.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
int normalize360(int value) {
    while (value >= 360) value -= 360;
    while (value < 0) value += 360;
    return value;
}
}

SpriteViewSelection SpriteViewSelector::selectNormal(std::uint16_t absoluteBaseResource,
                                                       std::uint8_t viewCount,
                                                       std::uint16_t placementField14,
                                                       std::int32_t projectedScreenX,
                                                       std::int32_t screenCenterX,
                                                       std::int32_t partyHeadingDegrees) {
    if (viewCount != 4 && viewCount != 8)
        throw std::invalid_argument("Opening-demo sprite selector only accepts the proven 4/8-view CODE.2 branch");

    // CODE.2 0x2B3C2..0x2B40E:
    //   relative = (screenCenterX - projectedX) / 8 + heading
    //              + direction*45 - 180
    // Runtime +0x14 bits 3..5 are the facing/direction field.
    const auto direction=static_cast<std::uint8_t>((placementField14 >> 3) & 7u);
    int relative=(screenCenterX-projectedScreenX)/8 + partyHeadingDegrees
                 + static_cast<int>(direction)*45 - 180;

    // CODE.2 0x6B5A0..0x6B5F9.  The +44 and view-count centering term are
    // intentional integer arithmetic from the original executable.
    relative += 44;
    relative -= (8/static_cast<int>(viewCount))*22;
    relative=normalize360(relative);
    const auto cameraSector=static_cast<std::uint8_t>((relative/45)%8);

    // Level-22 placements have none of the alternate +0x14 bits (mask 0x6)
    // that select the special animation-state branches.  CODE.2 0x6B67D..
    // therefore reduces the 8 camera sectors to the number of stored views.
    if ((placementField14 & 0x6u) != 0)
        throw std::invalid_argument("Alternate sprite-state branch is outside the opening-demo selector contract");
    const auto divisor=8/static_cast<int>(viewCount);
    const auto viewOffset=static_cast<std::uint8_t>(cameraSector/divisor);

    // CODE.2 0x2BC1C..0x2BC53 seeds renderRecord+4 with
    //   definition.base + definition.views * ((field14 >> 6) & 7)
    // before 0x2B2F4 adds the camera-facing view offset and category resource base.
    // Direction bits 3..5 must NOT be reused here; doing so selected unrelated
    // scenery groups (for example Level-22 foliage became bare branches).
    const auto animationPhase=static_cast<std::uint8_t>((placementField14 >> 6) & 7u);
    const auto resource=static_cast<std::uint32_t>(absoluteBaseResource)
        + static_cast<std::uint32_t>(viewCount)*animationPhase + viewOffset;
    if (resource>0xFFFFu) throw std::overflow_error("Sprite resource index overflow");
    return {static_cast<std::uint16_t>(resource),cameraSector,viewOffset,direction,animationPhase};
}

} // namespace ravenloft::game
