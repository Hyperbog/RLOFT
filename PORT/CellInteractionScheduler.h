#pragma once

#include "games/ravenloft/game/CellInteractionSystem.h"
#include <cstdint>
#include <vector>

namespace ravenloft::game {

class CellInteractionScheduler {
public:
    // Direct reconstruction of CODE.2 0x20B40 over the logical per-column
    // interaction lists. Each list corresponds to one non-null DS:CD3C[x]
    // chain and contains only live 10-byte records (the original zero terminator
    // is represented by vector length).
    static void tick(std::vector<std::vector<CellInteractionRecord>>& columns,
                     std::uint8_t& activeTransitionCount);
};

} // namespace ravenloft::game
