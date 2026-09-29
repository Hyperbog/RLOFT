#pragma once

#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/CellInteractionRuntimeIndex.h"
#include "games/ravenloft/game/CellInteractionSystem.h"
#include <cstdint>

namespace ravenloft::game {

struct NpcCandidateCellScanResult {
    bool blocked = false;
    bool transitionTriggered = false;
};

// Ordinary-NPC (mover ids 0..399) reconstruction of the cell-scan portion of
// CODE.2 0x2D5D0, addresses 0x2D5DC..0x2D93F. Player-only helper branches
// 0x2D004/0x2D3D8 are deliberately outside this class.
class NpcCandidateCellScanSystem {
public:
    NpcCandidateCellScanSystem(const data::WorkLevelSnapshot& work,
                               CellInteractionRuntimeIndex& interactions)
        : work_(work), interactions_(interactions) {}

    NpcCandidateCellScanResult evaluate(
        std::int32_t candidateYFixed,
        std::int32_t candidateXFixed,
        int moverId,
        std::uint8_t moverNpcTraits39,
        bool scanSuppressedF048,
        const CellInteractionCallbacks& interactionCallbacks) const;

private:
    const data::WorkLevelSnapshot& work_;
    CellInteractionRuntimeIndex& interactions_;
};

} // namespace ravenloft::game
