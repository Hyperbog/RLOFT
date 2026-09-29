#pragma once
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/OriginalRng30CA2.h"
#include <cstdint>
#include <functional>

namespace ravenloft::game {
struct NpcType8RelocateCallbacks1B1C4 {
    std::function<int(std::int32_t candidateYFixed,
                      std::int32_t candidateXFixed,
                      int moverId)> collision2D5D0;
};
class NpcType8Relocate1B1C4 {
public:
    static void execute(data::RuntimePlacement22& actor,
                        int moverId,
                        OriginalRng30CA2& rng,
                        const NpcType8RelocateCallbacks1B1C4& callbacks);
};
} // namespace ravenloft::game
