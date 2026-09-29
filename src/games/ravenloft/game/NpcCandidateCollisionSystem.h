#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/CandidateCollisionTailSystem.h"
#include "games/ravenloft/game/CellInteractionRuntimeIndex.h"
#include "games/ravenloft/game/NpcCandidateCellScanSystem.h"
#include <cstdint>

namespace ravenloft::game {

// Complete ordinary-NPC (runtime mover index 0..399) composition of
// CODE.2 0x2D5D0: per-cell scan followed by the 0x2D944 collision tail.
// Player mover ids -1/-2 and special mover 0x190 retain separate branches.
class NpcCandidateCollisionSystem {
public:
    NpcCandidateCollisionSystem(const data::WorkLevelSnapshot& work,
                                const data::NpcDatabase& npcs,
                                const data::InitDatabase& init,
                                CellInteractionRuntimeIndex& interactions)
        : work_(work), npcs_(npcs),
          npcOverlap_(work, npcs), scenery_(work, init),
          cellScan_(work, interactions), tail_(npcOverlap_, scenery_) {}

    // Returns the original 0/1 result for ordinary NPC movers.
    int evaluate(std::int32_t candidateYFixed,
                 std::int32_t candidateXFixed,
                 int moverId,
                 std::int32_t partyXFixed,
                 std::int32_t partyYFixed,
                 bool scanSuppressedF048,
                 const CellInteractionCallbacks& interactionCallbacks,
                 const SceneryCollisionCallbacks& sceneryCallbacks = {}) const;

private:
    const data::WorkLevelSnapshot& work_;
    const data::NpcDatabase& npcs_;
    NpcOverlapSystem npcOverlap_;
    SceneryCollisionSystem scenery_;
    NpcCandidateCellScanSystem cellScan_;
    CandidateCollisionTailSystem tail_;
};

} // namespace ravenloft::game
