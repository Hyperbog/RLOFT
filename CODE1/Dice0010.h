#pragma once

#include "RuntimeRng618A.h"

namespace ravenloft::game {

// Fidelity-audited reconstruction of CODE.1 0x0010..0x0048.
//
// Verified directly against the original instructions: signed JL controls the
// die loop; IMUL's low 32-bit product is logically shifted by 15; INC and both
// ADDs retain 32-bit wrap; the final CMP/JGE interprets EBX as signed and
// clamps values below one to exactly one.
class Dice0010 {
public:
    static int roll(int diceCount,
                    int dieSides,
                    int perDieModifier,
                    int flatModifier,
                    RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
