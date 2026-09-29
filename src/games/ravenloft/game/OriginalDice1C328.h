#pragma once
#include "games/ravenloft/game/OriginalRng30CA2.h"

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0x1C328..0x1C365.
// Arguments are literal original semantics: diceCount, dieSides,
// perDieModifier (added once for every die actually rolled), and flatModifier.
class OriginalDice1C328 {
public:
    static int roll(int diceCount,
                    int dieSides,
                    int perDieModifier,
                    int flatModifier,
                    OriginalRng30CA2& rng);
};

} // namespace ravenloft::game
