#pragma once
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include <cstdint>

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0x1AF90.
class NpcDirectionSelector1AF90 {
public:
    static int select(std::int32_t deltaXFixed,
                      std::int32_t deltaYFixed,
                      int currentDirection,
                      int retry,
                      OriginalRng30CA2& rng);
};

} // namespace ravenloft::game
