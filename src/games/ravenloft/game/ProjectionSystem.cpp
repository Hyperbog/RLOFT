#include "games/ravenloft/game/ProjectionSystem.h"
#include <limits>

namespace ravenloft::game {

ProjectedPoint ProjectionSystem::project(std::int32_t worldXFixed,
                                          std::int32_t worldYFixed,
                                          std::int32_t cameraXFixed,
                                          std::int32_t cameraYFixed,
                                          int headingDegrees) const {
    // CODE.2 0x357A3..0x3580F.  The original uses the SINTAB sine value for
    // heading at DS:255F8 and the +90-degree alias for DS:255FC.
    const auto dx=worldXFixed-cameraXFixed;
    const auto dy=worldYFixed-cameraYFixed;
    const auto forwardX=trig_.sinMul16_16(dx,headingDegrees);
    const auto forwardY=trig_.cosMul16_16(dy,headingDegrees);
    const auto sideX=trig_.cosMul16_16(dx,headingDegrees);
    const auto sideY=trig_.sinMul16_16(dy,headingDegrees);

    ProjectedPoint out;
    out.depthFixed=forwardX-forwardY-0x2000; // executable's camera near-plane bias
    out.lateralFixed=sideX+sideY;

    // CODE.2 0x35812..0x35832: reject signed-negative depth and values below
    // 0x5800.  Expressing the same comparison directly is equivalent for the
    // signed 16.16 coordinates used by the shipped game.
    if(out.depthFixed<0x5800) return out;

    const auto& rc=trig_.rendererConstants();
    const auto numerator=static_cast<std::int64_t>(rc.perspectiveScale)*out.lateralFixed;
    const auto projected=numerator/out.depthFixed; // C++20 signed division truncates toward zero, as IDIV does.
    const auto sx=static_cast<std::int64_t>(rc.screenCenterX)+projected;
    if(sx<std::numeric_limits<std::int16_t>::min()||sx>std::numeric_limits<std::int16_t>::max()) return out;

    const auto divisor=out.depthFixed>>8;
    if(divisor<=0) return out;
    const auto scale=static_cast<std::uint32_t>(rc.depthScaleNumerator/static_cast<std::uint32_t>(divisor));
    if(scale>0xFFFFu) return out;

    out.visible=true;
    out.screenX=static_cast<std::int16_t>(sx);
    out.scale8_8=static_cast<std::uint16_t>(scale);
    return out;
}

} // namespace ravenloft::game
