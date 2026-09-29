#include "games/ravenloft/game/MovementVector2DF2C.h"

namespace ravenloft::game {

MovementVector2DF2CResult MovementVector2DF2C::compute(const data::TrigTable& trig,
                                                       std::int32_t magnitude16_16,
                                                       std::int32_t degrees,
                                                       bool gridSnapC02F) {
    // 0x2DF38..0x2DF55 performs only one wrap in either direction.
    if (degrees >= 360) degrees -= 360;
    if (degrees < 0) degrees += 360;

    if (!gridSnapC02F) {
        // 0x2DF62..0x2DF80: peculiar low-magnitude clamp retained literally.
        const auto hi = static_cast<std::int16_t>(static_cast<std::uint32_t>(magnitude16_16) >> 16u);
        const auto lo = static_cast<std::uint16_t>(magnitude16_16);
        if (hi >= 0 && hi == 0 && lo >= 0xB000u)
            magnitude16_16 = 0x0000AFFF;

        MovementVector2DF2CResult r;
        // DS:C664 is the sine table + 90 degrees (cosine); DS:C65C is sine.
        // Original outputs are X=-sin(angle)*magnitude, Y=cos(angle)*magnitude.
        r.deltaY = trig.cosMul16_16(magnitude16_16, degrees);
        r.deltaX = -trig.sinMul16_16(magnitude16_16, degrees);
        return r;
    }

    // 0x2E00C..0x2E0D8: angle/45 selects one of eight whole-cell vectors.
    const int direction = degrees / 45;
    switch (direction) {
        case 0: return {0,  0x00010000};
        case 1: return {-0x00010000,  0x00010000};
        case 2: return {-0x00010000,  0};
        case 3: return {-0x00010000, -0x00010000};
        case 4: return {0, -0x00010000};
        case 5: return {0x00010000, -0x00010000};
        case 6: return {0x00010000, 0};
        case 7: return {0x00010000, 0x00010000};
        default: return {};
    }
}

} // namespace ravenloft::game
