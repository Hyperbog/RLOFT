#pragma once

#include "games/ravenloft/game/CellInteractionSystem.h"
#include <cstdint>

namespace ravenloft::game {

struct NpcCellInteractionResult {
    bool blocked = false;
    bool transitionTriggered = false;
};

class NpcCellInteractionSystem {
public:
    // Reconstructs the ordinary-NPC (mover ids 0..399) interaction branch of
    // CODE.2 0x2D5D0 at 0x2D70A..0x2D811.
    static NpcCellInteractionResult evaluate(
        std::uint16_t interactionCode,
        CellInteractionRecord* record,
        std::uint8_t npcTraits39,
        const CellInteractionCallbacks& callbacks);
};

} // namespace ravenloft::game
