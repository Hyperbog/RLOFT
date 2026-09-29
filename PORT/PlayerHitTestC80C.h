#pragma once

#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <functional>
#include "games/ravenloft/game/OriginalRng30CA2.h"

namespace ravenloft::game {

struct PlayerHitTestContextC80C {
    std::function<int(int)> containerWeightF5F8;
};

struct PlayerHitTestResultC80C {
    bool hit=false;
    int roll=0;
    int required=0;
    int encumbranceTier=0;
    int strengthAdjustment=0;
    bool forcedActorHit=false;
};

// Direct reconstruction of CODE.2 0xC80C..0xC90C plus its 0xC7C4
// Strength-to-hit helper. This is the player-character -> NPC hit test used by
// 0xE5F0 normal hand attacks.
class PlayerHitTestC80C {
public:
    static PlayerHitTestResultC80C evaluate(std::size_t characterIndex,
                                            const data::CharacterRecord& character,
                                            const data::RuntimePlacement22& actor,
                                            const data::NpcDefinition& definition,
                                            int itemId,
                                            bool useStrengthAdjustment,
                                            const data::ObjectDatabase& objects,
                                            OriginalRng30CA2& rng,
                                            const PlayerHitTestContextC80C& context = {});

    static int strengthHitAdjustmentC7C4(const data::CharacterRecord& character);
};

} // namespace ravenloft::game
