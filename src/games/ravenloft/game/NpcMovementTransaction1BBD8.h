#pragma once
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct NpcMovementCallbacks1BBD8 {
    // CODE.2 0x1B760. retry is the original ESI value 0..3 (or 1 on the
    // state-0x40 / countdown-zero path).
    std::function<void(data::RuntimePlacement22& actor, int moverId, int retry)> decide1B760;
    // CODE.2 0x2D5D0. Any nonzero return is accepted by 0x1BBD8, including -1.
    std::function<int(std::int32_t candidateYFixed,
                      std::int32_t candidateXFixed,
                      int moverId)> collision2D5D0;
};

// Direct structural reconstruction of CODE.2 0x1BBD8.
class NpcMovementTransaction1BBD8 {
public:
    static void execute(data::RuntimePlacement22& actor,
                        const data::NpcDefinition& definition,
                        int moverId,
                        std::uint16_t levelWidth,
                        const data::TrigTable& trig,
                        bool gridSnapC02F,
                        const NpcMovementCallbacks1BBD8& callbacks);
};

} // namespace ravenloft::game
