#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/CellInteractionRuntimeIndex.h"
#include "games/ravenloft/game/CellInteractionSystem.h"
#include "games/ravenloft/game/NpcOverlapSystem.h"
#include "games/ravenloft/game/SceneryCollisionSystem.h"
#include <cstdint>
#include <functional>

namespace ravenloft::game {

// Player-only dependencies called directly by CODE.2 0x2D5D0.  They remain
// separate reconstructed routines because both perform substantial scripted
// state changes beyond collision classification itself.
struct CandidateCollisionPlayerCallbacks {
    // CODE.2 0x2D004. Argument is cellWord & 0x3F. Return value is the
    // original helper's 0/1 result; 1 causes 0x2D5D0 to return accepted
    // immediately after the helper's scripted side effects.
    std::function<int(std::uint8_t cellCode)> trigger2D004;

    // CODE.2 0x2D3D8 for a type-8 interaction. The six original arguments are
    // exposed in their proven order/meaning.
    std::function<void(int sourceCellX,
                       int sourceCellY,
                       std::uint8_t destinationX,
                       std::uint8_t destinationY,
                       std::uint8_t facingDelta45,
                       std::uint8_t destinationLevel)> portal2D3D8;
};

// Complete branch-for-branch reconstruction of CODE.2 0x2D5D0..0x2D9F8.
// Literal mover ids are retained because the executable gives them distinct
// semantics: -1 player, -2 special query, 0..399 runtime NPC, 0x190 special.
class CandidateCollisionSystem2D5D0 {
public:
    CandidateCollisionSystem2D5D0(const data::WorkLevelSnapshot& work,
                                   const data::NpcDatabase& npcs,
                                   const data::InitDatabase& init,
                                   CellInteractionRuntimeIndex& interactions)
        : work_(work), npcs_(npcs), interactions_(interactions),
          npcOverlap_(work, npcs), scenery_(work, init) {}

    // Return values are exactly the original routine: 1 accepted, 0 blocked,
    // -1 player scenery special. scanSuppressedF048 is both read during the
    // cell scan and set to 1 after a player type-8 portal, matching DS:F048.
    int evaluate(std::int32_t candidateYFixed,
                 std::int32_t candidateXFixed,
                 int moverId,
                 std::int32_t partyXFixed,
                 std::int32_t partyYFixed,
                 std::uint16_t currentLevel,
                 std::uint32_t& scanSuppressedF048,
                 const CellInteractionCallbacks& interactionCallbacks,
                 const CandidateCollisionPlayerCallbacks& playerCallbacks = {},
                 const SceneryCollisionCallbacks& sceneryCallbacks = {}) const;

private:
    const data::WorkLevelSnapshot& work_;
    const data::NpcDatabase& npcs_;
    CellInteractionRuntimeIndex& interactions_;
    NpcOverlapSystem npcOverlap_;
    SceneryCollisionSystem scenery_;
};

} // namespace ravenloft::game
