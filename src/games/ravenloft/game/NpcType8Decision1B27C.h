#pragma once
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/NpcType8Relocate1B1C4.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include <cstdint>

namespace ravenloft::game {
class NpcType8Decision1B27C {
public:
    static void execute(data::RuntimePlacement22& actor,
                        int moverId,
                        std::int32_t partyXFixed,
                        std::int32_t partyYFixed,
                        OriginalRng30CA2& rng,
                        const NpcType8RelocateCallbacks1B1C4& relocateCallbacks);
};
} // namespace ravenloft::game
