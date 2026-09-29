#pragma once

#include "games/ravenloft/game/NpcOverlapSystem.h"
#include "games/ravenloft/game/PartyOverlapSystem.h"
#include "games/ravenloft/game/SceneryCollisionSystem.h"
#include <cstdint>

namespace ravenloft::game {

class CandidateCollisionTailSystem {
public:
    CandidateCollisionTailSystem(const NpcOverlapSystem& npcOverlap,
                                 const SceneryCollisionSystem& scenery)
        : npcOverlap_(npcOverlap), scenery_(scenery) {}

    // Direct reconstruction of CODE.2 0x2D944..0x2D9F8.
    // Returns 1 accepted, 0 blocked, -1 special interaction.
    int evaluate(std::int32_t candidateYFixed,
                 std::int32_t candidateXFixed,
                 int moverId,
                 std::uint8_t moverNpcTraits39,
                 std::uint16_t currentLevel,
                 std::int32_t partyXFixed,
                 std::int32_t partyYFixed,
                 bool blockedByEarlierCellScan,
                 const SceneryCollisionCallbacks& sceneryCallbacks = {}) const;

private:
    const NpcOverlapSystem& npcOverlap_;
    const SceneryCollisionSystem& scenery_;
};

} // namespace ravenloft::game
