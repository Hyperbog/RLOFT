#pragma once
#include "games/ravenloft/data/TrigTable.h"
#include <cstdint>

namespace ravenloft::game {

struct MovementVector2DF2CResult {
    std::int32_t deltaX = 0;
    std::int32_t deltaY = 0;
};

// Direct reconstruction of CODE.2 0x2DF2C.
class MovementVector2DF2C {
public:
    static MovementVector2DF2CResult compute(const data::TrigTable& trig,
                                             std::int32_t magnitude16_16,
                                             std::int32_t degrees,
                                             bool gridSnapC02F);
};

} // namespace ravenloft::game
