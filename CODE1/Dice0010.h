#pragma once

#include "RuntimeRng618A.h"

namespace ravenloft::game {

// Direct reconstruction of CODE.1 0x0010..0x0048.
//
// For each die actually rolled, the routine computes:
//   1 + ((rng15 * dieSides) >> 15) + perDieModifier
// It then adds flatModifier once and clamps the final signed result to >= 1.
class Dice0010 {
public:
    static int roll(int diceCount,
                    int dieSides,
                    int perDieModifier,
                    int flatModifier,
                    RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
